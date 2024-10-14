IMPLEMENTATION[arm && mmu && !cpu_virt]:

#include "config.h"
#include "globals.h"
#include "kmem.h"
#include "kmem_space.h"

PRIVATE inline NEEDS["globals.h", "kmem.h"]
Kernel_task::Kernel_task()
: Task(Ram_quota::root, Kmem::kdir, Caps::none())
{}

PUBLIC static inline NEEDS["kmem_space.h"]
void
Kernel_task::map_syscall_page(void *p)
{
  auto pte = Kmem::kdir->walk(Virt_addr(Kmem_space::Syscalls),
                              Pdir::Depth, true,
                              Kmem_alloc::q_allocator(Ram_quota::root.unwrap()));

  if (pte.level == 0) // allocation of second level failed
    panic("Error mapping syscall page to %p",
          reinterpret_cast<void *>(Kmem_space::Syscalls));

  pte.set_page(Phys_mem_addr(Kmem::kdir->virt_to_phys(reinterpret_cast<Address>(p))),
               Page::Attr::kern_global(Page::Rights::URX()));
  pte.write_back_if(true);
  Mem_unit::tlb_flush_kernel(Kmem_space::Syscalls);
}

IMPLEMENTATION[arm && mmu && cpu_virt]:

#include "config.h"
#include "globals.h"
#include "kmem.h"

PRIVATE inline NEEDS["globals.h", "kmem.h"]
Kernel_task::Kernel_task()
: Task(Ram_quota::root, reinterpret_cast<Pdir*>(Kmem::kdir.unwrap()), Caps::none())
{}

PUBLIC static inline
void
Kernel_task::map_syscall_page(void *p)
{
  Mem_space::set_syscall_page(p);
}

//---------------------------------------------------------------------------
IMPLEMENTATION[arm && mpu]:

#include "config.h"
#include "globals.h"
#include "kmem.h"

PRIVATE inline NEEDS["globals.h", "kmem.h"]
Kernel_task::Kernel_task()
: Task(Ram_quota::root, static_cast<Pdir*>(Kmem::kdir), Caps::none())
{}
