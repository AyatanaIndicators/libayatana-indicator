
#include "listener.h"
#include "listener-marshal.h"
#include <dbus/dbus-glib-bindings.h>
#include "dbus-indicate-client.h"

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
	IndicateListener * listener;
} proxy_t;

typedef struct {
	DBusGConnection * bus;
	gchar * name;
} proxy_todo_t;

G_DEFINE_TYPE (IndicateListener, indicate_listener, G_TYPE_OBJECT);

/* Prototypes */
static void indicate_listener_finalize (GObject * obj);
static void dbus_owner_change (DBusGProxy * proxy, const gchar * name, const gchar * prev, const gchar * new, IndicateListener * listener);
static void proxy_struct_destroy (gpointer data);
static void build_todo_list_cb (DBusGProxy * proxy, char ** names, GError * error, void * data);
static void todo_list_add (const gchar * name, DBusGProxy * proxy, IndicateListener * listener);
static gboolean todo_idle (gpointer data);
static void proxy_indicator_added (DBusGProxy * proxy, guint id, const gchar * type, proxy_t * proxyt);
static void proxy_get_indicator_list (DBusGProxy * proxy, GArray * indicators, GError * error, gpointer data);

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

	dbus_g_object_register_marshaller(indicate_listener_marshal_VOID__UINT_STRING,
	                                  G_TYPE_NONE,
	                                  G_TYPE_UINT,
	                                  G_TYPE_STRING,
	                                  G_TYPE_INVALID);

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
	listener->todo_idle = 0;

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

IndicateListener *
indicate_listener_new (void)
{
	IndicateListener * listener;
	listener = g_object_new(INDICATE_TYPE_LISTENER, NULL);
	return listener;
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

	if (prev != NULL && prev[0] == '\0') {
		todo_list_add(name, proxy, listener);
	}

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
	DBusGConnection * bus;
	gchar * bus_name;
	if (proxy == listener->dbus_proxy_system) {
		bus = listener->system_bus;
		bus_name = "system";
	} else {
		bus = listener->session_bus;
		bus_name = "session";
	}
	g_debug ("Adding on %s bus: %s", bus_name, name);

	proxy_todo_t todo;
	todo.name = g_strdup(name);
	todo.bus  = bus;

	g_array_append_val(listener->proxy_todo, todo);

	if (listener->todo_idle == 0) {
		listener->todo_idle = g_idle_add(todo_idle, listener);
	}

	return;
}

gboolean
todo_idle (gpointer data)
{
	IndicateListener * listener = INDICATE_LISTENER(data);
	if (listener == NULL) {
		g_error("Listener got lost in todo_idle");
		listener->todo_idle = 0;
		return FALSE;
	}

	if (listener->proxy_todo->len == 0) {
		/* Basically if we have no todo, we need to stop running.  This
		 * is done this way to make the function error handling simpler
		 * and results in an extra run */
		listener->todo_idle = 0;
		return FALSE;
	}

	proxy_todo_t * todo = &g_array_index(listener->proxy_todo, proxy_todo_t, listener->proxy_todo->len - 1);

	proxy_t * proxyt = g_new(proxy_t, 1);
	proxyt->name = todo->name;
	proxyt->proxy = dbus_g_proxy_new_for_name(todo->bus,
	                                          proxyt->name,
	                                          "/org/freedesktop/indicate",
	                                          "org.freedesktop.indicator");
	proxyt->listener = listener;

	listener->proxy_todo = g_array_remove_index(listener->proxy_todo, listener->proxy_todo->len - 1);

	if (proxyt->proxy == NULL) {
		g_warning("Unable to create proxy for %s", proxyt->name);
		return TRUE;
	}

	dbus_g_proxy_add_signal(proxyt->proxy, "IndicatorAdded",
	                        G_TYPE_UINT, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(proxyt->proxy, "IndicatorAdded",
	                            G_CALLBACK(proxy_indicator_added), proxyt, NULL);

	org_freedesktop_indicator_get_indicator_list_async(proxyt->proxy, proxy_get_indicator_list, proxyt);
	return TRUE;
}

static void
proxy_get_indicator_list (DBusGProxy * proxy, GArray * indicators, GError * error, gpointer data)
{
	if (error != NULL) {
		return;
	}

	proxy_t * proxyt = (proxy_t *)data;

	int i;
	for (i = 0; i < indicators->len; i++) {
		g_debug("Interface %s has an indicator %d", proxyt->name, g_array_index(indicators, guint, i));
	}

	return;
}

static void
proxy_indicator_added (DBusGProxy * proxy, guint id, const gchar * type, proxy_t * proxyt)
{
	g_debug("Interface %s has an indicator %d", proxyt->name, id);
	return;
}
