
#include "listener.h"
#include <dbus/dbus-glib-bindings.h>

/* Errors */
enum {
	LAST_ERROR
};

/* Signals */
enum {
	INDICATOR_ADDED,
	INDICATOR_REMOVED,
	INDICATOR_MODIFIED,
	SERVER_ADDED,
	SERVER_REMOVED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

typedef struct {
	DBusGProxy * proxy;
	gchar * name;
} proxy_t;

typedef struct {
	gchar * name;
} proxy_todo_t;

G_DEFINE_TYPE (IndicateListener, indicate_listener, G_TYPE_OBJECT);

/* Prototypes */
static void indicate_listener_finalize (GObject * obj);
static void dbus_owner_change (DBusGProxy * proxy, const gchar * name, const gchar * prev, const gchar * new, IndicateListener * listener);
static void proxy_struct_destroy (gpointer data);
static void build_todo_list_cb (DBusGProxy * proxy, char ** names, GError * error, void * data);
static void todo_list_add (const gchar * name, DBusGProxy * proxy, IndicateListener * listener);

/* Code */
static void
indicate_listener_class_init (IndicateListenerClass * class)
{
	g_debug("Listener Class Initialized");
	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	gobj->finalize = indicate_listener_finalize;

/* TODO, I need new marshallers here, bah humbug
	signals[INDICATOR_ADDED] = g_signal_new("indicator-added",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateListenerClass, indicator_added),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__POINTER_POINTER_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_OBJECT, G_TYPE_OBJECT, G_TYPE_STRING);
	signals[INDICATOR_REMOVED] = g_signal_new("indicator-removed",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_removed),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
	signals[INDICATOR_MODIFIED] = g_signal_new("indicator-modified",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_modified),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
*/

	return;
}

static void
indicate_listener_init (IndicateListener * listener)
{
	g_debug("Listener Object Initialized");
	GError *error = NULL;

	/* Get the buses */
	listener->session_bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (error != NULL) {
		g_error("Unable to get session bus: %s", error->message);
		g_error_free(error);
		return;
	}

	listener->system_bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (error != NULL) {
		g_error("Unable to get system bus: %s", error->message);
		g_error_free(error);
		return;
	}

	/* Set up the DBUS service proxies */
	listener->dbus_proxy_session = dbus_g_proxy_new_for_name_owner (listener->session_bus,
	                                                                DBUS_SERVICE_DBUS,
	                                                                DBUS_PATH_DBUS,
	                                                                DBUS_INTERFACE_DBUS,
	                                                                &error);
	if (error != NULL) {
		g_error("Unable to get dbus proxy on session bus: %s", error->message);
		g_error_free(error);
		return;
	}

	listener->dbus_proxy_system = dbus_g_proxy_new_for_name_owner (listener->system_bus,
	                                                               DBUS_SERVICE_DBUS,
	                                                               DBUS_PATH_DBUS,
	                                                               DBUS_INTERFACE_DBUS,
	                                                               &error);
	if (error != NULL) {
		g_error("Unable to get dbus proxy on system bus: %s", error->message);
		g_error_free(error);
		return;
	}

	/* Set up name change signals */
	dbus_g_proxy_add_signal(listener->dbus_proxy_session, "NameOwnerChanged",
	                        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
	                        G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(listener->dbus_proxy_session, "NameOwnerChanged",
	                            G_CALLBACK(dbus_owner_change), listener, NULL);
	dbus_g_proxy_add_signal(listener->dbus_proxy_system, "NameOwnerChanged",
	                        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
	                        G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(listener->dbus_proxy_system, "NameOwnerChanged",
	                            G_CALLBACK(dbus_owner_change), listener, NULL);

	/* Initialize Data structures */
	listener->proxies_system  = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                                  g_free, proxy_struct_destroy);
	listener->proxies_session = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                                  g_free, proxy_struct_destroy);
	/* TODO: Look at some common scenarios and find out how to make this sized */
	listener->proxy_todo = g_array_new(FALSE, TRUE, sizeof(proxy_todo_t));

	/*            WARNING              */
	/* Starting massive asynchronisity */
	/*                                 */

	/* Build todo list */
	org_freedesktop_DBus_list_names_async (listener->dbus_proxy_session, build_todo_list_cb, listener);
	org_freedesktop_DBus_list_names_async (listener->dbus_proxy_system, build_todo_list_cb, listener);

	return;
}

static void
indicate_listener_finalize (GObject * obj)
{
	IndicateListener * listener = INDICATE_LISTENER(obj);

	return;
}

static void
dbus_owner_change (DBusGProxy * proxy, const gchar * name, const gchar * prev, const gchar * new, IndicateListener * listener)
{
	DBusGConnection * bus;
	gchar * bus_name;
	if (proxy == listener->dbus_proxy_system) {
		bus = listener->system_bus;
		bus_name = "system";
	} else {
		bus = listener->session_bus;
		bus_name = "session";
	}

	g_debug("Name change on %s bus: '%s' from '%s' to '%s'", bus_name, name, prev, new);

	return;
}

static void
proxy_struct_destroy (gpointer data)
{
	proxy_t * proxy_data = data;

	g_object_unref(proxy_data->proxy);
	// name is the key also, so it'll get destroyed there

	return;
}

static void
build_todo_list_cb (DBusGProxy * proxy, char ** names, GError * error, void * data)
{
	IndicateListener * listener = INDICATE_LISTENER(data);

	if (error != NULL) {
		g_warning("Unable to get names: %s", error->message);
		return;
	}

	guint i = 0;
	for (i = 0; names[i] != NULL; i++) {
		todo_list_add(names[i], proxy, listener);
	}

	return;
}

static void
todo_list_add (const gchar * name, DBusGProxy * proxy, IndicateListener * listener)
{
	g_debug ("Adding %s");

	return;
}

