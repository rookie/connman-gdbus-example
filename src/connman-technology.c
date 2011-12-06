/*
 *  Connection Technology example
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
#include "connman-technology.h"

struct _ConnmanTechnology {
  GObject parent;

  ConnmanProxyTechnology *proxy;
  gchar *type;
};

static GObjectClass *parent_class = NULL;

enum {
  SIGNAL_LAST
};

static gint signals[SIGNAL_LAST];

static void
connman_technology_get_property (GObject    *object,
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
connman_technology_set_property (GObject      *object,
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

static GVariant *connman_technology_get_properties(ConnmanTechnology *technology)
{
  GError *error = NULL;
  GVariant *ret;

  connman_proxy_technology_call_get_properties_sync(technology->proxy,
                                                    &ret, NULL, &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return NULL;
    }

  return ret;
}

gboolean connman_technology_enable_tethering(ConnmanTechnology *technology,
                                             const gchar *ssid,
                                             const gchar *secret)
{
  GError *error = NULL;

  g_message("Enabling tethering with ssid '%s' on %p", ssid, technology);

  connman_proxy_technology_call_set_property_sync(technology->proxy,
                                                  "TetheringIdentifier",
                                                  g_variant_new_variant(g_variant_new_string(ssid)),
                                                  NULL, &error);
  connman_proxy_technology_call_set_property_sync(technology->proxy,
                                                  "TetheringPassphrase",
                                                  g_variant_new_variant(g_variant_new_string(secret)),
                                                  NULL, &error);
  connman_proxy_technology_call_set_property_sync(technology->proxy,
                                                  "Tethering",
                                                  g_variant_new_variant(g_variant_new_boolean(TRUE)),
                                                  NULL, &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return FALSE;
    }

  return TRUE;
}

gboolean connman_technology_disable_tethering(ConnmanTechnology *technology)
{
  GError *error = NULL;

  g_message("Disabling tethering");

  connman_proxy_technology_call_set_property_sync(technology->proxy,
                                                  "Tethering",
                                                  g_variant_new_boolean(FALSE),
                                                  NULL, &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return FALSE;
    }

  return TRUE;
}

ConnmanTechnology *connman_technology_new(GBusType bus_type,
                                          const gchar *path)
{
  ConnmanTechnology *technology = g_object_new(CONNMAN_TYPE_TECHNOLOGY, NULL);
  GError *error = NULL;
  GVariant *props;
  gsize i;

  technology->proxy = connman_proxy_technology_proxy_new_for_bus_sync(bus_type,
                                                                      G_DBUS_PROXY_FLAGS_NONE,
                                                                      "net.connman", path,
                                                                      NULL, /* cancelable */
                                                                      &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return NULL;
    }

  props = connman_technology_get_properties(technology);

  for (i = 0; i < g_variant_n_children(props); i++)
    {
      GVariant *prop = g_variant_get_child_value(props, i);
      GVariant *key_v = g_variant_get_child_value(prop, 0);
      GVariant *val_v = g_variant_get_child_value(prop, 1);
      GVariant *val = g_variant_get_variant(val_v);
      const gchar *key = g_variant_get_string(key_v, NULL);

      if (g_str_equal(key, "Type"))
        technology->type = g_variant_dup_string(val, NULL);
    }

  g_message("new technology %p: '%s'", technology, technology->type);

  return technology;
}

const gchar *connman_technology_get_name(ConnmanTechnology *technology)
{
  g_return_val_if_fail(CONNMAN_IS_TECHNOLOGY(technology), NULL);
  return technology->type;
}

static void
connman_technology_init (ConnmanTechnology *technology)
{
}

static void
connman_technology_finalize (GObject *object)
{
  ConnmanTechnology *technology = CONNMAN_TECHNOLOGY(object);

  g_free(technology->type);
  technology->type = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
connman_technology_class_init (ConnmanTechnologyClass *klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass *) g_type_class_peek_parent (klass);
  object_class = (GObjectClass *) klass;

  object_class->get_property = connman_technology_get_property;
  object_class->set_property = connman_technology_set_property;
  object_class->finalize = connman_technology_finalize;
}

G_DEFINE_TYPE (ConnmanTechnology, connman_technology, G_TYPE_OBJECT)

