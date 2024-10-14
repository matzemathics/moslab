/*
 * Copyright (C) 2014-2016 Kernkonzept GmbH.
 * Author(s): Steffen Liebergeld <steffen.liebergeld@kernkonzept.com>
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 */

#pragma once

#include <l4/sys/capability>
#include <l4/sys/types.h>
#include <l4/sys/vcpu_context>
#include "vm.h"

class Vmx: public Vm
{
public:
  Vmx();

protected:
  bool npt_available() { return true; }
  bool cpu_virt_capable();
  void initialize_vmcb(unsigned long_mode);
  unsigned handle_vmexit();
  void set_rax(l4_umword_t rax);
  void set_rsp(l4_umword_t rsp);
  void set_rflags(l4_umword_t rflags);
  void set_rip(l4_umword_t rip);
  void set_cr0(l4_umword_t cr0);
  void set_cr3(l4_umword_t cr3);
  void set_cr4(l4_umword_t cr4);
  void set_dr7(l4_umword_t dr7);
  void set_efer(l4_umword_t efer);
  l4_umword_t get_rax();
  void enable_npt();
  void disable_npt();

private:
  void vmwrite(unsigned int field, l4_uint64_t val)
  {
    l4_vm_vmx_write(vmcb, field, val);
  }

  l4_uint64_t vmread(unsigned int field)
  {
    return l4_vm_vmx_read(vmcb, field);
  }

  l4_uint16_t vmread_16(unsigned int field)
  {
    return l4_vm_vmx_read_16(vmcb, field);
  }

  l4_uint64_t vmread_64(unsigned int field)
  {
    return l4_vm_vmx_read_64(vmcb, field);
  }

  l4_uint32_t vmread_32(unsigned int field)
  {
    return l4_vm_vmx_read_32(vmcb, field);
  }

  l4_umword_t vmread_nat(unsigned int field)
  {
    return l4_vm_vmx_read_nat(vmcb, field);
  }

  void jump_over_current_insn(unsigned bytes);

  L4::Cap<L4::Vcpu_context> vmcs_cap;
};
