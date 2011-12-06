#ifndef CONNMAN_SERVICE_H_
#define CONNMAN_SERVICE_H_

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ConnmanService      ConnmanService;
typedef struct _ConnmanServiceClass ConnmanServiceClass;

enum {
  CONNMAN_SERVICE_TYPE_UNKNOWN = 0,
  CONNMAN_SERVICE_TYPE_ETHERNET,
  CONNMAN_SERVICE_TYPE_WIFI,
  CONNMAN_SERVICE_TYPE_MAX = CONNMAN_SERVICE_TYPE_WIFI
};

struct _ConnmanServiceClass {
  GObjectClass parent_class;
};

const gchar *connman_service_get_name(ConnmanService *service);
guint connman_service_get_strength(ConnmanService *service);
gboolean connman_service_login_required(ConnmanService *service);
gboolean connman_service_passphrase_required(ConnmanService *service);
gboolean connman_service_passphrase_is_favorite(ConnmanService *service);
gboolean connman_service_passphrase_is_immutable(ConnmanService *service);
gboolean connman_service_type_wifi(ConnmanService *service);
gboolean connman_service_type_ethernet(ConnmanService *service);

gboolean connman_service_has_object_path(ConnmanService *service,
                                         const gchar *path);
ConnmanService *connman_service_new(GVariant *variant);

GType connman_service_get_type (void);
#define CONNMAN_TYPE_SERVICE             connman_service_get_type()
#define CONNMAN_SERVICE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONNMAN_TYPE_SERVICE, ConnmanService))
#define CONNMAN_SERVICE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), CONNMAN_TYPE_SERVICE, ConnmanServiceClass))
#define CONNMAN_IS_SERVICE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONNMAN_TYPE_SERVICE))
#define CONNMAN_IS_SERVICE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), CONNMAN_TYPE_SERVICE))
#define CONNMAN_SERVICE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), CONNMAN_TYPE_SERVICE, ConnmanServiceClass))

G_END_DECLS


#endif /* CONNMAN_SERVICE_H_ */
