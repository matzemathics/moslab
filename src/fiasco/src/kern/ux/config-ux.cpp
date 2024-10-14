/*
 * Fiasco-UX
 * Architecture specific config code
 */

INTERFACE:

#include "idt_init.h"

#define TARGET_NAME "ux-x86"

EXTENSION class Config
{
public:
  enum
  {
    // cannot access user memory directly
    Access_user_mem = No_access_user_mem,

    SCHED_PIT = 0,

    Scheduler_one_shot = false,
    Scheduler_mode = SCHED_PIT,
    Scheduler_granularity = 10000U,
    Default_time_slice = 10 * Scheduler_granularity,

    scheduler_irq_vector	= 0x20U,

    // Size of the host address space, change the following if your host
    // address space size is different, do not go below 2GB (0x80000000)
    Host_as_size       = 0xc0000000U,

    Kip_clock_uses_rdtsc = false,
    Pic_prio_modify = true,
  };

  static const bool hlt_works_ok		= true;

  enum {
    Is_ux = 1,
  };
};

IMPLEMENTATION[ux]:

#include <feature.h>
KIP_KERNEL_FEATURE("io_prot");

const char *const Config::kernel_warn_config_string = 0;

IMPLEMENT_OVERRIDE inline ALWAYS_INLINE
constexpr unsigned Config::kmem_per_cent() { return 8; }

IMPLEMENT_OVERRIDE inline ALWAYS_INLINE
constexpr unsigned long Config::kmem_max() { return 64UL << 20; }

IMPLEMENT FIASCO_INIT
void
Config::init_arch()
{}
