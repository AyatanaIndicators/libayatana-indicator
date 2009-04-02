/*
A library to allow applictions to provide simple indications of
information to be displayed to users of the application through the
interface shell.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of either or both of the following licenses:

1) the GNU Lesser General Public License version 3, as published by the 
Free Software Foundation; and/or
2) the GNU Lesser General Public License version 2.1, as published by 
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the applicable version of the GNU Lesser General Public 
License for more details.

You should have received a copy of both the GNU Lesser General Public 
License version 3 and version 2.1 along with this program.  If not, see 
<http://www.gnu.org/licenses/>
*/
 
#include "server.h"
#include "interests-priv.h"
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

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
	INVALID_INDICATOR_ID,
	NO_SHOW_INTEREST,
	NO_REMOVE_INTEREST,
	SHOW_INTEREST_FAILED,
	REMOVE_INTEREST_FAILED,
	LAST_ERROR
};

/* Signals */
enum {
	INDICATOR_ADDED,
	INDICATOR_REMOVED,
	INDICATOR_MODIFIED,
	SERVER_SHOW,
	SERVER_HIDE,
	SERVER_DISPLAY,
	INTEREST_ADDED,
	INTEREST_REMOVED,
	LAST_SIGNAL
};

/* Properties */
enum {
	PROP_0,
	PROP_DESKTOP,
	PROP_TYPE
};

static guint signals[LAST_SIGNAL] = { 0 };

/* Private area */
typedef struct _IndicateServerPrivate IndicateServerPrivate;
struct _IndicateServerPrivate
{
	DBusGConnection *connection;
	DBusGProxy * dbus_proxy;

	gchar * path;
	GSList * indicators;
	gboolean visible;
	guint current_id;

	gchar * desktop;
	gchar * type;

	// TODO: Should have a more robust way to track this, but this'll work for now
	guint num_hidden;

	gboolean interests[INDICATE_INTEREST_LAST];
	GList * interestedfolks;
};

#define INDICATE_SERVER_GET_PRIVATE(o) \
          (G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATE_TYPE_SERVER, IndicateServerPrivate))

typedef struct _IndicateServerInterestedFolk IndicateServerInterestedFolk;
struct _IndicateServerInterestedFolk {
	gchar * sender;
	gboolean interests[INDICATE_INTEREST_LAST];
};


/* Define Type */
G_DEFINE_TYPE (IndicateServer, indicate_server, G_TYPE_OBJECT);

/* Prototypes */
static void indicate_server_finalize (GObject * obj);
static gboolean get_indicator_count (IndicateServer * server, guint * count, GError **error);
static gboolean get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error);
static gboolean get_indicator_list (IndicateServer * server, GArray ** indicators, GError ** error);
static gboolean get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error);
static gboolean get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
static gboolean get_indicator_property_group (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error);
static gboolean get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error);
static gboolean show_indicator_to_user (IndicateServer * server, guint id, GError ** error);
static void dbus_owner_change (DBusGProxy * proxy, const gchar * name, const gchar * prev, const gchar * new, IndicateServer * server);
static guint get_next_id (IndicateServer * server);
static void set_property (GObject * obj, guint id, const GValue * value, GParamSpec * pspec);
static void get_property (GObject * obj, guint id, GValue * value, GParamSpec * pspec);
static gboolean show_interest (IndicateServer * server, gchar * sender, IndicateInterests interest);
static gboolean remove_interest (IndicateServer * server, gchar * sender, IndicateInterests interest);
static gboolean check_interest (IndicateServer * server, IndicateInterests intrest);
static gint indicate_server_interested_folks_equal (gconstpointer a, gconstpointer b);
static void indicate_server_interested_folks_init (IndicateServerInterestedFolk * folk);
static void indicate_server_interested_folks_set (IndicateServerInterestedFolk * folk, IndicateInterests interest, gboolean value);
static void indicate_server_interested_folks_copy (IndicateServerInterestedFolk * folk, gboolean * interests);

