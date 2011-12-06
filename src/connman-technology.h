#ifndef CONNMAN_TECHNOLOGY_H_
#define CONNMAN_TECHNOLOGY_H_

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ConnmanTechnology      ConnmanTechnology;
typedef struct _ConnmanTechnologyClass ConnmanTechnologyClass;

struct _ConnmanTechnologyClass {
  GObjectClass parent_class;
};

const gchar *connman_technology_get_name(ConnmanTechnology *technology);

gboolean connman_technology_enable_tethering(ConnmanTechnology *technology,
                                             const gchar *ssid,
                                             const gchar *secret);
gboolean connman_technology_disable_tethering(ConnmanTechnology *technology);

ConnmanTechnology *connman_technology_new(GBusType bus_type,
                                          const gchar *path);

GType connman_technology_get_type (void);
#define CONNMAN_TYPE_TECHNOLOGY             connman_technology_get_type()
#define CONNMAN_TECHNOLOGY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONNMAN_TYPE_TECHNOLOGY, ConnmanTechnology))
#define CONNMAN_TECHNOLOGY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), CONNMAN_TYPE_TECHNOLOGY, ConnmanTechnologyClass))
#define CONNMAN_IS_TECHNOLOGY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONNMAN_TYPE_TECHNOLOGY))
#define CONNMAN_IS_TECHNOLOGY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), CONNMAN_TYPE_TECHNOLOGY))
#define CONNMAN_TECHNOLOGY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), CONNMAN_TYPE_TECHNOLOGY, ConnmanTechnologyClass))

G_END_DECLS


#endif /* CONNMAN_TECHNOLOGY_H_ */
