/* SPDX-License-Identifier: GPL-2.0-only OR License-Ref-kk-custom */
/*
 * Copyright (C) 2023, 2021 Kernkonzept GmbH.
 * Author(s): Frank Mehnert <frank.mehnert@kernkonzept.com>
 */

#include <l4/sys/types.h>
#include <cstdio>

/**
 * Same function as l4_timeout_from_us() with subtracting `e = log2(us) - 7`.
 */
static l4_timeout_s custom_l4_timeout_from_us_man7(l4_uint32_t us)
{
  l4_timeout_s t;
  unsigned m;
  int e = (31 - __builtin_clz(us)) - 7;
  if (e < 0) e = 0;
  m = us >> e;
  t.t = (e << 10) | m;
  return t;
}

/**
 * Same function as l4_timeout_from_us() with subtracting `e = log2(us) - 9`.
 */
static l4_timeout_s custom_l4_timeout_from_us_man9(l4_uint32_t us)
{
  l4_timeout_s t;
  unsigned m;
  int e = (31 - __builtin_clz(us)) - 9;
  if (e < 0) e = 0;
  m = us >> e;
  t.t = (e << 10) | m;
  return t;
}

static void deviation(l4_uint32_t us, l4_uint32_t d,
                      unsigned *value, unsigned *order, bool verbose = false)
{
  unsigned v, o;
  if (d == 0)
    {
      *value = 0;
      *order = 0;
      return;
    }
  for (v = us; d < 10000; d *= 10, v *= 10)
    if (verbose)
      printf("1: d=%d, v=%d\n", d, v);
  for (o = 0; (d*10 / v) < 99; v /= 10, ++o)
    if (verbose)
      printf("2: v=%d, o=%d\n", v, o);
  *value = d / v;
  *order = o - 1;
  if (verbose)
    printf("%d / %d\n", d / v, o - 1);
}

static void enumerate_timeouts()
{
  printf("Enumerating all value timeout values...\n");
  // 0xffffffff == L4_TIMEOUT_NEVER, see l4_timeout_from_us().
  for (l4_uint32_t us = 1; us < ~0U; ++us)
    {
      if ((us % 50000000) == 1)
        printf("  Finished %llu%%\n",
               ((l4_uint64_t)us * 100) / ((1ULL << 32) - 1));
      l4_timeout_s to1 = custom_l4_timeout_from_us_man7(us);
      l4_uint32_t res1 = l4_timeout_rel_get(to1);
      l4_timeout_s to2 = custom_l4_timeout_from_us_man9(us);
      l4_uint32_t res2 = l4_timeout_rel_get(to2);
      // The resulting time periods in microseconds are always smaller than or
      // equal to the original time period. The bigger the result, the closer
      // to the original time period.
      if (   res2 < res1 // custom_l4_timeout_from_us_man9() worse than
                         // custom_l4_timeout_from_us_man7()
          || res1 > us   // calculated timeout unexpectedly bigger
          || res2 > us)  // calculated timeout unexpectedly bigger
        {
          // Unexpected: Show the results for both ways.
          unsigned v1, o1;
          deviation(us, us - res1, &v1, &o1);
          unsigned v2, o2;
          deviation(us, us - res2, &v2, &o2);
          printf("%8d: res1: 0x%04x e=%d => %8d; res2: 0x%04x e=%d => %8d",
                 us,
                 to1.t, (to1.t & 0x7c00) >> 10, res1,
                 to2.t, (to2.t & 0x7c00) >> 10, res2);
          if (v1 || v2)
            {
              if (!v1)
                printf("        ");
              else
                printf("  %d.%u-E%u", v1 / 10, v1 % 10, o1);
              if (v2)
                printf(" vs %d.%u-E%u", v2 / 10, v2 % 10, o2);
            }
          printf("\n");
        }
    }
  printf("Done!\n");
}

int main()
{
  enumerate_timeouts();
  return 0;
}
