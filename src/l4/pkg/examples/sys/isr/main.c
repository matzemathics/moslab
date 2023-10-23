/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>,
 *               Björn Döbel <doebel@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */
/*
 * This example shall show how to connect to an interrupt, receive interrupt
 * events and detach again. As the interrupt source we'll use the virtual
 * key interrupt -- pin 0 of the log capability.
 */

#include <l4/re/c/util/cap_alloc.h>
#include <l4/re/c/namespace.h>
#include <l4/sys/factory.h>
#include <l4/sys/icu.h>
#include <l4/sys/irq.h>
#include <l4/sys/vcon.h>
#include <l4/sys/utcb.h>

#include <stdio.h>

int main(void)
{
  int const irqno = 0;
  l4_cap_idx_t irqcap, icucap;
  l4_vcon_attr_t attr;
  long err;

  icucap = l4re_env()->log;

  /* Get a free capability slot for the ICU capability. */
  if (l4_is_invalid_cap(icucap))
    {
      printf("Did not find the Vlog ICU.\n");
      return 1;
    }

  /* Get another free capability slot for the corresponding IRQ object. */
  if (l4_is_invalid_cap(irqcap = l4re_util_cap_alloc()))
    {
      printf("Cannot allocate capability slot.\n");
      return 1;
    }

  /* Create IRQ object. */
  if ((err = l4_error(l4_factory_create_irq(l4re_env()->factory, irqcap))))
    {
      printf("Could not create IRQ object: %ld (%s).\n", err, l4sys_errtostr(err));
      return 1;
    }

  /*
   * Bind the recently allocated IRQ object to the IRQ number irqno as provided
   * by the ICU.
   */
  if ((err = l4_error(l4_icu_bind(icucap, irqno, irqcap))))
    {
      printf("Could not bind IRQ%d to ICU: %ld (%s).\n", irqno, err, l4sys_errtostr(err));
      return 1;
    }

  if ((err = l4_error(l4_vcon_get_attr(icucap, &attr))))
    {
      printf("Could not get Vcon attributes: %ld (%s).\n", err, l4sys_errtostr(err));
      return 1;
    }

  /* Disable echo at Vcon console. */
  attr.l_flags &= ~L4_VCON_ECHO;

  if ((err = l4_error(l4_vcon_set_attr(icucap, &attr))))
    {
      printf("Could not set Vcon attributes: %ld (%s).\n", err, l4sys_errtostr(err));
      return 1;
    }

  printf("Vcon echo disabled.\n");

  /* Bind ourselves to the IRQ. Define the IPC label which is sent if an IRQ
   * IPC arrives. */
  if ((err = l4_error(l4_rcv_ep_bind_thread(irqcap, l4re_env()->main_thread, 0x1234))))
    {
      printf("Could not bind to IRQ%d: %ld (%s).\n", irqno, err, l4sys_errtostr(err));
      return 1;
    }

  printf("Attached to key IRQ %d.\nPress keys now, Shift-Q to exit.\n", irqno);

  /* IRQ receive loop. */
  while (1)
    {
      /* Wait for the interrupt to happen. If we received an IRQ, the label
       * return code is set to 0. If we didn't receive an IRQ, the error flag
       * in the message tag is set and l4_error() reads the IPC error code from
       * the UTCB. */
      l4_umword_t label;
      if ((err = l4_error(l4_irq_wait(irqcap, &label, L4_IPC_NEVER))))
        printf("Could not receive IRQ: %ld (%s).\n", err, l4sys_errtostr(err));
      else
        {
          char buf[128];
          int n;

          if (label != 0x1234)
            {
              printf("Unexpected label %0lx -- ignoring interrupt.\n", label);
              continue;
            }

          /* Process the interrupt -- may do a 'break' */
          printf("Got IRQ with expected label 0x%lX.\n", label);
          n = l4_vcon_read(icucap, buf, sizeof(buf));
          if (n < 0)
            printf("Could not read from Vcon interface: %d (%s).\n", n, l4sys_errtostr(n));
          else
            {
              unsigned i;
              int terminate = 0;
              for (i = 0; i < (unsigned)n && i < sizeof(buf); ++i)
                {
                  int c = (unsigned char)buf[i];
                  if (c >= 32 && c < 128) // Filter UTF-8 encodings.
                    printf("Got key '%c'.\n", c);
                  else
                    printf("Got keycode %d.\n", c);
                  if (buf[i] == 'Q')
                    terminate = 1;
                }

              if (terminate)
                break;
            }
        }
    }

  /* We're done, detach from the interrupt. */
  if ((err = l4_error(l4_irq_detach(irqcap))))
    printf("Could not detach from IRQ: %ld (%s).\n", err, l4sys_errtostr(err));

  printf("Application terminated.\n");
  return 0;
}
