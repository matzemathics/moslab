INTERFACE:

#include <types.h>

class Acpi_gas
{
public:
  enum Type { System_mem = 0, System_io = 1, Pci_cfg_mem = 2 };
  Unsigned8  id;
  Unsigned8  width;
  Unsigned8  offset;
  Unsigned8  access_size;
  Unsigned64 addr;
} __attribute__((packed));



class Acpi_table_head
{
public:
  char       signature[4];
  Unsigned32 len;
  Unsigned8  rev;
  Unsigned8  chk_sum;
  char       oem_id[6];
  char       oem_tid[8];
  Unsigned32 oem_rev;
  Unsigned32 creator_id;
  Unsigned32 creator_rev;

  bool checksum_ok() const;
} __attribute__((packed));


class Acpi_sdt
{
private:
  unsigned _num_tables;
  Acpi_table_head const **_tables;
};

class Acpi
{
public:
  static Acpi_sdt const *sdt() { return &_sdt; }

private:
  static Acpi_sdt _sdt;
  static bool _init_done;
};

class Acpi_madt : public Acpi_table_head
{
public:
  enum Type
  { LAPIC, IOAPIC, Irq_src_ovr, NMI, LAPIC_NMI, LAPIC_adr_ovr, IOSAPIC,
    LSAPIC, Irq_src, LOCAL_X2APIC = 9, LOCAL_X2APIC_NMI = 10,
    GICC = 0xb, GICD, GICM, GICR, ITS,
  };

  struct Apic_head
  {
    Unsigned8 type;
    Unsigned8 len;
  } __attribute__((packed));

  struct Lapic : public Apic_head
  {
    enum { ID = LAPIC };
    Unsigned8 apic_processor_id;
    Unsigned8 apic_id;
    Unsigned32 flags;
  } __attribute__((packed));

  struct Io_apic : public Apic_head
  {
    enum { ID = IOAPIC };
    Unsigned8 id;
    Unsigned8 res;
    Unsigned32 adr;
    Unsigned32 irq_base;
  } __attribute__((packed));

  struct Irq_source : public Apic_head
  {
    enum { ID = Irq_src_ovr };
    Unsigned8  bus;
    Unsigned8  src;
    Unsigned32 irq;
    Unsigned16 flags;
  } __attribute__((packed));

  struct Gic_cpu_if : public Apic_head
  {
    enum { ID = GICC };
    enum { Enabled = 0x1, Perf_irq_edge = 0x2, Vgic_irq_edge = 0x4 };
    Unsigned8 reserved[2];
    Unsigned32 cpu_if_num;
    Unsigned32 uid;
    Unsigned32 flags;
    Unsigned32 parking_protocol_version;
    Unsigned32 perf_gsiv;
    Unsigned64 parked_addr;
    Unsigned64 gicc_base;
    Unsigned64 gicv_base;
    Unsigned64 gich_base;
    Unsigned32 vgic_maintenance_irq;
    Unsigned64 gicr_base;
    Unsigned64 mpidr;
    Unsigned8  power_efficiency_class;
    Unsigned8  reserved2;
    Unsigned16 spe_overflow_irq;
  } __attribute__((packed));

  struct Gic_distributor_if : public Apic_head
  {
    enum { ID = GICD };
    enum { V1 = 1, V2, V3, V4 };
    Unsigned8   reserved[2];
    Unsigned32  id;
    Unsigned64  base;
    Unsigned32  reserved2;
    Unsigned8   version;
  } __attribute__((packed));

  struct Gic_redistributor_if : public Apic_head
  {
    enum { ID = GICR };
    Unsigned8   reserved[2];
    Unsigned64  base;
    Unsigned32  length;
  } __attribute__((packed));

  struct Gic_its_if : public Apic_head
  {
    enum { ID = ITS };
    Unsigned8   reserved[2];
    Unsigned32  id;
    Unsigned64  base;
  } __attribute__((packed));
  struct Local_x2apic : public Apic_head
  {
    enum { ID = LOCAL_X2APIC };
    Unsigned16 reserved;
    Unsigned32 local_apic_id;
    Unsigned32 lapic_flags;
    Unsigned32 uid;
  } __attribute__((packed));

public:
  Unsigned32 local_apic;
  Unsigned32 apic_flags;

private:
  char data[0];
} __attribute__((packed));

class Acpi_srat : public Acpi_table_head
{
public:
  struct Acpi_subtable_header
  {
    Unsigned8 type;
    Unsigned8 len;
  } __attribute__((packed));

  class Type
  {
  public:
    enum
    {
      Cpu_affinity        = 0,
      Memory_affinity     = 1,
      X2APIC_cpu_affinity = 2,
      Gicc_affinity       = 3,
    };
  };

