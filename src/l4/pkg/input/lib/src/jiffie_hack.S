
.global jiffies
#ifdef ARCH_amd64
jiffies = 0x6ffff000 + 0xa0
#elif ARCH_riscv
jiffies = 0x7fff0000 + 0xa0
#else
jiffies = 0xaffff000 + 0xa0
#endif