/* DBus API */
gboolean indicate_server_get_indicator_count (IndicateServer * server, guint * count, GError **error);
gboolean indicate_server_get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error);
gboolean indicate_server_get_indicator_list (IndicateServer * server, GArray ** indicators, GError ** error);
gboolean indicate_server_get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error);
gboolean indicate_server_get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
gboolean indicate_server_get_indicator_property_group (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error);
gboolean indicate_server_get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error);
gboolean indicate_server_show_indicator_to_user (IndicateServer * server, guint id, GError ** error);
gboolean indicate_server_show_interest (IndicateServer * server, gchar * interest, DBusGMethodInvocation * method);
gboolean indicate_server_remove_interest (IndicateServer * server, gchar * interest, DBusGMethodInvocation * method);

/* Has to be after the dbus prototypes */
#include "dbus-indicate-server.h"



/* Code */
static void
indicate_server_class_init (IndicateServerClass * class)
{
	/* g_debug("Server Class Initialized"); */
	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	g_type_class_add_private (class, sizeof (IndicateServerPrivate));

	gobj->finalize = indicate_server_finalize;
	gobj->set_property = set_property;
	gobj->get_property = get_property;

	signals[INDICATOR_ADDED] = g_signal_new(INDICATE_SERVER_SIGNAL_INDICATOR_ADDED,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_added),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
	signals[INDICATOR_REMOVED] = g_signal_new(INDICATE_SERVER_SIGNAL_INDICATOR_REMOVED,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_removed),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
	signals[INDICATOR_MODIFIED] = g_signal_new(INDICATE_SERVER_SIGNAL_INDICATOR_MODIFIED,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_modified),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
	signals[SERVER_SHOW] = g_signal_new(INDICATE_SERVER_SIGNAL_SERVER_SHOW,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, server_show),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__POINTER,
	                                        G_TYPE_NONE, 1, G_TYPE_STRING);
	signals[SERVER_HIDE] = g_signal_new(INDICATE_SERVER_SIGNAL_SERVER_HIDE,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, server_hide),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__POINTER,
	                                        G_TYPE_NONE, 1, G_TYPE_STRING);
	signals[SERVER_DISPLAY] = g_signal_new(INDICATE_SERVER_SIGNAL_SERVER_DISPLAY,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, server_display),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__VOID,
	                                        G_TYPE_NONE, 0);
	signals[INTEREST_ADDED] = g_signal_new(INDICATE_SERVER_SIGNAL_INTEREST_ADDED,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, interest_added),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT,
	                                        G_TYPE_NONE, 1, G_TYPE_UINT);
	signals[INTEREST_REMOVED] = g_signal_new(INDICATE_SERVER_SIGNAL_INTEREST_REMOVED,
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, interest_removed),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT,
	                                        G_TYPE_NONE, 1, G_TYPE_UINT);

	g_object_class_install_property (gobj, PROP_DESKTOP,
	                                 g_param_spec_string("desktop", "Desktop File",
	                                              "The desktop file representing this server",
	                                              "",
	                                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobj, PROP_TYPE,
	                                 g_param_spec_string("type", "Server Type",
	                                              "The type of indicators that this server will provide",
	                                              "",
	                                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	dbus_g_object_type_install_info(INDICATE_TYPE_SERVER,
	                                &dbus_glib_indicate_server_object_info);

	class->get_indicator_count = get_indicator_count;
	class->get_indicator_count_by_type = get_indicator_count_by_type;
	class->get_indicator_list = get_indicator_list;
	class->get_indicator_list_by_type = get_indicator_list_by_type;
	class->get_indicator_property = get_indicator_property;
	class->get_indicator_property_group = get_indicator_property_group;
	class->get_indicator_properties = get_indicator_properties;
	class->show_indicator_to_user = show_indicator_to_user;
	class->get_next_id = get_next_id;
	class->show_interest = show_interest;
	class->remove_interest = remove_interest;
	class->check_interest = check_interest;

	return;
}

static void
indicate_server_init (IndicateServer * server)
{
	/* g_debug("Server Object Initialized"); */

	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	priv->path = g_strdup("/org/freedesktop/indicate");
	priv->indicators = NULL;
	priv->num_hidden = 0;
	priv->visible = FALSE;
	priv->current_id = 0;
	priv->type = NULL;
	priv->desktop = NULL;

	guint i;
	for (i = INDICATE_INTEREST_NONE; i < INDICATE_INTEREST_LAST; i++) {
		priv->interests[i] = FALSE;
	}
	priv->interestedfolks = NULL;


	return;
}

static void
indicate_server_finalize (GObject * obj)
{
	IndicateServer * server = INDICATE_SERVER(obj);
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	/* TODO: This probably shouldn't be as far down as finalize, but it's fine here. */
	if (priv->visible) {
		g_signal_emit(server, signals[SERVER_HIDE], 0, priv->type ? priv->type : "", TRUE);
	}

	if (priv->path) {
		g_free(priv->path);
	}
	if (priv->desktop) {
		g_free(priv->desktop);
	}
	if (priv->type) {
		g_free(priv->type);
	}

	return;
}

static void
set_property (GObject * obj, guint id, const GValue * value, GParamSpec * pspec)
{
	g_return_if_fail(G_VALUE_HOLDS_STRING(value));
	g_return_if_fail(id == PROP_DESKTOP || id == PROP_TYPE);

	gchar ** outstr;
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(obj);
	switch (id) {
	case PROP_DESKTOP:
		outstr = &(priv->desktop);
		break;
	case PROP_TYPE:
		outstr = &(priv->type);
		break;
	}

	if (*outstr != NULL) {
		g_free(*outstr);
	}

	*outstr = g_strdup(g_value_get_string(value));

	return;
}

static void
get_property (GObject * obj, guint id, GValue * value, GParamSpec * pspec)
{
	g_return_if_fail(id == PROP_DESKTOP || id == PROP_TYPE);

	gchar * outstr;
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(obj);
	switch (id) {
	case PROP_DESKTOP:
		outstr = priv->desktop;
		break;
	case PROP_TYPE:
		outstr = priv->type;
		break;
	}

	if (outstr != NULL) {
		g_value_set_string(value, outstr);
	} else {
		g_value_set_static_string(value, "");
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
	g_return_if_fail(INDICATE_IS_SERVER(server));
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	if (priv->visible)
		return;

	priv->connection = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);

	dbus_g_connection_register_g_object(priv->connection,
	                                    priv->path,
	                                    G_OBJECT(server));
	priv->visible = TRUE;

	g_signal_emit(server, signals[SERVER_SHOW], 0, priv->type ? priv->type : "", TRUE);

	priv->dbus_proxy = dbus_g_proxy_new_for_name_owner (priv->connection,
	                                                    DBUS_SERVICE_DBUS,
	                                                    DBUS_PATH_DBUS,
	                                                    DBUS_INTERFACE_DBUS,
	                                                    NULL);
	dbus_g_proxy_add_signal(priv->dbus_proxy, "NameOwnerChanged",
	                        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
	                        G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(priv->dbus_proxy, "NameOwnerChanged",
	                            G_CALLBACK(dbus_owner_change), server, NULL);
	
	return;
}

void
indicate_server_hide (IndicateServer * server)
{
	g_return_if_fail(INDICATE_IS_SERVER(server));
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	if (!priv->visible)
		return;

	priv->visible = FALSE;

	/* Delete interested parties */
	g_list_foreach(priv->interestedfolks, (GFunc)g_free, NULL);
	g_list_free(priv->interestedfolks);
	priv->interestedfolks = NULL;

	/* Signal the lack of interest */
	guint i;
	for (i = INDICATE_INTEREST_NONE; i < INDICATE_INTEREST_LAST; i++) {
		if (priv->interests[i]) {
			g_signal_emit(G_OBJECT(server), signals[INTEREST_REMOVED], 0, i, TRUE);
		}
		priv->interests[i] = FALSE;
	}

	g_signal_emit(server, signals[SERVER_HIDE], 0, priv->type ? priv->type : "", TRUE);

	g_object_unref(G_OBJECT(priv->dbus_proxy));
	dbus_g_connection_unref (priv->connection);
	priv->connection = NULL;
	
	return;
}

static void
dbus_owner_change (DBusGProxy * proxy, const gchar * name, const gchar * prev, const gchar * new, IndicateServer * server)
{


}

static guint
get_next_id (IndicateServer * server)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);
	priv->current_id++;
	return priv->current_id;
}

static gboolean
show_interest (IndicateServer * server, gchar * sender, IndicateInterests interest)
{
	IndicateServerInterestedFolk localfolk;
	localfolk.sender = sender;

	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	GList * entry = g_list_find_custom(priv->interestedfolks, &localfolk, indicate_server_interested_folks_equal);
	IndicateServerInterestedFolk * folkpointer = NULL;
	if (entry == NULL) {
		folkpointer = g_new(IndicateServerInterestedFolk, 1);
		indicate_server_interested_folks_init(folkpointer);
		priv->interestedfolks = g_list_append(priv->interestedfolks, folkpointer);
	} else {
		folkpointer = (IndicateServerInterestedFolk *)entry->data;
	}

	indicate_server_interested_folks_set(folkpointer, interest, TRUE);
	if (!priv->interests[interest]) {
		g_signal_emit(G_OBJECT(server), signals[INTEREST_ADDED], 0, interest, TRUE);
		priv->interests[interest] = TRUE;
	}

	return TRUE;
}

static gboolean
remove_interest (IndicateServer * server, gchar * sender, IndicateInterests interest)
{
	IndicateServerInterestedFolk localfolk;
	localfolk.sender = sender;

	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	GList * entry = g_list_find_custom(priv->interestedfolks, &localfolk, indicate_server_interested_folks_equal);
	IndicateServerInterestedFolk * folkpointer = NULL;
	if (entry == NULL) {
		folkpointer = g_new(IndicateServerInterestedFolk, 1);
		indicate_server_interested_folks_init(folkpointer);
		priv->interestedfolks = g_list_append(priv->interestedfolks, folkpointer);
	} else {
		folkpointer = (IndicateServerInterestedFolk *)entry->data;
	}

	indicate_server_interested_folks_set(folkpointer, interest, FALSE);

	if (priv->interests[interest]) {
		guint i;
		for (i = INDICATE_INTEREST_NONE; i < INDICATE_INTEREST_LAST; i++) {
			priv->interests[i] = FALSE;
		}

		GList * listi = NULL;
		for (listi = priv->interestedfolks ; listi != NULL ; listi = listi->next) {
			folkpointer = (IndicateServerInterestedFolk *)listi->data;
			indicate_server_interested_folks_copy(folkpointer, priv->interests);
		}

		if (!priv->interests[interest]) {
			g_signal_emit(G_OBJECT(server), signals[INTEREST_REMOVED], 0, interest, TRUE);
		}
	}

	return TRUE;
}

static gboolean
check_interest (IndicateServer * server, IndicateInterests interest)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);
	return priv->interests[interest];
}