  enum Cpu_affinity_flags
  {
    Cpu_use_affinity = 1,
  };

  struct Cpu_affinity : public Acpi_subtable_header
  {
    Unsigned8  proximity_domain_lo;
    Unsigned8  apic_id;
    Unsigned32 flags;
    Unsigned8  local_sapic_eid;
    Unsigned8  proximity_domain_hi[3];
    Unsigned32 clock_domain;
  } __attribute__((packed));

  enum Mem_affinity_flags
  {
    Mem_enabled       = 1,
    Mem_hot_pluggable = 1 << 1,
    Mem_non_volatile  = 1 << 2,
  };

  struct Mem_affinity : public Acpi_subtable_header
  {
    Unsigned32 proximity_domain;
    Unsigned16 reserved;           /* Reserved, must be zero */
    Unsigned64 base_Address;
    Unsigned64 length;
    Unsigned32 reserved1;
    Unsigned32 flags;
    Unsigned64 reserved2;          /* Reserved, must be zero */
  } __attribute__((packed));


  Unsigned32 table_revision;
  Unsigned64 reserved;

//private:
  char data[0];
};

template< bool >
struct Acpi_helper_get_msb
{ template<typename P> static Address msb(P) { return 0; } };

template<>
struct Acpi_helper_get_msb<true>
{ template<typename P> static Address msb(P p) { return p >> (sizeof(Address) * 8); } };


IMPLEMENTATION:

#include "boot_alloc.h"
#include "kip.h"
#include "kmem.h"
#include "panic.h"
#include "warn.h"
#include <cctype>

Acpi_sdt Acpi::_sdt;
bool Acpi::_init_done;

template< typename T >
class Acpi_sdt_p : public Acpi_table_head
{
public:
  T ptrs[0];

} __attribute__((packed));

typedef Acpi_sdt_p<Unsigned32> Acpi_rsdt_p;
typedef Acpi_sdt_p<Unsigned64> Acpi_xsdt_p;

class Acpi_rsdp
{
public:
  char       signature[8];
  Unsigned8  chk_sum;
  char       oem[6];
  Unsigned8  rev;
  Unsigned32 rsdt_phys;
  Unsigned32 len;
  Unsigned64 xsdt_phys;
  Unsigned8  ext_chk_sum;
  char       reserved[3];

  bool checksum_ok() const;

  static Acpi_rsdp const *locate();
} __attribute__((packed));


template<unsigned LEN>
static void
print_acpi_id(char const (&id)[LEN])
{
  char id_str[LEN];
  for (unsigned i = 0; i < LEN; ++i)
    id_str[i] = isalnum(id[i]) ? id[i] : '.';
  printf("%.*s", LEN, id_str);
}

PUBLIC void
Acpi_rsdp::print_info() const
{
  printf("ACPI: RSDP[%p]\tr%02x OEM:", static_cast<void const *>(this), rev);
  print_acpi_id(oem);
  printf("\n");
}

PUBLIC void
Acpi_table_head::print_info() const
{
  printf("ACPI: ");
  print_acpi_id(signature);
  printf("[%p]\tr%02x OEM:", static_cast<void const *>(this), rev);
  print_acpi_id(oem_id);
  printf(" OEMTID:");
  print_acpi_id(oem_tid);
  printf("\n");
}

PUBLIC
void
Acpi_sdt::print_summary() const
{
  for (unsigned i = 0; i < _num_tables; ++i)
    if (_tables[i])
      _tables[i]->print_info();
}

PUBLIC template< typename T >
unsigned
Acpi_sdt_p<T>::entries() const
{ return (len - sizeof(Acpi_table_head)) / sizeof(ptrs[0]); }

PUBLIC template< typename SDT >
void
Acpi_sdt::init(SDT *sdt)
{
  unsigned entries = sdt->entries();
  _tables = Boot_alloced::allocate<Acpi_table_head const *>(entries);
  if (_tables)
    {
      _num_tables = entries;
      for (unsigned i = 0; i < entries; ++i)
        if (sdt->ptrs[i])
          _tables[i] = this->map_entry(i, sdt->ptrs[i]);
        else
          _tables[i] = 0;
    }
}

PRIVATE static
void *
Acpi::_map_table_head(Unsigned64 phys)
{
  // is the acpi address bigger that our handled physical addresses
  if (Acpi_helper_get_msb<(sizeof(phys) > sizeof(Address))>::msb(phys))
    {
      printf("ACPI: cannot map phys address %llx, out of range (%ubit)\n",
             static_cast<unsigned long long>(phys),
             static_cast<unsigned>(sizeof(Address)) * 8);
      return 0;
    }

  Address t = Kmem::mmio_remap(phys, Config::PAGE_SIZE, true);
  if (t == Invalid_address)
    {
      printf("ACPI: cannot map phys address %llx, map failed\n",
             static_cast<unsigned long long>(phys));
      return 0;
    }

  return reinterpret_cast<void *>(t);
}

