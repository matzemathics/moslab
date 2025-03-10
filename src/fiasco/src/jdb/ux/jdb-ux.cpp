INTERFACE [ux]:

#include "types.h"
#include "l4_types.h"

class Trap_state;
class Thread;
class Context;
class Jdb_entry_frame;
class Mem_space;

EXTENSION class Jdb
{
public:
  typedef enum
    {
      s_unknown, s_ipc, s_pagefault, s_fputrap,
      s_interrupt, s_timer_interrupt, s_slowtrap, s_user_invoke,
    } Guessed_thread_state;

  enum { MIN_SCREEN_HEIGHT = 20, MIN_SCREEN_WIDTH = 80 };

  template < typename T > static T peek(T const *addr, Address_type user);

  static int (*bp_test_log_only)();
  static int (*bp_test_break)(Cpu_number cpu, Jdb_entry_frame *ef,
                              String_buffer *buf);

private:
  static unsigned short rows, cols;

};

IMPLEMENTATION [ux]:

#include <cstdio>
#include <cstdlib>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "globals.h"
#include "initcalls.h"
#include "jdb_core.h"
#include "jdb_screen.h"
#include "kernel_console.h"
#include "kernel_task.h"
#include "kernel_thread.h"
#include "keycodes.h"
#include "kmem.h"
#include "libc_support.h"
#include "logdefs.h"
#include "mem_layout.h"
#include "push_console.h"
#include "simpleio.h"
#include "space.h"
#include "static_init.h"
#include "thread.h"
#include "thread_state.h"
#include "trap_state.h"
#include "usermode.h"
#include "vkey.h"

int (*Jdb::bp_test_log_only)();
int (*Jdb::bp_test_break)(Cpu_number cpu, Jdb_entry_frame *ef,
                          String_buffer *buf);

unsigned short Jdb::rows, Jdb::cols;

static Proc::Status jdb_irq_state;

IMPLEMENT inline
void
Jdb::enter_trap_handler(Cpu_number)
{
  conf_screen();

  // Set terminal raw mode
  enter_getchar();

  // Flush all output streams
  fflush (NULL);
}

IMPLEMENT inline
void
Jdb::leave_trap_handler(Cpu_number)
{
  // Restore terminal mode
  leave_getchar();

  // Flush all output streams
  fflush (NULL);

}

IMPLEMENT inline
bool
Jdb::handle_user_request(Cpu_number)
{ return false; }

IMPLEMENT inline
bool
Jdb::test_checksums()
{ return true; }


// disable interrupts before entering the kernel debugger
IMPLEMENT
void
Jdb::save_disable_irqs(Cpu_number cpu)
{
  assert(cpu == Cpu_number::boot_cpu());
  (void)cpu;
  jdb_irq_state = Proc::cli_save();
}

// restore interrupts after leaving the kernel debugger
IMPLEMENT
void
Jdb::restore_irqs(Cpu_number cpu)
{
  assert(cpu == Cpu_number::boot_cpu());
  (void)cpu;
  Proc::sti_restore(jdb_irq_state);
}

STATIC_INITIALIZE_P(Jdb,JDB_INIT_PRIO);

PUBLIC static FIASCO_INIT
void
Jdb::init()
{
  // Install JDB handler
  Trap_state::base_handler = &enter_jdb;

  // be sure that Push_console comes very first
  Kconsole::console()->register_console(push_cons()),

  register_libc_atexit(leave_getchar);
  atexit(leave_getchar);
}

PRIVATE
static void
Jdb::conf_screen()
{
  struct winsize win;
  ioctl (fileno (stdin), TIOCGWINSZ, &win);
  rows = win.ws_row;
  cols = win.ws_col;

  if (rows < MIN_SCREEN_HEIGHT || cols < MIN_SCREEN_WIDTH)
    printf ("%sTerminal probably too small, should be at least %dx%d!\033[0m\n",
	    esc_emph, MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);

  Jdb_screen::set_height(rows);
  Jdb_screen::set_width(cols);
}

/** handle int3 debug extension */
PUBLIC static inline NOEXPORT
bool
Jdb::int3_extension()
{
  auto cpu = Cpu_number::boot_cpu();
  Jdb_entry_frame *ef = Jdb::entry_frame.cpu(cpu);
  error_buffer.cpu(cpu).clear();

  if (ef->debug_entry_kernel_str())
    error_buffer.cpu(cpu).printf("%s", ef->text());
  else if (ef->debug_entry_user_str())
    error_buffer.cpu(cpu).printf("user \"%.*s\"", ef->textlen(), ef->text());
  else if (ef->debug_entry_kernel_sequence())
    return execute_command_ni(ef->text(), ef->textlen());
  else
    return false;

  return true;
}

static
bool
Jdb::handle_special_cmds(int)
{ return 1; }

IMPLEMENT
bool
Jdb::handle_debug_traps(Cpu_number cpu)
{
  error_buffer.cpu(cpu).clear();
  switch (entry_frame.cpu(cpu)->_trapno)
    {
      case 1:
        error_buffer.cpu(cpu).printf("Interception");
        break;
      case 3:
	if (!int3_extension())
          return false;
#ifdef FIXME
        if (get_thread(cpu)->d_taskno())
	  {
	    if (bp_test_log_only && bp_test_log_only())
	      return false;
	    if (bp_test_break
		&& bp_test_break(cp, &entry_frame, &error_buffer.cpu(cpu)))
	      break;
	  }
#endif
    }

  return true;
}

IMPLEMENT
void
Jdb::handle_nested_trap(Jdb_entry_frame *e)
{
  printf("Trap in JDB: IP:%08lx\n", e->ip());
}

