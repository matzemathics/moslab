#ifndef _ASSERT_H
#define _ASSERT_H

#include <cdefs.h>

#ifdef NDEBUG
# define assert(expr) ((void) 0)
# define check(expr)  (void)(expr)
#else
# include <stdio.h>

__BEGIN_DECLS
void
assert_fail(char const *expr_msg, char const *file, unsigned int line,
            void *caller);
__END_DECLS

# define assert(expression) \
    do \
      if (__builtin_expect(!(expression), 0)) \
        assert_fail(#expression, __FILE__, __LINE__, \
                    __builtin_return_address(0)); \
    while (0)
# define check(expr) assert(expr)
#endif

#endif
