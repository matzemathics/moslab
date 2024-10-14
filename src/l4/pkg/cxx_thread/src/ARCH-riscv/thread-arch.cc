/* SPDX-License-Identifier: GPL-2.0-only or License-Ref-kk-custom */
/*
 * Copyright (C) 2021 Kernkonzept GmbH.
 * Author(s): Georg Kotheimer <georg.kotheimer@kernkonzept.com>
 */

#include <l4/cxx/thread>

void L4_cxx_start(void);

void L4_cxx_start(void)
{
  asm volatile (".global L4_Thread_start_cxx_thread \n"
                "L4_Thread_start_cxx_thread:        \n"
#if __riscv_xlen == 32
                "lw a0, 4(sp)                       \n"
#else
                "ld a0, 8(sp)                       \n"
#endif
                "j L4_Thread_execute                \n");
}

void L4_cxx_kill(void);

void L4_cxx_kill(void)
{
  asm volatile (".global L4_Thread_kill_cxx_thread \n"
                "L4_Thread_kill_cxx_thread:        \n"
#if __riscv_xlen == 32
                "lw a0, 4(sp)                       \n"
#else
                "ld a0, 8(sp)                       \n"
#endif
                "j L4_Thread_shutdown              \n");
}
