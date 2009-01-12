
#include "server.h"
#include "dbus-indicate-server.h"

/* Errors */
enum {
	NO_GET_DESKTOP,
	NO_GET_INDICATOR_COUNT,
	NO_GET_INDICATOR_COUNT_BY_TYPE,
	NO_GET_INDICATOR_LIST,
	NO_GET_INDICATOR_LIST_BY_TYPE,
	NO_GET_INDICATOR_PROPERTY,
	NO_GET_INDICATOR_PROPERTY_GROUP,
	NO_GET_INDICATOR_PROPERTIES,
	NO_SHOW_INDICATOR_TO_USER,
	LAST_ERROR
};

/* Signals */
enum {
	INDICATOR_ADDED,
	INDICATOR_REMOVED,
	INDICATOR_MODIFIED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (IndicateServer, indicate_server, G_TYPE_OBJECT);

/* Prototypes */
static void indicate_server_finalize (GObject * obj);

/* Code */
static void
indicate_server_class_init (IndicateServerClass * class)
{
	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	gobj->finalize = indicate_server_finalize;

	signals[INDICATOR_ADDED] = g_signal_new("indicator-added",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_added),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
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

	dbus_g_object_type_install_info(INDICATE_TYPE_SERVER,
	                                &dbus_glib_indicate_server_object_info);

	class->get_desktop = NULL;
	class->get_indicator_count = NULL;
	class->get_indicator_count_by_type = NULL;
	class->get_indicator_list = NULL;
	class->get_indicator_list_by_type = NULL;
	class->get_indicator_property = NULL;
	class->get_indicator_property_group = NULL;
	class->get_indicator_properties = NULL;
	class->show_indicator_to_user = NULL;

	return;
}

static void
indicate_server_init (IndicateServer * server)
{
	server->path = g_strdup("/org/freedesktop/indicate");
	server->indicators = NULL;

	return;
}

static void
indicate_server_finalize (GObject * obj)
{
	IndicateServer * server = INDICATE_SERVER(obj);

	if (server->path) {
		g_free(server->path);
	}

	return;
}

static GQuark
indicate_server_error_quark (void)
{
	static GQuark quark = 0;
	if (quark == 0) {
		quark = g_quark_from_static_string (G_LOG_DOMAIN);
	}
	return quark;
}


void
indicate_server_show (IndicateServer * server)
{
	DBusGConnection * connection;

	connection = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);

	dbus_g_connection_register_g_object(connection,
	                                    server->path,
	                                    G_OBJECT(server));
	
	return;
}

void
indicate_server_add_indicator (IndicateServer * server, IndicateIndicator * indicator)
{
	g_object_ref(indicator);
	server->indicators = g_slist_prepend(server->indicators, indicator);
	return;
}

void
indicate_server_remove_indicator (IndicateServer * server, IndicateIndicator * indicator)
{
	server->indicators = g_slist_remove(server->indicators, indicator);
	g_object_unref(indicator);
	return;
}

static IndicateServer * default_indicate_server = NULL;

IndicateServer *
indicate_server_ref_default (void)
{
	if (default_indicate_server != NULL) {
		g_object_ref(default_indicate_server);
	} else {
		default_indicate_server = g_object_new(INDICATE_TYPE_SERVER, NULL);
		g_object_add_weak_pointer(G_OBJECT(default_indicate_server),
		                          (gpointer *)&default_indicate_server);
	}

	return default_indicate_server;
}

void
indicate_server_set_default (IndicateServer * server)
{
	if (default_indicate_server != NULL) {
		g_warning("Setting a default Indicator Server when one has already been created.  I'm not going to destroy that one, but let it live.  This may create some odd results if you don't know what you're doing.");
	}

	if (server != NULL) {
		default_indicate_server = server;
		g_object_add_weak_pointer(G_OBJECT(default_indicate_server),
		                          (gpointer *)&default_indicate_server);
	}

	return;
}

/* Virtual Functions */
gboolean 
indicate_server_get_desktop (IndicateServer * server, gchar ** desktop_path, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_desktop (server, desktop_path, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_DESKTOP,
		            "get_desktop function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_count (IndicateServer * server, guint * count, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_count (server, count, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_COUNT,
		            "get_indicator_count function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_count_by_type (server, type, count, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_COUNT_BY_TYPE,
		            "get_indicator_count_by_type function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_list (IndicateServer * server, guint ** indicators, GError ** error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_list (server, indicators, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_LIST,
		            "get_indicator_list function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_list_by_type (server, type, indicators, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_LIST_BY_TYPE,
		            "get_indicator_list_by_type function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_property (server, id, property, value, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_PROPERTY,
		            "get_indicator_property function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_property_group (IndicateServer * server, guint id, gchar ** properties, gchar *** value, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_property_group (server, id, properties, value, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_PROPERTY_GROUP,
		            "get_indicator_property_group function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->get_indicator_properties (server, id, properties, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_PROPERTIES,
		            "get_indicator_properties function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

gboolean 
indicate_server_show_indicator_to_user (IndicateServer * server, guint id, GError ** error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL) {
		return class->show_indicator_to_user (server, id, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_SHOW_INDICATOR_TO_USER,
		            "show_indicator_to_user function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
	}

	return TRUE;
}