static void
indicator_show_cb (IndicateIndicator * indicator, IndicateServer * server)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);
	priv->num_hidden--;
	g_signal_emit(server, signals[INDICATOR_ADDED], 0, indicate_indicator_get_id(indicator), indicate_indicator_get_indicator_type(indicator), TRUE);
	return;
}

static void
indicator_hide_cb (IndicateIndicator * indicator, IndicateServer * server)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);
	priv->num_hidden++;
	g_signal_emit(server, signals[INDICATOR_REMOVED], 0, indicate_indicator_get_id(indicator), indicate_indicator_get_indicator_type(indicator), TRUE);
	return;
}

static void
indicator_modified_cb (IndicateIndicator * indicator, gchar * property, IndicateServer * server)
{
	/* g_debug("Indicator Modified: %d %s", indicate_indicator_get_id(indicator), property); */
	g_signal_emit(server, signals[INDICATOR_MODIFIED], 0, indicate_indicator_get_id(indicator), property, TRUE);
}

void
indicate_server_add_indicator (IndicateServer * server, IndicateIndicator * indicator)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	g_object_ref(indicator);
	priv->indicators = g_slist_prepend(priv->indicators, indicator);

	if (!indicate_indicator_is_visible(indicator)) {
		priv->num_hidden++;
	} else {
		g_signal_emit(server, signals[INDICATOR_ADDED], 0, indicate_indicator_get_id(indicator), indicate_indicator_get_indicator_type(indicator), TRUE);
	}

	g_signal_connect(indicator, INDICATE_INDICATOR_SIGNAL_SHOW, G_CALLBACK(indicator_show_cb), server);
	g_signal_connect(indicator, INDICATE_INDICATOR_SIGNAL_HIDE, G_CALLBACK(indicator_hide_cb), server);
	g_signal_connect(indicator, INDICATE_INDICATOR_SIGNAL_MODIFIED, G_CALLBACK(indicator_modified_cb), server);

	return;
}

