//----------------------------------------------------------------------
INTERFACE [arm && arm_v8]:

#include "alternatives.h"

EXTENSION class Cpu
{
public:
  enum Scr_bits
  {
    Scr_ns   = 1UL <<  0, ///< Non-Secure mode
    Scr_irq  = 1UL <<  1, ///< IRQ to EL3
    Scr_fiq  = 1UL <<  2, ///< FIQ to EL3
    Scr_ea   = 1UL <<  3, ///< External Abort and SError to EL3
    Scr_smd  = 1UL <<  7, ///< SMC disable
    Scr_hce  = 1UL <<  8, ///< HVC enable at EL1, EL2, and EL3
    Scr_sif  = 1UL <<  9, ///< Secure instruction fetch enable
    Scr_rw   = 1UL << 10, ///< EL2 / EL1 is AArch64
    Scr_st   = 1UL << 11, ///< Trap Secure EL1 access to timer to EL3
    Scr_twi  = 1UL << 12, ///< Trap WFI to EL3
    Scr_twe  = 1UL << 13, ///< Trap WFE to EL3
    Scr_apk  = 1UL << 16, ///< Do not trap on Pointer Authentication key accesses
    Scr_api  = 1UL << 17, ///< Do not trap on Pointer Authentication instructions
    Scr_eel2 = 1UL << 18, ///< Secure EL2 enable
  };

  enum {
    Sctlr_m       = 1UL << 0,
    Sctlr_a       = 1UL << 1,
    Sctlr_c       = 1UL << 2,
    Sctlr_sa      = 1UL << 3,
    Sctlr_sa0     = 1UL << 4,
    Sctlr_cp15ben = 1UL << 5,
    Sctlr_itd     = 1UL << 7,
    Sctlr_sed     = 1UL << 8,
    Sctlr_uma     = 1UL << 9,
    Sctlr_i       = 1UL << 12,
    Sctlr_dze     = 1UL << 14,
    Sctlr_uct     = 1UL << 15,
    Sctlr_ntwi    = 1UL << 16,
    Sctlr_ntwe    = 1UL << 18,
    Sctlr_wxn     = 1UL << 19,
    Sctlr_e0e     = 1UL << 24,
    Sctlr_ee      = 1UL << 25,
    Sctlr_uci     = 1UL << 26,

    Sctlr_el1_res = (1UL << 11) | (1UL << 20) | (3UL << 22) | (3UL << 28),

    Sctlr_el1_generic = Sctlr_c
                    | Sctlr_cp15ben
                    | Sctlr_i
                    | Sctlr_dze
                    | Sctlr_uct
                    | Sctlr_uci
                    | Sctlr_el1_res,
  };

  enum : Mword
  {
    Cptr_el2_generic    = 0x33ffUL, // Reserved(RES1): 0-9, 12-13
    Cptr_el2_tfp        = 1UL << 10, // Trap advanced SIMD and floating-point
    Cptr_el2_tta        = 1UL << 20, // Trap accesses to trace registers

    // Trap advanced SIMD and floating-point instructions at both EL0 and EL1.
    Cpacr_el1_fpen_full = 3UL << 20,
  };

  struct has_aarch32_el1 : public Alternative_static_functor<has_aarch32_el1>
  {
    static bool probe()
    {
      Mword pfr0;
      asm ("mrs %0, ID_AA64PFR0_EL1": "=r" (pfr0));
      return pfr0 & 0x20;
    }
  };
};

//----------------------------------------------------------------------
INTERFACE [arm && !arm_sve]:

EXTENSION class Cpu
{
public:
  enum : Mword
  {
    // When we run at EL2 we have to make sure that CPACR_EL1.FPEN is 3 when
    // user-mode runs with HCR.TGE = 1, otherwise we get undefined instruction
    // exceptions instead of FPU traps into EL2.
    Cpacr_el1_generic_hyp = Cpacr_el1_fpen_full,
  };
};

//----------------------------------------------------------------------
INTERFACE [arm && arm_sve]:

EXTENSION class Cpu
{
public:
  enum : Mword
  {
    Cptr_el3_ez            = 1UL << 8, // Do not trap SVE instructions.

    Cptr_el2_tz            = 1UL << 8, // Trap SVE instructions.

    // Trap advanced SVE instructions at both EL0 and EL1.
    Cpacr_el1_zen_full     = 3UL << 16,
    // When we run at EL2 we have to make sure that CPACR_EL1.FPEN is 3 and
    // CPACR_EL1.ZEN is 3 when user-mode runs with HCR.TGE = 1, otherwise we get
    // undefined instruction exceptions instead of FPU/SVE traps into EL2.
    Cpacr_el1_generic_hyp  = Cpacr_el1_zen_full | Cpacr_el1_fpen_full,

    Zcr_vl_128             = 0,
    Zcr_vl_2048            = 15,
    Zcr_vl_max             = Zcr_vl_2048,
    Zcr_vl_mask            = 0xf,
  };
};