PUBLIC static
bool
Acpi::check_signature(char const *sig, char const *reference)
{
  for (; *reference; ++sig, ++reference)
    if (*reference != *sig)
      return false;

  return true;
}

PUBLIC static template<typename TAB>
TAB *
Acpi::map_table_head(Unsigned64 phys)
{ return reinterpret_cast<TAB *>(_map_table_head(phys)); }


PRIVATE template< typename T >
Acpi_table_head const *
Acpi_sdt::map_entry(unsigned idx, T phys)
{
  if (idx >= _num_tables)
    {
      printf("ACPI: table index out of range (%u >= %u)\n", idx, _num_tables);
      return 0;
    }

  return Acpi::map_table_head<Acpi_table_head>(static_cast<Unsigned64>(phys));
}



PUBLIC static
void
Acpi::init_virt()
{
  enum { Print_info = 0 };

  if (_init_done)
    return;
  _init_done = 1;

  if (Print_info)
    printf("ACPI-Init\n");

  Acpi_rsdp const *rsdp = Acpi_rsdp::locate();
  if (!rsdp)
    {
      WARN("ACPI: Could not find RSDP, skip init\n");
      return;
    }

  rsdp->print_info();

  if (rsdp->rev && rsdp->xsdt_phys)
    {
      Address p = Kmem::mmio_remap(rsdp->xsdt_phys, sizeof(Acpi_xsdt_p), true);
      if (p == ~0UL)
        WARN("ACPI: Could not map XSDT\n");
      else
        {
          Acpi_xsdt_p const *x = reinterpret_cast<Acpi_xsdt_p *>(p);
          if (!x->checksum_ok())
            WARN("ACPI: Checksum mismatch in XSDT\n");
          else
            {
              _sdt.init(x);
              if (Print_info)
                {
                  x->print_info();
                  _sdt.print_summary();

                  Acpi_srat const *srat = Acpi::find<Acpi_srat const *>("SRAT");
                  if (srat)
                    srat->show();
                }
              return;
            }
        }
    }

  if (rsdp->rsdt_phys)
    {
      Address p = Kmem::mmio_remap(rsdp->rsdt_phys, sizeof(Acpi_rsdt_p), true);
      if (p == ~0UL)
        WARN("ACPI: Could not map RSDT\n");
      else
        {
          Acpi_rsdt_p const *r = reinterpret_cast<Acpi_rsdt_p *>(p);
          if (!r->checksum_ok())
            WARN("ACPI: Checksum mismatch in RSDT\n");
          else
            {
              _sdt.init(r);
              if (Print_info)
                {
                  r->print_info();
                  _sdt.print_summary();

                  Acpi_srat const *srat = Acpi::find<Acpi_srat const *>("SRAT");
                  if (srat)
                    srat->show();
                }
            }
        }
    }
}

PUBLIC static
template< typename T >
T
Acpi::find(const char *s)
{
  init_virt();
  return static_cast<T>(sdt()->find(s));
}

IMPLEMENT
bool
Acpi_rsdp::checksum_ok() const
{
  // ACPI 1.0 checksum
  Unsigned8 sum = 0;
  for (unsigned i = 0; i < 20; i++)
    sum += *(reinterpret_cast<Unsigned8 const *>(this) + i);

  if (sum)
    return false;

  if (rev == 0)
    return true;

  // Extended Checksum
  for (unsigned i = 0; i < len; ++i)
    sum += *(reinterpret_cast<Unsigned8 const *>(this) + i);

  return !sum;
}

IMPLEMENT
bool
Acpi_table_head::checksum_ok() const
{
  Unsigned8 sum = 0;
  for (unsigned i = 0; i < len; ++i)
    sum += *(reinterpret_cast<Unsigned8 const *>(this) + i);

  return !sum;
}

PUBLIC
Acpi_table_head const *
Acpi_sdt::find(char const *sig) const
{
  for (unsigned i = 0; i < _num_tables; ++i)
    {
      Acpi_table_head const *t = _tables[i];
      if (!t)
	continue;

      if (Acpi::check_signature(t->signature, sig)
          && t->checksum_ok())
	return t;
    }

  return 0;
}

PUBLIC
Acpi_madt::Apic_head const *
Acpi_madt::find(Unsigned8 type, int idx) const
{
  for (unsigned i = 0; i < len-sizeof(Acpi_madt);)
    {
      Apic_head const *a = reinterpret_cast<Apic_head const *>(data + i);
      //printf("a=%p, a->type=%u, a->len=%u\n", a, a->type, a->len);
      if (a->type == type)
	{
	  if (!idx)
	    return a;
	  --idx;
	}
      i += a->len;
    }

  return 0;
}

