/*
 * Fiasco
 * Floating point unit code
 */

INTERFACE:

#include "initcalls.h"
#include "per_cpu_data.h"
#include "types.h"

class Context;
class Fpu_state;
class Trap_state;


class Fpu
{
public:
  // all the following methods are arch dependent
  static void init(Cpu_number cpu, bool) FIASCO_INIT_CPU_AND_PM;

  static unsigned state_size();
  static unsigned state_align();
  static void init_state(Fpu_state *);
  static void restore_state(Fpu_state const *);
  static void save_state(Fpu_state *);
  static void copy_state(Fpu_state *, Fpu_state const *);

  static Per_cpu<Fpu> fpu;
};

// ----------------------------------------------------------------------
INTERFACE [fpu && lazy_fpu]:

EXTENSION class Fpu
{
public:
  Context *owner() const { return _owner; }
  void set_owner(Context *owner) { _owner = owner; }
  bool is_owner(Context *owner) const { return _owner == owner; }

private:
  Context *_owner;
};

//---------------------------------------------------------------------------
IMPLEMENTATION:

DEFINE_PER_CPU Per_cpu<Fpu> Fpu::fpu;


//---------------------------------------------------------------------------
IMPLEMENTATION [!fpu]:

IMPLEMENT inline
void Fpu::init_state(Fpu_state *)
{}

IMPLEMENT inline
unsigned Fpu::state_size()
{ return 0; }

IMPLEMENT inline
unsigned Fpu::state_align()
{ return 1; }

IMPLEMENT
void Fpu::init(Cpu_number, bool)
{}

IMPLEMENT inline
void Fpu::save_state(Fpu_state *)
{}

IMPLEMENT inline
void Fpu::restore_state(Fpu_state const *)
{}

IMPLEMENT inline
void Fpu::copy_state(Fpu_state *, Fpu_state const *)
{}

PUBLIC static inline
void Fpu::disable()
{}

PUBLIC static inline
void Fpu::enable()
{}

//---------------------------------------------------------------------------
IMPLEMENTATION [fpu]:

#include <cstring>

IMPLEMENT_DEFAULT inline NEEDS[<cstring>]
void Fpu::copy_state(Fpu_state *to, Fpu_state const *from)
{
  memcpy(to, from, Fpu::state_size());
}
