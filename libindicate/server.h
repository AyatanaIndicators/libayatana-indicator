
#ifndef INDICATE_SERVER_H_INCLUDED__
#define INDICATE_SERVER_H_INCLUDED__ 1

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/* Boilerplate */
#define INDICATE_TYPE_SERVER (indicate_server_get_type ())
#define INDICATE_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_SERVER, IndicateServer))
#define INDICATE_IS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_SERVER))
#define INDICATE_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), INDICATE_TYPE_SERVER, IndicateServerClass))
#define INDICATE_IS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), INDICATE_TYPE_SERVER))
#define INDICATE_SERVER_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), INDICATE_TYPE_SERVER, IndicateServerClass))

typedef struct _IndicateServer IndicateServer;

#include "indicator.h"

struct _IndicateServer {
	GObject parent;

	gchar * path;
	GSList * indicators;
	gboolean visible;
	guint current_id;

	// TODO: Should have a more robust way to track this, but this'll work for now
	guint num_hidden;
};

typedef struct _IndicateServerClass IndicateServerClass;
struct _IndicateServerClass {
	GObjectClass parent;

	/* Signals */
	void (* indicator_added) (IndicateServer * server, guint id, gchar * type);
	void (* indicator_removed) (IndicateServer * server, guint id, gchar * type);
	void (* indicator_modified) (IndicateServer * server, guint id, gchar * property);

	/* Virtual Functions */
	gboolean (*get_desktop) (IndicateServer * server, gchar ** desktop_path, GError **error);
	gboolean (*get_indicator_count) (IndicateServer * server, guint * count, GError **error);
	gboolean (*get_indicator_count_by_type) (IndicateServer * server, gchar * type, guint * count, GError **error);
	gboolean (*get_indicator_list) (IndicateServer * server, GArray ** indicators, GError ** error);
	gboolean (*get_indicator_list_by_type) (IndicateServer * server, gchar * type, guint ** indicators, GError ** error);
	gboolean (*get_indicator_property) (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
	gboolean (*get_indicator_property_group) (IndicateServer * server, guint id, gchar ** properties, gchar *** value, GError **error);
	gboolean (*get_indicator_properties) (IndicateServer * server, guint id, gchar *** properties, GError **error);
	gboolean (*show_indicator_to_user) (IndicateServer * server, guint id, GError ** error);
	guint    (*get_next_id) (IndicateServer * server);
};

GType indicate_server_get_type (void) G_GNUC_CONST;

/* Create a new server */
IndicateServer * indicate_server_new (void);

/* Sets the object.  By default this is /org/freedesktop/indicators */
void indicate_server_set_dbus_object (const gchar * obj);

/* Sets the desktop file to get data like name and description
 * out of */
void indicate_server_set_desktop_file (const gchar * path);

/* Show and hide the server on DBus, this allows for the server to
 * be created, change the object, and then shown.  If for some
 * reason the app wanted to hide all it's indicators, this is a
 * sure fire way to do so.  No idea why, but I'm sure I'll learn. */
void indicate_server_show (IndicateServer * server);
void indicate_server_hide (IndicateServer * server);

guint indicate_server_get_next_id (IndicateServer * server);
void indicate_server_add_indicator (IndicateServer * server, IndicateIndicator * indicator);
void indicate_server_remove_indicator (IndicateServer * server, IndicateIndicator * indicator);

IndicateServer * indicate_server_ref_default (void);
void indicate_server_set_default (IndicateServer * server);

/* DBus API */
gboolean indicate_server_get_desktop (IndicateServer * server, gchar ** desktop_path, GError **error);
gboolean indicate_server_get_indicator_count (IndicateServer * server, guint * count, GError **error);
gboolean indicate_server_get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error);
gboolean indicate_server_get_indicator_list (IndicateServer * server, GArray ** indicators, GError ** error);
gboolean indicate_server_get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error);
gboolean indicate_server_get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
gboolean indicate_server_get_indicator_property_group (IndicateServer * server, guint id, gchar ** properties, gchar *** value, GError **error);
gboolean indicate_server_get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error);
gboolean indicate_server_show_indicator_to_user (IndicateServer * server, guint id, GError ** error);

G_END_DECLS

#endif /* INDICATE_INDICATOR_H_INCLUDED__ */

