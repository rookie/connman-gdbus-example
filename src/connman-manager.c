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
#include "connman-service.h"
#include "connman-technology.h"

struct _ConnmanManager {
  GObject parent;

  GBusType bus_type;
  ConnmanProxyManager *proxy;
  GSList *services;
  GSList *technologies;
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

static void
manager_state_changed(ConnmanProxyManager *proxy,
                      ConnmanManager      *manager)
{
}

static GVariant *connman_manager_get_properties(ConnmanManager *manager)
{
  GError *error = NULL;
  GVariant *ret;

  connman_proxy_manager_call_get_properties_sync(manager->proxy,
                                                 &ret, NULL, &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return NULL;
    }

  return ret;
}

static gboolean service_in_list(ConnmanManager *manager,
                                GVariant       *child)
{
  GSList *iter;
  GVariant *o = g_variant_get_child_value(child, 0);
  const gchar *path = g_variant_get_string(o, NULL);

  for (iter = manager->services; iter; iter = iter->next)
    {
      ConnmanService *service = CONNMAN_SERVICE(iter->data);

      if (connman_service_has_object_path(service, path))
        return TRUE;
    }

  return FALSE;
}

gboolean connman_manager_update_services(ConnmanManager *manager)
{
  GError *error = NULL;
  GVariant *services;
  GSList *iter, *remove_list = NULL;
  gsize i;

  connman_proxy_manager_call_get_services_sync(manager->proxy,
                                               &services, NULL, &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return FALSE;
    }

  /* look for added services */
  for (i = 0; i < g_variant_n_children(services); i++)
    {
      GVariant *child = g_variant_get_child_value(services, i);

      if (!service_in_list(manager, child))
        {
          ConnmanService *service;

          service = connman_service_new(child);
          manager->services = g_slist_append(manager->services, service);
        }
    }

  /* look for removed services */
  for (iter = manager->services; iter; iter = iter->next)
    {
      ConnmanService *service = CONNMAN_SERVICE(iter->data);
      gboolean found = FALSE;

      for (i = 0; i < g_variant_n_children(services); i++)
        {
          GVariant *child = g_variant_get_child_value(services, i);
          GVariant *o = g_variant_get_child_value(child, 0);
          const gchar *path = g_variant_get_string(o, NULL);

          if (connman_service_has_object_path(service, path))
            {
              found = TRUE;
              break;
            }
        }

      if (!found)
        remove_list = g_slist_append(remove_list, service);
    }

  /* 
   * do the actual remove of services in an extra loop, so we don't
   * alter the list we're walking
   */
  for (iter = remove_list; iter; iter = iter->next)
    {
      ConnmanService *service = CONNMAN_SERVICE(iter->data);
      manager->services = g_slist_remove(manager->services, service);
      g_object_unref(G_OBJECT(service));
    }

  return TRUE;
}

gboolean connman_manager_update_technologies (ConnmanManager *manager)
{
  GVariant *props = connman_manager_get_properties(manager);
  gsize i;
  
  for (i = 0; i < g_variant_n_children(props); i++)
    {
      GVariant *prop = g_variant_get_child_value(props, i);
      GVariant *key_v = g_variant_get_child_value(prop, 0);
      const gchar *key = g_variant_get_string(key_v, NULL);

      if (g_str_equal(key, "Technologies"))
        {
          GVariant *v = g_variant_get_child_value(prop, 1);
          GVariant *array = g_variant_get_variant(v);
          gsize j;

          for (j = 0; j <  g_variant_n_children(array); j++)
            {
              GVariant *item = g_variant_get_child_value(array, j);
              const gchar *path = g_variant_get_string(item, NULL);
              ConnmanTechnology *technology;
             
              technology = connman_technology_new(manager->bus_type, path);
              manager->technologies = g_slist_append(manager->technologies,
                                                     technology);
            }
        }
    }

  return TRUE;
}

gboolean connman_manager_is_online (ConnmanManager *manager)
{
  GVariant *props = connman_manager_get_properties(manager);
  gsize i;
  
  for (i = 0; i < g_variant_n_children(props); i++)
    {
      GVariant *prop = g_variant_get_child_value(props, i);
      GVariant *key_v = g_variant_get_child_value(prop, 0);
      const gchar *key = g_variant_get_string(key_v, NULL);

      if (g_str_equal(key, "OfflineMode"))
        {
          GVariant *v = g_variant_get_child_value(prop, 1);
          GVariant *va = g_variant_get_variant(v);
          gboolean offline = g_variant_get_boolean(va);

          return !offline;
        }
    }

  return FALSE;
}

static void
service_connect_callback (GObject *source_object,
                          GAsyncResult *res,
                          gpointer user_data)
{
  ConnmanManager *manager = CONNMAN_MANAGER(user_data);
  GError *error = NULL;
  gchar *ret;

  connman_proxy_manager_call_connect_service_finish(manager->proxy,
                                                    &ret, res, &error);
  if (error)
    {
      g_critical("Unable to connect to service: %s", error->message);
      g_error_free(error);
    }
  else
    {
      g_message("%s(): connected to %s", __func__, ret);
    }
}

gboolean connman_manager_connect_service (ConnmanManager *manager,
                                          ConnmanService *service,
                                          const gchar *secret)
{
  GVariantBuilder *builder;
  GError *error = NULL;
  GVariant *variant;
  const gchar *ssid = connman_service_get_name(service);

  g_message("Connecting to service '%s'", connman_service_get_name(service));

  if (!connman_service_type_wifi(service))
    {
      g_critical("Service is not a Wifi service");
      return FALSE;
    }

  builder = g_variant_builder_new(G_VARIANT_TYPE_ARRAY);

  g_variant_builder_add(builder, "{sv}", "Type", g_variant_new_string("wifi"));
  g_variant_builder_add(builder, "{sv}", "Mode", g_variant_new_string("managed"));
  g_variant_builder_add(builder, "{sv}", "SSID", g_variant_new_string(ssid));

  if (secret)
    {
      g_variant_builder_add(builder, "{sv}", "Security", g_variant_new_string("rsn"));
      g_variant_builder_add(builder, "{sv}", "Passphrase", g_variant_new_string(secret));
    }
  else
    {
      g_variant_builder_add(builder, "{sv}", "Security", g_variant_new_string("none"));
    }

  variant = g_variant_builder_end(builder);

  connman_proxy_manager_call_connect_service(manager->proxy,
                                             variant, NULL,
                                             service_connect_callback, manager);
  g_variant_unref(variant);

  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return FALSE;
    }
  
