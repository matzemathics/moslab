/*
 * Copyright (C) 2016-2017 Kernkonzept GmbH.
 * Author(s): Sarah Hoffmann <sarah.hoffmann@kernkonzept.com>
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 */
#pragma once

#include <l4/sys/compiler.h>

#if _MIPS_SZPTR == 32
# define L4UTIL_THREAD_START_LOAD_FUNC_ADDR(func) "  la $t9," func "\n"
#else
# define L4UTIL_THREAD_START_LOAD_FUNC_ADDR(func) "  dla $t9," func "\n"
#endif

#if _MIPS_SIM == _ABIO32
# define L4UTIL_THREAD_START_SETUP_GP \
"  bal 10f \n" \
"   nop    \n" \
"10:       \n" \
"  .cpload $31 \n"
#else
# define L4UTIL_THREAD_START_SETUP_GP \
" bal 10f \n" \
"  nop    \n" \
"10:      \n" \
"  .cpsetup $31, $25, 10b \n"
#endif

#define L4UTIL_THREAD_FUNC_MIPS_TEMPLATE(name, locality) \
EXTERN_C_BEGIN \
locality L4_NORETURN void name(void) \
{ \
  asm("  .set push \n" \
      "  .set noreorder \n" \
      L4UTIL_THREAD_START_SETUP_GP \
      L4UTIL_THREAD_START_LOAD_FUNC_ADDR(#name "_mips_helper_func") \
      "  jal $t9 \n" \
      "   nop    \n"\
      "  .set pop" \
  ); \
  __builtin_unreachable(); \
} \
static L4_NORETURN void __attribute__((used)) name ##_mips_helper_func(void); \
EXTERN_C_END \
static L4_NORETURN void name##_mips_helper_func(void)

#define L4UTIL_THREAD_STATIC_FUNC(name) \
        L4UTIL_THREAD_FUNC_MIPS_TEMPLATE(name, static)

#define L4UTIL_THREAD_FUNC(name) \
        L4UTIL_THREAD_FUNC_MIPS_TEMPLATE(name, )

#include_next <l4/util/thread.h>
