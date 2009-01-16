
#ifndef INDICATE_LISTENER_H_INCLUDED__
#define INDICATE_LISTENER_H_INCLUDED__ 1

#include <glib.h>
#include <glib-object.h>

#include <dbus/dbus-glib.h>

#include "indicator.h"
#include "server.h"

G_BEGIN_DECLS

/* Boilerplate */
#define INDICATE_TYPE_LISTENER (indicate_listener_get_type ())
#define INDICATE_LISTENER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_LISTENER, IndicateListener))
#define INDICATE_IS_LISTENER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_LISTENER))
#define INDICATE_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), INDICATE_TYPE_LISTENER, IndicateListenerClass))
#define INDICATE_IS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), INDICATE_TYPE_LISTENER))
#define INDICATE_LISTENER_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), INDICATE_TYPE_LISTENER, IndicateListenerClass))

typedef gchar IndicateListenerServer;
typedef guint IndicateListenerIndicator;

typedef struct _IndicateListener IndicateListener;
struct _IndicateListener {
	GObject parent;

	DBusGConnection * session_bus;
	DBusGConnection * system_bus;

	DBusGProxy * dbus_proxy_session;
	DBusGProxy * dbus_proxy_system;

	GHashTable * proxies_working;
	GHashTable * proxies_possible;

	GArray * proxy_todo;
	guint todo_idle;
};

typedef struct _IndicateListenerClass IndicateListenerClass;
struct _IndicateListenerClass {
	GObjectClass parent;

	/* Signals */
	void (* indicator_added) (IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type);
	void (* indicator_removed) (IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type);
	void (* indicator_modified) (IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gchar * property);

	void (* server_added) (IndicateListenerServer * server);
	void (* server_removed) (IndicateListenerServer * server);
};

GType indicate_listener_get_type (void) G_GNUC_CONST;

/* Create a new listener */
IndicateListener * indicate_listener_new (void);





G_END_DECLS

#endif /* INDICATE_LISTENER_H_INCLUDED__ */