IMPLEMENT inline
bool
Jdb::handle_conditional_breakpoint(Cpu_number, Jdb_entry_frame *)
{ return false; }


PUBLIC static
unsigned char *
Jdb::access_mem_task(Jdb_address addr, bool)
{
  Mem_space::Phys_addr phys;
  Mem_space::Page_order size;

  if (addr.is_kmem())
    {
      Address end_addr = Kernel_thread::init_done()
                         ? reinterpret_cast<Address>(&Mem_layout::end)
                         : reinterpret_cast<Address>(Mem_layout::initcall_end);

      // Kernel address.
      // We can directly access it via virtual addresses if it's kernel code
      // (which is always mapped, but doesn't appear in the kernel pagetable)
      //  or if we find a mapping for it in the kernel's master pagetable.
      return ((addr.addr() >= reinterpret_cast<Address>(&Mem_layout::load)
               && addr.addr() < end_addr)
              || Kernel_task::kernel_task()->virt_to_phys(addr.addr())
                 != Invalid_address)
             ? static_cast<unsigned char *>(addr.virt())
             : nullptr;
    }
  else if (addr.is_phys())
    {
      return nullptr;
    }
  else
    {
      // User address.
      // We can't directly access it because it's in a different host process
      // but if the task's pagetable has a mapping for it, we can translate
      // task-virtual -> physical -> kernel-virtual address and then access.
      Virt_addr va(addr.addr());
      return (static_cast<Mem_space *>(addr.space())
                ->v_lookup(va, &phys, &size, 0))
               ? static_cast<unsigned char *>(Kmem::phys_to_virt(
                 cxx::int_value<Mem_space::Phys_addr>(phys)
                 + cxx::int_value<Virt_size>(cxx::get_lsb(va, size))))
               : nullptr;
    }
}

IMPLEMENT inline NEEDS ["space.h"]
template <typename T>
T
Jdb::peek (T const *addr, Address_type user)
{
  // FIXME: assume UP here (current_meme_space(0))
  return Mem_space::current_mem_space(Cpu_number::boot_cpu())->peek(addr, user);
}

PUBLIC
static int
Jdb::is_adapter_memory(Jdb_address)
{
  return 0;
}

#define WEAK __attribute__((weak))
extern "C" char in_slowtrap, in_page_fault, in_handle_fputrap;
extern "C" char in_interrupt, in_timer_interrupt, in_timer_interrupt_slow;
extern "C" char i30_ret_switch WEAK, in_slow_ipc1 WEAK;
extern "C" char in_slow_ipc2 WEAK, in_slow_ipc4;
extern "C" char in_sc_ipc1 WEAK, in_sc_ipc2 WEAK;
#undef WEAK

/** Try to guess the thread state of t by walking down the kernel stack and
 * locking at the first return address we find. */
PUBLIC
static Jdb::Guessed_thread_state
Jdb::guess_thread_state(Thread *t)
{
  auto to_mword = [](void *addr) { return reinterpret_cast<Mword>(addr); };

  Guessed_thread_state state = s_unknown;
  Mword *ktop =
    offset_cast<Mword*>(context_of(t->get_kernel_sp()), Context::Size);

  for (int i=-1; i>-26; i--)
    {
      if (ktop[i] != 0)
	{
	  if (ktop[i] == to_mword(&in_page_fault))
	    state = s_pagefault;
	  if ((ktop[i] == to_mword(&i30_ret_switch)) ||// shortcut.S, int 0x30
	      (ktop[i] == to_mword(&in_slow_ipc1)) ||  // shortcut.S, int 0x30
	      (ktop[i] == to_mword(&in_slow_ipc4)) ||  // entry.S, int 0x30
	      (ktop[i] == to_mword(&in_sc_ipc1))   ||  // entry.S, int 0x30
	      (ktop[i] == to_mword(&in_sc_ipc2))   ||  // entry.S, sysenter
	     0)
	    state = s_ipc;
	  else if (ktop[i] == reinterpret_cast<Mword>(&Thread::user_invoke))
	    state = s_user_invoke;
	  else if (ktop[i] == to_mword(&in_handle_fputrap))
	    state = s_fputrap;
	  else if (ktop[i] == to_mword(&in_interrupt))
	    state = s_interrupt;
	  else if ((ktop[i] == to_mword(&in_timer_interrupt)) ||
		   (ktop[i] == to_mword(&in_timer_interrupt_slow)))
	    state = s_timer_interrupt;
	  else if (ktop[i] == to_mword(&in_slowtrap))
	    state = s_slowtrap;
	  if (state != s_unknown)
	    break;
	}
    }

  if (state == s_unknown && (t->state() & Thread_ipc_mask))
    state = s_ipc;

  return state;
}

// Don't make these members of Jdb else we have to include <termios.h>
// into the Jdb interface ...
static struct termios raw, new_raw;
static int    getchar_entered;

/** prepare Linux console for raw input */
PUBLIC static
void
Jdb::enter_getchar()
{
  if (!getchar_entered++)
    {
      tcgetattr (fileno (stdin), &raw);
      memcpy(&new_raw, &raw, sizeof(new_raw));
      new_raw.c_lflag    &= ~(ICANON|ECHO);
      new_raw.c_cc[VMIN]  = 0;
      new_raw.c_cc[VTIME] = 1;
      tcsetattr (fileno (stdin), TCSAFLUSH, &new_raw);
    }
}

/** restore Linux console. */
PUBLIC static
void
Jdb::leave_getchar()
{
  if (!--getchar_entered)
    tcsetattr (fileno (stdin), TCSAFLUSH, &raw);
}

//----------------------------------------------------------------------------
IMPLEMENTATION [ux && mp]:

static
void
Jdb::send_nmi(Cpu_number)
{
}