PUBLIC template<typename T> inline
T const *
Acpi_madt::find(int idx) const
{
  return static_cast<T const *>(find(T::ID, idx));
}


PRIVATE static
Acpi_rsdp const *
Acpi_rsdp::locate_via_kip()
{
  // If we are booted from UEFI, bootstrap reads the RSDP pointer from
  // UEFI and creates a memory descriptor with sub type 5 for it
  for (auto const &md: Kip::k()->mem_descs_a())
    if (   md.type() == Mem_desc::Info
        && md.ext_type() == Mem_desc::Info_acpi_rsdp)
      {
        // Rover across first page in memory descriptor. The actual RSDP
        // address was rounded down to page alignment by bootstrap...
        for (unsigned off = 0; off < Config::PAGE_SIZE; off += sizeof(void*))
          {
            Acpi_rsdp const *r = Acpi::map_table_head<Acpi_rsdp>(md.start() + off);
            if (   Acpi::check_signature(r->signature, "RSD PTR ")
                && r->checksum_ok())
              return r;
          }

        panic("RSDP memory descriptor from bootstrap invalid");
      }

  return 0;
}

PUBLIC template<typename T> inline
T const *
Acpi_srat::find() const
{
  return 0;
}

PUBLIC
void
Acpi_srat::show() const
{
  printf("SRAT Table Revision: 0x%x (len: %d)\n",
         table_revision, len);

  unsigned l = 0;

  while (l < len - sizeof(Acpi_srat))
    {
      Acpi_subtable_header const *h
        = reinterpret_cast<Acpi_subtable_header const *>(data + l);

      switch (h->type)
        {
        case Type::Cpu_affinity:
            {
              Cpu_affinity const *c
                = reinterpret_cast<Cpu_affinity const *>(data + l);
              if (c->flags & Cpu_use_affinity)
                printf("  Cpu: prox_dom_lo,hi[3]=%x,%x,%x,%x apic_id=0x%x local_sapic_eid=0x%x clk_dom=0x%x\n",
                       c->proximity_domain_lo, c->proximity_domain_hi[0],
                       c->proximity_domain_hi[1], c->proximity_domain_hi[2],
                       c->apic_id, c->local_sapic_eid, c->clock_domain);
            }
          break;
        case Type::Memory_affinity:
            {
              Mem_affinity const *m
                = reinterpret_cast<Mem_affinity const *>(data + l);
              if (m->flags & Mem_enabled)
                printf("  Mem: Proxdomain=%d Baseaddr=%llx Len=%llx %s %s\n",
                       m->proximity_domain, m->base_Address, m->length,
                       (m->flags & Mem_hot_pluggable) ? "HotPluggable" : "Noplug",
                       (m->flags & Mem_non_volatile) ? "NonVolatile" : "Forgetting");
            }
          break;
        default:
          printf("Unhandled SRAT type %d\n", h->type);
          break;
        };

      l += h->len;
    }
}

// ------------------------------------------------------------------------
IMPLEMENTATION [ia32,amd64]:

#include "panic.h"

PRIVATE static
Acpi_rsdp const *
Acpi_rsdp::locate_in_region(Address start, Address end)
{
  for (Address p = start; p < end; p += 16)
    {
      Acpi_rsdp const* r = reinterpret_cast<Acpi_rsdp const *>(p);
      if (Acpi::check_signature(r->signature, "RSD PTR ")
          && r->checksum_ok())
        return r;
    }

  return 0;
}

IMPLEMENT
Acpi_rsdp const *
Acpi_rsdp::locate()
{
  enum
  {
    ACPI20_PC99_RSDP_START = 0x0e0000,
    ACPI20_PC99_RSDP_END   = 0x100000,

    BDA_EBDA_SEGMENT       = 0x00040E,
  };

  extern char ebda_segment[];

  if (Acpi_rsdp const *r = locate_via_kip())
    return r;

  if (Acpi_rsdp const *r = locate_in_region(ACPI20_PC99_RSDP_START,
                                            ACPI20_PC99_RSDP_END))
    return r;

  Address ebda = *reinterpret_cast<Unsigned16 const *>(ebda_segment) << 4;
  if (ebda > 0x80000 && ebda < 0x100000 - 1024) // sanity check
    if (Acpi_rsdp const *r = locate_in_region(ebda, ebda + 1024))
      return r;

  return 0;
}

// ------------------------------------------------------------------------
IMPLEMENTATION [arm]:

IMPLEMENT
Acpi_rsdp const *
Acpi_rsdp::locate()
{
  return locate_via_kip();
}
