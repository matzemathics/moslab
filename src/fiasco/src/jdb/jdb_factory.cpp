IMPLEMENTATION:

#include <climits>
#include <cstring>
#include <cstdio>

#include "jdb.h"
#include "jdb_core.h"
#include "jdb_module.h"
#include "jdb_screen.h"
#include "jdb_kobject.h"
#include "jdb_obj_info.h"
#include "kernel_console.h"
#include "keycodes.h"
#include "factory.h"
#include "simpleio.h"
#include "static_init.h"

class Jdb_factory : public Jdb_kobject_handler
{
public:
  Jdb_factory() FIASCO_INIT;
};

IMPLEMENT
Jdb_factory::Jdb_factory()
  : Jdb_kobject_handler(static_cast<Factory *>(nullptr))
{
  Jdb_kobject::module()->register_handler(this);
}

PUBLIC
bool
Jdb_factory::show_kobject(Kobject_common *, int ) override
{
  return true;
}

PUBLIC
void
Jdb_factory::show_kobject_short(String_buffer *buf, Kobject_common *o, bool) override
{
  Factory *t = cxx::dyn_cast<Factory*>(o);
  buf->printf(" c=%lu l=%lu", t->current(), t->limit());
}

PUBLIC
bool
Jdb_factory::info_kobject(Jobj_info *i, Kobject_common *o) override
{
  Factory *t = cxx::dyn_cast<Factory*>(o);
  i->type = i->factory.Type;
  i->factory.current = t->current();
  i->factory.limit = t->limit();
  return true;
}

static Jdb_factory jdb_factory INIT_PRIORITY(JDB_MODULE_INIT_PRIO);