  return TRUE;
}

static ConnmanTechnology *find_wifi_technology (ConnmanManager *manager)
{
  GSList *iter;

  for (iter = manager->technologies; iter; iter = iter->next)
    {
      ConnmanTechnology *t = CONNMAN_TECHNOLOGY(iter->data);

      if (g_str_equal("wifi", connman_technology_get_name(t)))
        return t;
    }

  return NULL;
}

gboolean connman_manager_enable_tethering (ConnmanManager *manager,
                                           const gchar *bssid,
                                           const gchar *secret)
{
  ConnmanTechnology *wifi = find_wifi_technology(manager);

  if (!wifi)
    {
      g_critical("No wifi technology, can not enable tethering");
      return FALSE;
    }

  return connman_technology_enable_tethering(wifi, bssid, secret);
}

gboolean connman_manager_disable_tethering (ConnmanManager *manager)
{
  ConnmanTechnology *wifi = find_wifi_technology(manager);

  if (!wifi)
    {
      g_critical("No wifi technology, can not disable tethering");
      return FALSE;
    }

  return connman_technology_disable_tethering(wifi);
}

GSList *connman_manager_get_services (ConnmanManager *manager)
{
  g_return_val_if_fail(CONNMAN_IS_MANAGER(manager), NULL);

  return manager->services;
}

ConnmanManager *connman_manager_new (GBusType bus_type)
{
  GError *error = NULL;
  ConnmanManager *manager = g_object_new(CONNMAN_TYPE_MANAGER, NULL);

  manager->services = NULL;
  manager->technologies = NULL;
  manager->bus_type = bus_type;

  manager->proxy = connman_proxy_manager_proxy_new_for_bus_sync(bus_type,
                                                                G_DBUS_PROXY_FLAGS_NONE,
                                                                "net.connman", "/",
                                                                NULL, /* cancelable */
                                                                &error);
  if (error)
    {
      g_error("%s", error->message);
      g_error_free(error);
      return NULL;
    }

  g_signal_connect(G_OBJECT(manager->proxy), "state-changed",
                   G_CALLBACK(manager_state_changed), manager);

  connman_manager_update_technologies(manager);
  connman_manager_update_services(manager);

  g_message("%d services", g_slist_length(manager->services));

  return manager;
}

static void
connman_manager_init (ConnmanManager *manager)
{
}

static void
connman_manager_finalize (GObject *object)
{
  ConnmanManager *manager = CONNMAN_MANAGER(object);

  g_slist_foreach(manager->technologies, (GFunc) g_object_unref, NULL);
  g_slist_free(manager->technologies);
  manager->technologies = NULL;

  g_slist_foreach(manager->services, (GFunc) g_object_unref, NULL);
  g_slist_free(manager->services);
  manager->services = NULL;

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