void
indicate_server_remove_indicator (IndicateServer * server, IndicateIndicator * indicator)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	priv->indicators = g_slist_remove(priv->indicators, indicator);
	if (indicate_indicator_is_visible(indicator)) {
		g_signal_emit(server, signals[INDICATOR_REMOVED], 0, indicate_indicator_get_id(indicator), indicate_indicator_get_indicator_type(indicator), TRUE);
	} else {
		priv->num_hidden--;
	}

	g_signal_handlers_disconnect_by_func(indicator, indicator_show_cb, server);
	g_signal_handlers_disconnect_by_func(indicator, indicator_hide_cb, server);
	g_signal_handlers_disconnect_by_func(indicator, indicator_modified_cb, server);

	g_object_unref(indicator);
	return;
}

void
indicate_server_set_dbus_object (const gchar * obj)
{
	/* TODO */

	return;
}

void
indicate_server_set_desktop_file (IndicateServer * server, const gchar * path)
{
	GValue value = {0};
	g_value_init(&value, G_TYPE_STRING);
	g_value_set_string(&value, path);
	g_object_set_property(G_OBJECT(server), "desktop", &value);
	return;
}

void
indicate_server_set_type (IndicateServer * server, const gchar * type)
{
	GValue value = {0};
	g_value_init(&value, G_TYPE_STRING);
	g_value_set_string(&value, type);
	g_object_set_property(G_OBJECT(server), "type", &value);
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

static gboolean
get_indicator_count (IndicateServer * server, guint * count, GError **error)
{
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	guint lstcnt = g_slist_length(priv->indicators);

	g_return_val_if_fail(priv->num_hidden < lstcnt, TRUE);
	
	*count = lstcnt - priv->num_hidden;

	return TRUE;
}

typedef struct {
	gchar * type;
	guint count;
} count_by_t;

static void
count_by_type (IndicateIndicator * indicator, count_by_t * cbt)
{
	g_return_if_fail(INDICATE_IS_INDICATOR(indicator));
	if (!indicate_indicator_is_visible(indicator)) {
		return;
	}

	const gchar * type = indicate_indicator_get_indicator_type(indicator);
	/* g_debug("Looking for indicator of type '%s' and have type '%s'", cbt->type, type); */

	if (type == NULL && cbt->type == NULL) {
		cbt->count++;
	} else if (type == NULL || cbt->type == NULL) {
	} else if (!strcmp(type, cbt->type)) {
		cbt->count++;
	}

	return;
}

static gboolean
get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error)
{
	/* g_debug("get_indicator_count_by_type: '%s'", type); */
	count_by_t cbt;
	cbt.type = type;
	cbt.count = 0;

	/* Handle the NULL string case as NULL itself, we're a big
	   boy language; we have pointers. */
	if (cbt.type != NULL && cbt.type[0] == '\0') {
		cbt.type = NULL;
	}

	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	g_slist_foreach(priv->indicators, (GFunc)count_by_type, &cbt);
	*count = cbt.count;

	return TRUE;
}

