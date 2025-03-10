/* SPDX-License-Identifier: GPL-2.0-only OR License-Ref-kk-custom */
/*
 * Copyright (C) 2018-2023 Kernkonzept GmbH.
 * Author(s): Adam Lackorzynski <adam@l4re.org>
 */

#include <l4/bid_config.h>

#include "panic.h"
#include "platform-arm.h"
#include <assert.h>
#include <stdio.h>

static inline unsigned current_el()
{
  l4_umword_t current_el;
  asm ("mrs %0, CurrentEL" : "=r" (current_el));
  return (current_el >> 2) & 3;
}

void Platform_arm::setup_kernel_config(l4_kernel_info_t *kip)
{
  setup_kernel_config_arm_common(kip);

  l4_kip_platform_info_arch *ia = &kip->platform_info.arch;

  asm("mrs %0, MIDR_EL1" : "=r" (ia->cpuinfo.MIDR));
  asm("mrs %0, MPIDR_EL1" : "=r" (ia->cpuinfo.MPIDR));
  asm("mrs %0, REVIDR_EL1" : "=r" (ia->cpuinfo.REVIDR));

  asm("mrs %0, ID_PFR0_EL1"  : "=r" (ia->cpuinfo.ID_PFR[0]));
  asm("mrs %0, ID_PFR1_EL1"  : "=r" (ia->cpuinfo.ID_PFR[1]));
  // Armv8.x... needs check
  //asm("mrs %0, ID_PFR2_EL1"  : "=r" (ia->cpuinfo.ID_PFR[2]));
  asm("mrs %0, ID_DFR0_EL1"  : "=r" (ia->cpuinfo.ID_DFR0));
  asm("mrs %0, ID_AFR0_EL1"  : "=r" (ia->cpuinfo.ID_AFR0));
  asm("mrs %0, ID_MMFR0_EL1" : "=r" (ia->cpuinfo.ID_MMFR[0]));
  asm("mrs %0, ID_MMFR1_EL1" : "=r" (ia->cpuinfo.ID_MMFR[1]));
  asm("mrs %0, ID_MMFR2_EL1" : "=r" (ia->cpuinfo.ID_MMFR[2]));
  asm("mrs %0, ID_MMFR3_EL1" : "=r" (ia->cpuinfo.ID_MMFR[3]));
  asm("mrs %0, ID_ISAR0_EL1" : "=r" (ia->cpuinfo.ID_ISAR[0]));
  asm("mrs %0, ID_ISAR1_EL1" : "=r" (ia->cpuinfo.ID_ISAR[1]));
  asm("mrs %0, ID_ISAR2_EL1" : "=r" (ia->cpuinfo.ID_ISAR[2]));
  asm("mrs %0, ID_ISAR3_EL1" : "=r" (ia->cpuinfo.ID_ISAR[3]));
  asm("mrs %0, ID_ISAR4_EL1" : "=r" (ia->cpuinfo.ID_ISAR[4]));
  asm("mrs %0, ID_ISAR5_EL1" : "=r" (ia->cpuinfo.ID_ISAR[5]));
  asm("mrs %0, ID_ISAR6_EL1" : "=r" (ia->cpuinfo.ID_ISAR[6]));
  //asm("mrs %0, S3_0_C0_C0_3" : "=r" (ia->cpuinfo.ID_MVFR[0]));
  //asm("mrs %0, S3_0_C0_C1_3" : "=r" (ia->cpuinfo.ID_MVFR[1]));
  //asm("mrs %0, S3_0_C0_C2_3" : "=r" (ia->cpuinfo.ID_MVFR[2]));
  asm("mrs %0, ID_AA64DFR0_EL1" : "=r" (ia->cpuinfo.ID_AA64DFR[0]));
  asm("mrs %0, ID_AA64DFR1_EL1" : "=r" (ia->cpuinfo.ID_AA64DFR[1]));
  asm("mrs %0, ID_AA64ISAR0_EL1" : "=r" (ia->cpuinfo.ID_AA64ISAR[0]));
  asm("mrs %0, ID_AA64ISAR1_EL1" : "=r" (ia->cpuinfo.ID_AA64ISAR[1]));
  asm("mrs %0, ID_AA64ISAR2_EL1" : "=r" (ia->cpuinfo.ID_AA64ISAR[2]));
  asm("mrs %0, ID_AA64MMFR0_EL1" : "=r" (ia->cpuinfo.ID_AA64MMFR[0]));
  asm("mrs %0, ID_AA64MMFR1_EL1" : "=r" (ia->cpuinfo.ID_AA64MMFR[1]));
  // Armv8.2
  //asm("mrs %0, ID_AA64MMFR2_EL1" : "=r" (ia->cpuinfo.ID_AA64MMFR[2]));
  asm("mrs %0, ID_AA64PFR0_EL1" : "=r" (ia->cpuinfo.ID_AA64PFR[0]));
  asm("mrs %0, ID_AA64PFR1_EL1" : "=r" (ia->cpuinfo.ID_AA64PFR[1]));

  assert(kernel_type != EL_Support::Unknown);
  if (kernel_type == EL_Support::EL2 && current_el() < 2)
    panic("Kernel requires EL2 (virtualization) but running in EL1.");
}

