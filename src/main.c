/*
 *  Connection Manager example
 *
 *  Copyright (C) 2011  Daniel Mack <daniel@zonque.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <glib.h>

#include "connman-manager.h"

int main(int argc, char **argv)
{
  GMainLoop *loop;
  ConnmanManager *manager;
  GBusType bus_type;

  g_type_init();

  if (geteuid() == 0)
    {
      g_message("Running as root, using the system bus");
      bus_type = G_BUS_TYPE_SYSTEM;
    }
  else
    {
      g_message("Running as user, using the session bus");
      bus_type = G_BUS_TYPE_SESSION;
    }

  manager = connman_manager_new(bus_type);

  if (manager == NULL)
    {
      g_error("Failed to create manager object");
      return -1;
    }

  return 0;
}