static gboolean
get_indicator_list (IndicateServer * server, GArray ** indicators, GError ** error)
{
	g_return_val_if_fail(INDICATE_IS_SERVER(server), TRUE);

	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);
	g_return_val_if_fail(class->get_indicator_count != NULL, TRUE);

	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	*indicators = g_array_sized_new(FALSE, FALSE, sizeof(guint), g_slist_length(priv->indicators) - priv->num_hidden);

	GSList * iter;
	int i;
	for (iter = priv->indicators, i = 0; iter != NULL; iter = iter->next, i++) {
		IndicateIndicator * indicator = INDICATE_INDICATOR(iter->data);
		if (indicate_indicator_is_visible(indicator)) {
			guint id = indicate_indicator_get_id(indicator);
			g_array_insert_val(*indicators, i, id);
		}
	}

	return TRUE;
}

static gboolean
get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error)
{
	g_return_val_if_fail(INDICATE_IS_SERVER(server), TRUE);

	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);
	g_return_val_if_fail(class->get_indicator_count != NULL, TRUE);

	if (type != NULL && type[0] == '\0') {
		type = NULL;
	}

	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	/* Can't be larger than this and it's not worth the reallocation
	   for the small number we have.  The memory isn't worth the time. */
	*indicators = g_array_sized_new(FALSE, FALSE, sizeof(guint), g_slist_length(priv->indicators) - priv->num_hidden);

	GSList * iter;
	int i;
	for (iter = priv->indicators, i = 0; iter != NULL; iter = iter->next) {
		IndicateIndicator * indicator = INDICATE_INDICATOR(iter->data);
		if (indicate_indicator_is_visible(indicator)) {
			const gchar * itype = indicate_indicator_get_indicator_type(indicator);
			guint id = indicate_indicator_get_id(indicator);

			if (type == NULL && itype == NULL) {
				g_array_insert_val(*indicators, i++, id);
			} else if (type == NULL || itype == NULL) {
			} else if (!strcmp(type, itype)) {
				g_array_insert_val(*indicators, i++, id);
			}
		}
	}

	return TRUE;
}