//--------------------------------------------------------------------------
IMPLEMENTATION [arm && !cpu_virt]:

EXTENSION class Cpu
{
public:
  enum
  {
    Sctlr_generic = Sctlr_el1_generic
                    | Sctlr_m
                    | (Config::Cp15_c1_use_alignment_check ?  Sctlr_a : 0)
  };

  enum : Unsigned64
  {
    Scr_default_bits = Scr_ns | Scr_rw | Scr_smd,
  };
};

PRIVATE inline
unsigned
Cpu::asid_bits() const
{ return (_cpu_id._mmfr[0] & 0xf0U) == 0x20 ? 16 : 8; }

IMPLEMENT_OVERRIDE
void
Cpu::init_supervisor_mode(bool)
{
  extern char exception_vector[];
  asm volatile ("msr VBAR_EL1, %0" : : "r"(&exception_vector));

  if (asid_bits() < Mem_unit::Asid_bits)
    panic("ASID size too small: HW provides %d bits, configured %d bits!",
          asid_bits(), Mem_unit::Asid_bits);
}

//--------------------------------------------------------------------------
IMPLEMENTATION [arm && cpu_virt]:

EXTENSION class Cpu
{
public:
  enum : Unsigned64
  {
    Scr_default_bits = Scr_ns | Scr_rw | Scr_smd | Scr_hce,
  };

  enum : Unsigned64
  {
    Hcr_must_set_bits = Hcr_vm | Hcr_swio | Hcr_ptw
                      | Hcr_amo | Hcr_imo | Hcr_fmo
                      | Hcr_tidcp | Hcr_tsc | Hcr_tactlr,

    /**
     * HCR value to be used for the VMM.
     *
     * The AArch64 VMM is currently running in EL1.
     */
    Hcr_host_bits = Hcr_must_set_bits | Hcr_rw | Hcr_dc,

    /**
     * HCR value to be used for normal threads.
     *
     * On AArch64 (with virtualization support) running in EL1.
     */
    Hcr_non_vm_bits = Hcr_must_set_bits | Hcr_rw | Hcr_dc | Hcr_tsw
                      | Hcr_ttlb | Hcr_tvm | Hcr_trvm
  };

  enum
  {
    Sctlr_res = (3UL << 4)  | (1UL << 11) | (1UL << 16)
              | (1UL << 18) | (3UL << 22) | (3UL << 28),

    Sctlr_generic = Sctlr_m
                    | (Config::Cp15_c1_use_alignment_check ?  Sctlr_a : 0)
                    | Sctlr_c
                    | Sctlr_i
                    | Sctlr_res,
  };

  enum {
    Mdcr_hpmn_mask = 0xf,
    Mdcr_tpmcr     = 1UL << 5,
    Mdcr_tpm       = 1UL << 6,
    Mdcr_hpme      = 1UL << 7,
    Mdcr_tde       = 1UL << 8,
    Mdcr_tda       = 1UL << 9,
    Mdcr_tdosa     = 1UL << 10,
    Mdcr_tdra      = 1UL << 11,

    Mdcr_bits      = Mdcr_tpmcr | Mdcr_tpm
                     | Mdcr_tda | Mdcr_tdosa | Mdcr_tdra,
    Mdcr_vm_mask   = 0xf00,
  };
};

PUBLIC inline
unsigned
Cpu::supported_pa_range() const
{
  static Unsigned8 const pa_range[16] = { 32, 36, 40, 42, 44, 48, 52 };
  return pa_range[_cpu_id._mmfr[0] & 0x0fU];
}

PRIVATE inline
unsigned
Cpu::vmid_bits() const
{ return (_cpu_id._mmfr[1] & 0xf0U) == 0x20 ? 16 : 8; }

IMPLEMENT_OVERRIDE
void
Cpu::init_hyp_mode()
{
  extern char exception_vector[];

  // Feature availability check for IPA address space size
  if (supported_pa_range() < phys_bits())
    panic("IPA address size too small: HW provides %d bits, required %d bits!",
          supported_pa_range(), phys_bits());

  if (vmid_bits() < Mem_unit::Asid_bits)
    panic("VMID size too small: HW provides %d bits, configured %d bits!",
          vmid_bits(), Mem_unit::Asid_bits);

  asm volatile ("msr VBAR_EL2, %x0" : : "r"(&exception_vector));
  asm volatile ("msr VTCR_EL2, %x0" : :
                "r"(  (1UL << 31) // RES1
                    | (Page::Tcr_attribs << 8)
                    | Page::Vtcr_bits));

  asm volatile ("msr MDCR_EL2, %x0" : : "r"((Mword)Mdcr_bits));

  asm volatile ("msr SCTLR_EL1, %x0" : : "r"((Mword)Sctlr_el1_generic));
  asm volatile ("msr HCR_EL2, %x0" : : "r" (Hcr_non_vm_bits));
  asm volatile ("msr HSTR_EL2, %x0" : : "r" (Hstr_non_vm));

  Mem::dsb();
  Mem::isb();

  // HCPTR
  asm volatile("msr CPTR_EL2, %x0" : : "r" (Cptr_el2_generic | Cptr_el2_tta));
}
//--------------------------------------------------------------------------
IMPLEMENTATION [arm]:

PUBLIC static inline
bool
Cpu::has_generic_timer()
{ return true; }

PUBLIC inline
bool
Cpu::has_sve() const
{ return ((_cpu_id._pfr[0] >> 32) & 0xf) == 1; }

PUBLIC static inline
Mword
Cpu::midr()
{
  Mword m;
  asm volatile ("mrs %0, midr_el1" : "=r" (m));
  return m;
}

PUBLIC static inline
Mword
Cpu::mpidr()
{
  Mword mpid;
  asm volatile("mrs %0, mpidr_el1" : "=r"(mpid));
  return mpid;
}

IMPLEMENT
void
Cpu::id_init()
{
  __asm__("mrs %0, ID_AA64PFR0_EL1": "=r" (_cpu_id._pfr[0]));
  __asm__("mrs %0, ID_AA64PFR1_EL1": "=r" (_cpu_id._pfr[1]));
  __asm__("mrs %0, ID_AA64DFR0_EL1": "=r" (_cpu_id._dfr0));
  __asm__("mrs %0, ID_AA64DFR1_EL1": "=r" (_cpu_id._afr0));
  __asm__("mrs %0, ID_AA64MMFR0_EL1": "=r" (_cpu_id._mmfr[0]));
  __asm__("mrs %0, ID_AA64MMFR1_EL1": "=r" (_cpu_id._mmfr[1]));
}

IMPLEMENT
void Cpu::early_init()
{
  early_init_platform();

  Mem_unit::flush_cache();
}

PUBLIC static inline
void
Cpu::enable_dcache()
{
  Mword r;
  asm volatile("mrs     %0, SCTLR_EL1 \n"
               "orr     %0, %0, %1    \n"
               "msr     SCTLR_EL1, %0 \n"
               : "=&r" (r) : "r" ((Mword)(Sctlr_c | Sctlr_i)));
}

PUBLIC static inline
void
Cpu::disable_dcache()
{
  Mword r;
  asm volatile("mrs     %0, SCTLR_EL1 \n"
               "bic     %0, %0, %1    \n"
               "msr     SCTLR_EL1, %0 \n"
               : "=&r" (r) : "r" ((Mword)(Sctlr_c | Sctlr_i)));
}

//--------------------------------------------------------------------------
IMPLEMENTATION [arm && arm_v8]:

PUBLIC static inline
void
Cpu::enable_smp()
{}

PUBLIC static inline
void
Cpu::disable_smp()
{}

PUBLIC static inline
Unsigned64
Cpu::hcr()
{
  Unsigned64 r;
  asm volatile ("mrs %0, HCR_EL2" : "=r"(r));
  return r;
}

PUBLIC static inline
void
Cpu::hcr(Unsigned64 hcr)
{
  asm volatile ("msr HCR_EL2, %0" : : "r"(hcr));
}

//--------------------------------------------------------------------------
IMPLEMENTATION [arm && arm_v8 && arm_sve]:

PUBLIC static inline
unsigned
Cpu::sve_vl()
{
  Mword vl;
  asm volatile (".arch_extension sve\n"
                "rdvl %0, #1" : "=r"(vl));
  // rdvl returns the vector length in bytes, but we measure the vector length
  // in quad-words (128-bits).
  return vl / 16;
}

PUBLIC static inline
Mword
Cpu::zcr_el1()
{
  Unsigned64 r;
  asm volatile (".arch_extension sve\n"
                "mrs %0, ZCR_EL1" : "=r"(r));
  return r;
}

PUBLIC static inline
void
Cpu::zcr_el1(Unsigned64 zcr)
{
  asm volatile (".arch_extension sve\n"
                "msr ZCR_EL1, %0" : : "r"(zcr));
}

//--------------------------------------------------------------------------
IMPLEMENTATION [arm && arm_v8 && arm_sve && cpu_virt]:

PUBLIC static inline
Mword
Cpu::zcr()
{
  Unsigned64 r;
  asm volatile (".arch_extension sve\n"
                "mrs %0, ZCR_EL2" : "=r"(r));
  return r;
}

PUBLIC static inline
void
Cpu::zcr(Unsigned64 zcr)
{
  asm volatile (".arch_extension sve\n"
                "msr ZCR_EL2, %0" : : "r"(zcr));
}

//--------------------------------------------------------------------------
IMPLEMENTATION [arm && arm_v8 && arm_sve && !cpu_virt]:

PUBLIC static inline
Mword
Cpu::zcr()
{
  return zcr_el1();
}

PUBLIC static inline
void
Cpu::zcr(Unsigned64 zcr)
{
  return zcr_el1(zcr);
}
