INTERFACE [arm && 32bit]:

#define FIASCO_BOOT_PAGING_INFO \
  __attribute__((section(".bootstrap.info"), used))

// --------------------------------------------------
INTERFACE [arm && 32bit && mmu && !arm_lpae]:

struct Boot_paging_info
{
  void *kernel_page_directory;
};

//---------------------------------------------------
INTERFACE [arm && 32bit && mmu && arm_lpae]:

struct Boot_paging_info
{
  void *kernel_page_directory;
  void *kernel_lpae_dir;
};

//---------------------------------------------------
INTERFACE [arm && 32bit && !mmu]:

struct Boot_paging_info
{
  // Build fails if structure is empty...
  void *dummy;
};