static IndicateIndicator *
get_indicator (IndicateServer * server, guint id, GError **error)
{
	g_return_val_if_fail(INDICATE_IS_SERVER(server), NULL);
	IndicateServerPrivate * priv = INDICATE_SERVER_GET_PRIVATE(server);

	GSList * iter;
	for (iter = priv->indicators; iter != NULL; iter = iter->next) {
		IndicateIndicator * indicator = INDICATE_INDICATOR(iter->data);
		if (indicate_indicator_get_id(indicator) == id) {
			return indicator;
		}
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            INVALID_INDICATOR_ID,
		            "Invalid Indicator ID: %d",
		            id);
	}
	return NULL;
}

static gboolean
get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error)
{
	IndicateIndicator * indicator = get_indicator(server, id, error);
	if (indicator == NULL) {
		return FALSE;
	}

	*value = g_strdup(indicate_indicator_get_property(indicator, property));
	return TRUE;
}

static gboolean
get_indicator_property_group (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error)
{
	IndicateIndicator * indicator = get_indicator(server, id, error);
	if (indicator == NULL) {
		return FALSE;
	}

	GPtrArray * array = g_ptr_array_new();
	int i;
	for (i = 0; i < properties->len; i++) {
		const gchar * val = indicate_indicator_get_property(indicator, g_ptr_array_index(properties, i));
		if (val != NULL) {
			g_ptr_array_add(array, g_strdup(val));
		} else {
			g_ptr_array_add(array, g_strdup(""));
		}
	}
	g_ptr_array_add(array, NULL);
	*value = (gchar **)g_ptr_array_free(array, FALSE);

	return TRUE;
}

static gboolean
get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error)
{
	IndicateIndicator * indicator = get_indicator(server, id, error);
	if (indicator == NULL) {
		return FALSE;
	}

	GPtrArray * array = indicate_indicator_list_properties(indicator);
	g_ptr_array_add(array, NULL);

	*properties = (gchar **)g_ptr_array_free(array, FALSE);

	return TRUE;
}

