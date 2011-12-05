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
#include <gio/gio.h>

#include "connman-generated.h"
#include "connman-manager.h"

struct _ConnmanManager {
  GObject parent;

  ConnmanProxyManager *proxy;
};

static GObjectClass *parent_class = NULL;

enum {
  SIGNAL_LAST
};

static gint signals[SIGNAL_LAST];

static void
connman_manager_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
connman_manager_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

ConnmanManager *connman_manager_new(GBusType bus_type)
{
  ConnmanManager *manager = g_object_new(CONNMAN_TYPE_MANAGER, NULL);
  GError *error = NULL;

  manager->proxy = connman_proxy_manager_proxy_new_for_bus_sync(bus_type,
                                                                G_DBUS_PROXY_FLAGS_NONE,
                                                                "net.connman.Manager",
                                                                "/net/connman/Manager",
                                                                NULL, /* cancelable */
                                                                &error);
  if (error)
    {
      g_error(error->message);
      g_error_free(error);
      return NULL;
    }

  return manager;
}

static void
connman_manager_init (ConnmanManager *manager)
{
}

static void
connman_manager_finalize (GObject *object)
{
  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
connman_manager_class_init (ConnmanManagerClass *klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass *) g_type_class_peek_parent (klass);
  object_class = (GObjectClass *) klass;

  object_class->get_property = connman_manager_get_property;
  object_class->set_property = connman_manager_set_property;
  object_class->finalize = connman_manager_finalize;
}

G_DEFINE_TYPE (ConnmanManager, connman_manager, G_TYPE_OBJECT)

