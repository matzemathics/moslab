/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */
#include <l4/sys/err.h>
#include <l4/sys/types.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>

#include <cstring>
#include <stdio.h>

#include <l4/logging-server/logging.h>
 
int main()
{
  L4::Cap<Logger> server = L4Re::Env::env()->get_cap<Logger>("logging_server");
  if (!server.is_valid())
    {
      printf("Could not get server capability!\n");
      return 1;
    }

  int return_code = 0;
  if ((return_code = server->log_all("abcdefghijklmnopqrstuvwxyz")) != 0)
    {
      printf("Error talking to server: %d\n", return_code);
      return 1;
    }

  return 0;
}
