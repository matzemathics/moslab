
#include "construction.h"
#include "types.h"

typedef void (*ctor_t)(void);

void run_ctor_functions(ctor_function_t *start, ctor_function_t *end)
{
  for (;start != end; ++start)
    (*start)();
}

void static_construction()
{
  extern ctor_function_t __INIT_ARRAY_START__[];
  extern ctor_function_t __INIT_ARRAY_END__[];
  run_ctor_functions(__INIT_ARRAY_START__, __INIT_ARRAY_END__);

  extern ctor_function_t __CTOR_END__[];
  extern ctor_function_t __CTOR_LIST__[];
  run_ctor_functions(__CTOR_LIST__, __CTOR_END__);
}