static gboolean
show_indicator_to_user (IndicateServer * server, guint id, GError ** error)
{
	if (id == INDICATE_SERVER_INDICATOR_NULL) {
		g_signal_emit(server, signals[SERVER_DISPLAY], 0, TRUE);
		return TRUE;
	}

	IndicateIndicator * indicator = get_indicator(server, id, error);
	if (indicator == NULL) {
		return FALSE;
	}

	indicate_indicator_user_display(indicator);
	return TRUE;
}


/* Virtual Functions */
gboolean 
indicate_server_get_indicator_count (IndicateServer * server, guint * count, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_count != NULL) {
		return class->get_indicator_count (server, count, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_COUNT,
		            "get_indicator_count function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_count_by_type != NULL) {
		return class->get_indicator_count_by_type (server, type, count, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_COUNT_BY_TYPE,
		            "get_indicator_count_by_type function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_list (IndicateServer * server, GArray ** indicators, GError ** error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_list != NULL) {
		return class->get_indicator_list (server, indicators, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_LIST,
		            "get_indicator_list function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_list_by_type != NULL) {
		return class->get_indicator_list_by_type (server, type, indicators, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_LIST_BY_TYPE,
		            "get_indicator_list_by_type function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_property != NULL) {
		return class->get_indicator_property (server, id, property, value, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_PROPERTY,
		            "get_indicator_property function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_property_group (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_property_group != NULL) {
		return class->get_indicator_property_group (server, id, properties, value, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_PROPERTY_GROUP,
		            "get_indicator_property_group function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_indicator_properties != NULL) {
		return class->get_indicator_properties (server, id, properties, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_GET_INDICATOR_PROPERTIES,
		            "get_indicator_properties function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

gboolean 
indicate_server_show_indicator_to_user (IndicateServer * server, guint id, GError ** error)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->show_indicator_to_user != NULL) {
		return class->show_indicator_to_user (server, id, error);
	}

	if (error) {
		g_set_error(error,
		            indicate_server_error_quark(),
		            NO_SHOW_INDICATOR_TO_USER,
		            "show_indicator_to_user function doesn't exist for this server class: %s",
		            G_OBJECT_TYPE_NAME(server));
		return FALSE;
	}

	return TRUE;
}

guint 
indicate_server_get_next_id (IndicateServer * server)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->get_next_id != NULL) {
		return class->get_next_id (server);
	}

	return 0;
}

static IndicateInterests
interest_string_to_enum (gchar * interest_string)
{
	if (!g_strcmp0(interest_string, INDICATE_INTEREST_STRING_SERVER_DISPLAY)) {
		return INDICATE_INTEREST_SERVER_DISPLAY;
	}

	if (!g_strcmp0(interest_string, INDICATE_INTEREST_STRING_SERVER_SIGNAL)) {
		return INDICATE_INTEREST_SERVER_SIGNAL;
	}

	if (!g_strcmp0(interest_string, INDICATE_INTEREST_STRING_INDICATOR_DISPLAY)) {
		return INDICATE_INTEREST_INDICATOR_DISPLAY;
	}

	if (!g_strcmp0(interest_string, INDICATE_INTEREST_STRING_INDICATOR_SIGNAL)) {
		return INDICATE_INTEREST_INDICATOR_SIGNAL;
	}

	if (!g_strcmp0(interest_string, INDICATE_INTEREST_STRING_INDICATOR_COUNT)) {
		return INDICATE_INTEREST_INDICATOR_COUNT;
	}

	return INDICATE_INTEREST_NONE;
}

gboolean
indicate_server_show_interest (IndicateServer * server, gchar * interest, DBusGMethodInvocation * method)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->show_interest != NULL) {
		if (class->show_interest (server, dbus_g_method_get_sender(method), interest_string_to_enum(interest))){
			dbus_g_method_return(method);
			return TRUE;
		} else {
			GError * error;
			g_set_error(&error,
						indicate_server_error_quark(),
						SHOW_INTEREST_FAILED,
						"Unable to show interest: %s",
						interest);
			dbus_g_method_return_error(method, error);
			g_error_free(error);
			return FALSE;
		}
	}

	GError * error;
	g_set_error(&error,
				indicate_server_error_quark(),
				NO_SHOW_INTEREST,
				"show_interest function doesn't exist for this server class: %s",
				G_OBJECT_TYPE_NAME(server));
	dbus_g_method_return_error(method, error);
	g_error_free(error);
	return FALSE;
}

gboolean
indicate_server_remove_interest (IndicateServer * server, gchar * interest, DBusGMethodInvocation * method)
{
	IndicateServerClass * class = INDICATE_SERVER_GET_CLASS(server);

	if (class != NULL && class->remove_interest != NULL) {
		if (class->remove_interest (server, dbus_g_method_get_sender(method), interest_string_to_enum(interest))){
			dbus_g_method_return(method);
			return TRUE;
		} else {
			GError * error;
			g_set_error(&error,
						indicate_server_error_quark(),
						REMOVE_INTEREST_FAILED,
						"Unable to remove interest: %s",
						interest);
			dbus_g_method_return_error(method, error);
			g_error_free(error);
			return FALSE;
		}
	}

	GError * error;
	g_set_error(&error,
				indicate_server_error_quark(),
				NO_REMOVE_INTEREST,
				"remove_interest function doesn't exist for this server class: %s",
				G_OBJECT_TYPE_NAME(server));
	dbus_g_method_return_error(method, error);
	g_error_free(error);
	return FALSE;
}

/* Signal emission functions for sub-classes of the server */
void 
indicate_server_emit_indicator_added (IndicateServer *server, guint id, const gchar *type)
{
  g_return_if_fail (INDICATE_IS_SERVER (server));
  g_return_if_fail (type);

  g_signal_emit(server, signals[INDICATOR_ADDED], 0, id, type);
}

void 
indicate_server_emit_indicator_removed (IndicateServer *server, guint id, const gchar *type)
{
  g_return_if_fail (INDICATE_IS_SERVER (server));
  g_return_if_fail (type);

  g_signal_emit(server, signals[INDICATOR_REMOVED], 0, id, type);
}

void 
indicate_server_emit_indicator_modified (IndicateServer *server, guint id, const gchar *property)
{
  g_return_if_fail (INDICATE_IS_SERVER (server));
  g_return_if_fail (property);
  
  g_signal_emit(server, signals[INDICATOR_MODIFIED], 0, id, property);
}

void 
indicate_server_emit_server_display (IndicateServer *server)
{
  g_return_if_fail (INDICATE_IS_SERVER (server));
  
  g_signal_emit(server, signals[SERVER_DISPLAY], 0, TRUE);
}

/* *** Folks stuff *** */

static gint
indicate_server_interested_folks_equal (gconstpointer a, gconstpointer b)
{
	return g_strcmp0(((IndicateServerInterestedFolk *)a)->sender,((IndicateServerInterestedFolk *)b)->sender);
}

static void
indicate_server_interested_folks_init (IndicateServerInterestedFolk * folk)
{
	folk->sender = NULL;

	guint i;
	for (i = INDICATE_INTEREST_NONE; i < INDICATE_INTEREST_LAST; i++) {
		folk->interests[i] = FALSE;
	}

	return;
}

static void
indicate_server_interested_folks_set (IndicateServerInterestedFolk * folk, IndicateInterests interest, gboolean value)
{
	folk->interests[interest] = value;
	return;
}

static void
indicate_server_interested_folks_copy (IndicateServerInterestedFolk * folk, gboolean * interests)
{
	guint i;
	for (i = INDICATE_INTEREST_NONE; i < INDICATE_INTEREST_LAST; i++) {
		if (folk->interests[i]) {
			interests[i] = TRUE;
		}
	}

	return;
}
/* *** End Folks *** */
