/*
 *  Connection Service example
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
#include "connman-service.h"

struct _ConnmanService {
  GObject parent;

  gchar *object_path;

  /* properties */
  guchar strength;
  GSList nameservers;
  guint state;
  gboolean passphrase_required;
  gboolean login_required;
  gboolean auto_connect;
  gboolean immutable;
  gboolean favorite;
  gchar *name;
  gint type;
};

static GObjectClass *parent_class = NULL;

enum {
  SIGNAL_LAST
};

static gint signals[SIGNAL_LAST];

static void
connman_service_get_property (GObject    *object,
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
connman_service_set_property (GObject      *object,
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

gboolean connman_service_has_object_path(ConnmanService *service,
                                         const gchar *path)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);

  return g_str_equal(service->object_path, path);
}

ConnmanService *connman_service_new(GVariant *variant)
{
  ConnmanService *service = g_object_new(CONNMAN_TYPE_SERVICE, NULL);
  GVariant *child = g_variant_get_child_value(variant, 0);
  GVariant *attrs;
  gsize i;

  service->object_path = g_variant_dup_string(child, NULL);
  attrs = g_variant_get_child_value(variant, 1);

  for (i = 0; i < g_variant_n_children(attrs); i++)
    {
      GVariant *attr = g_variant_get_child_value(attrs, i);
      GVariant *key_v = g_variant_get_child_value(attr, 0);
      GVariant *val_v = g_variant_get_child_value(attr, 1);
      GVariant *val = g_variant_get_variant(val_v);
      const gchar *key = g_variant_get_string(key_v, NULL);

      if (g_str_equal(key, "Name"))
        service->name =  g_variant_dup_string(val, NULL);

      if (g_str_equal(key, "Type"))
        {
          const gchar *v = g_variant_get_string(val, NULL);
          
          if (g_str_equal(v, "wifi"))
            service->type = CONNMAN_SERVICE_TYPE_WIFI;
          
          if (g_str_equal(v, "ethernet"))
            service->type = CONNMAN_SERVICE_TYPE_ETHERNET;
        }

      if (g_str_equal(key, "LoginRequired"))
        service->login_required = g_variant_get_boolean(val);

      if (g_str_equal(key, "PassphraseRequired"))
        service->passphrase_required = g_variant_get_boolean(val);

      if (g_str_equal(key, "AutoConnect"))
        service->auto_connect = g_variant_get_boolean(val);
      
      if (g_str_equal(key, "Immutable"))
        service->immutable = g_variant_get_boolean(val);
      
      if (g_str_equal(key, "Favorite"))
        service->favorite = g_variant_get_boolean(val);
      
      if (g_str_equal(key, "Strength"))
        service->strength = g_variant_get_byte(val);
    }

  g_message("new service %p: '%s' (%d)", service, service->name, service->type);

  return service;
}

const gchar *connman_service_get_name(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), NULL);
  return service->name;
}

guint connman_service_get_strength(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), CONNMAN_SERVICE_TYPE_UNKNOWN);
  return service->strength;
}

gboolean connman_service_login_required(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);
  return service->login_required;
}

gboolean connman_service_passphrase_required(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);
  return service->passphrase_required;
}

gboolean connman_service_passphrase_is_favorite(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);
  return service->favorite;
}

gboolean connman_service_passphrase_is_immutable(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);
  return service->immutable;
}

gboolean connman_service_type_wifi(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);
  return service->type == CONNMAN_SERVICE_TYPE_WIFI;
}

gboolean connman_service_type_ethernet(ConnmanService *service)
{
  g_return_val_if_fail(CONNMAN_IS_SERVICE(service), FALSE);
  return service->type == CONNMAN_SERVICE_TYPE_ETHERNET;
}


static void
connman_service_init (ConnmanService *service)
{
}

static void
connman_service_finalize (GObject *object)
{
  ConnmanService *service = CONNMAN_SERVICE(object);

  g_free(service->object_path);
  service->object_path = NULL;
  
  g_free(service->name);
  service->name = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
connman_service_class_init (ConnmanServiceClass *klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass *) g_type_class_peek_parent (klass);
  object_class = (GObjectClass *) klass;

  object_class->get_property = connman_service_get_property;
  object_class->set_property = connman_service_set_property;
  object_class->finalize = connman_service_finalize;
}

G_DEFINE_TYPE (ConnmanService, connman_service, G_TYPE_OBJECT)

