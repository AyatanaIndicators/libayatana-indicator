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

#include "glib.h"
#include "glib/gmessages.h"
#include "indicator.h"
#include "server.h"

/* Signals */
enum {
	HIDE,
	SHOW,
	USER_DISPLAY,
	MODIFIED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

typedef struct _IndicateIndicatorPrivate IndicateIndicatorPrivate;
struct _IndicateIndicatorPrivate
{
	guint id;
	gboolean is_visible;
	IndicateServer * server;
	GHashTable * properties;
};

#define INDICATE_INDICATOR_GET_PRIVATE(o) \
          (G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATE_TYPE_INDICATOR, IndicateIndicatorPrivate))

G_DEFINE_TYPE (IndicateIndicator, indicate_indicator, G_TYPE_OBJECT);

static void indicate_indicator_finalize (GObject * object);
static void set_property (IndicateIndicator * indicator, const gchar * key, const gchar * data);
static const gchar * get_property (IndicateIndicator * indicator, const gchar * key);
static GPtrArray * list_properties (IndicateIndicator * indicator);


/* Functions */
static void
indicate_indicator_class_init (IndicateIndicatorClass * class)
{
	/* g_debug("Indicator Class Initialized."); */

	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	g_type_class_add_private (class, sizeof (IndicateIndicatorPrivate));

	gobj->finalize = indicate_indicator_finalize;

	/** 
		IndicateIndicator::display:

		Emitted when the user has clicked on this indicator.  In the
		messaging indicator this would be when someone clicks on the
		menu item for the indicator.
	*/
	signals[USER_DISPLAY] = g_signal_new(INDICATE_INDICATOR_SIGNAL_DISPLAY,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, user_display),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__VOID,
	                                     G_TYPE_NONE, 0);
	/**
		IndicateIndicator::hide:

		Emitted every time this indicator is hidden.  This
		is mostly used by #IndicateServer.
	*/
	signals[HIDE] = g_signal_new(INDICATE_INDICATOR_SIGNAL_HIDE,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, hide),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__VOID,
	                                     G_TYPE_NONE, 0);
	/**
		IndicateIndicator::hide:

		Emitted every time this indicator is shown.  This
		is mostly used by #IndicateServer.
	*/
	signals[SHOW] = g_signal_new(INDICATE_INDICATOR_SIGNAL_SHOW,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, show),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__VOID,
	                                     G_TYPE_NONE, 0);
	/**
		IndicateIndicator::modified:

		Emitted every time an indicator property is changed.
		This is mostly used by #IndicateServer.
	*/
	signals[MODIFIED] = g_signal_new(INDICATE_INDICATOR_SIGNAL_MODIFIED,
	                                     G_TYPE_FROM_CLASS(class),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET(IndicateIndicatorClass, modified),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__STRING,
	                                     G_TYPE_NONE, 1, G_TYPE_STRING);

	class->get_type = NULL;
	class->set_property = set_property;
	class->get_property = get_property;
	class->list_properties = list_properties;

	return;
}

static void
indicate_indicator_init (IndicateIndicator * indicator)
{
	/* g_debug("Indicator Object Initialized."); */
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	priv->is_visible = FALSE;

	priv->properties = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

	priv->server = indicate_server_ref_default();
	priv->id = indicate_server_get_next_id(priv->server);

	indicate_server_add_indicator(priv->server, indicator);

	return;
}

static void
indicate_indicator_finalize (GObject * obj)
{
	IndicateIndicator * indicator = INDICATE_INDICATOR(obj);
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	indicate_server_remove_indicator(priv->server, indicator);
	g_object_unref(priv->server);
	priv->server = NULL;

	G_OBJECT_CLASS (indicate_indicator_parent_class)->finalize (obj);
	return;
}

/**
	indicate_indicator_get_type:

	Gets a unique #GType for the #IndicateIndicator objects.

	Return value: A unique #GType value.
*/

/**
	indicate_indicator_new:

	Builds a new indicator object using g_object_new().

	Return value: A pointer to a new #IndicateIndicator object.
*/
IndicateIndicator *
indicate_indicator_new (void)
{
	IndicateIndicator * indicator = g_object_new(INDICATE_TYPE_INDICATOR, NULL);
	return indicator;
}

/**
	indicate_indicator_show:
	@indicator: a #IndicateIndicator to act on

	Shows this indicator on the bus.  If the #IndicateServer that it's
	connected to is not shown itself this function will show the server
	as well using #indicate_server_show.
*/
void
indicate_indicator_show (IndicateIndicator * indicator)
{
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	if (priv->is_visible) {
		return;
	}

	if (priv->server) {
		indicate_server_show(priv->server);
	}

	priv->is_visible = TRUE;
	g_signal_emit(indicator, signals[SHOW], 0, TRUE);

	return;
}

/**
	indicate_indicator_hide:
	@indicator: a #IndicateIndicator to act on

	Hides the indicator from the bus.  Does not effect the
	indicator's #IndicateServer in any way.
*/
void
indicate_indicator_hide (IndicateIndicator * indicator)
{
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	if (!priv->is_visible) {
		return;
	}

	priv->is_visible = FALSE;
	g_signal_emit(indicator, signals[HIDE], 0, TRUE);

	return;
}

/**
	indicate_indicator_is_visible:
	@indicator: a #IndicateIndicator to act on

	Checkes the visibility status of @indicator.

	Return value: %TRUE if the indicator is visible else %FALSE.
*/
gboolean
indicate_indicator_is_visible (IndicateIndicator * indicator)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), FALSE);
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);
	return priv->is_visible;
}

/**
	indicate_indicator_get_id:
	@indicator: a #IndicateIndicator to act on

	Gets the ID value of the @indicator.

	Return value: The ID of the indicator.  Can not be zero.
		Zero represents an error.
*/
guint
indicate_indicator_get_id (IndicateIndicator * indicator)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), 0);
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);
	return priv->id;
}

/**
	indicate_indicator_get_indicator_type:
	@indicator: a #IndicateIndicator to act on

	Returns the type of @indicator.  This is largely set by the subclass
	that the indicator was built with and should not be free'd.

	Return value: A string defining the type or NULL for no type.
*/
const gchar *
indicate_indicator_get_indicator_type (IndicateIndicator * indicator)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), NULL);
	IndicateIndicatorClass * class = INDICATE_INDICATOR_GET_CLASS(indicator);

	if (class->get_type != NULL) {
		return INDICATE_INDICATOR_GET_CLASS(indicator)->get_type(indicator);
	}

	return NULL;
}

/**
	indicate_indicator_user_display:
	@indicator: a #IndicateIndicator to act on

	Emits the #IndicateIndicator::user-display signal simliar to a user
	clicking on @indicator over the bus.  Signal will not be sent if the
	@indicator is not visible.
*/
void
indicate_indicator_user_display (IndicateIndicator * indicator)
{
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);
	if (!priv->is_visible) {
		return;
	}

	g_signal_emit(indicator, signals[USER_DISPLAY], 0, TRUE);
	return;
}

/**
	indicate_indicator_set_property:
	@indicator: a #IndicateIndicator to act on
	@key: name of the property
	@data: value of the property

	Sets a simple string property on @indicator.  If the property
	had previously been set it will replace it with the new value,
	otherwise it will create the property.  This will include an
	emition of #IndicateIndicator::modified if the property value
	was changed.
*/
void
indicate_indicator_set_property (IndicateIndicator * indicator, const gchar * key, const gchar * data)
{
	IndicateIndicatorClass * class = INDICATE_INDICATOR_GET_CLASS(indicator);
	if (class->set_property == NULL) {
		return;
	}

	return class->set_property(indicator, key, data);
}

/**
	indicate_indicator_set_property_icon:
	@indicator: a #IndicateIndicator to act on
	@key: name of the property
	@data: icon to set property with

	This is a helper function that wraps around #indicate_indicator_set_property
	but takes an #GdkPixbuf parameter.  It then takes the @data
	parameter, turns it into a PNG, base64 encodes it and then
	uses that data to call #indicate_indicator_set_property.
*/
void
indicate_indicator_set_property_icon (IndicateIndicator * indicator, const gchar * key, const GdkPixbuf * data)
{
	if (!GDK_IS_PIXBUF(data)) {
		g_warning("Invalide GdkPixbuf");
		return;
	}

	GError * error = NULL;
	gchar * png_data;
	gsize png_data_len;

	if (!gdk_pixbuf_save_to_buffer((GdkPixbuf *)data, &png_data, &png_data_len, "png", &error, NULL)) {
		if (error == NULL) {
			g_warning("Unable to create pixbuf data stream: %d", png_data_len);
		} else {
			g_warning("Unable to create pixbuf data stream: %s", error->message);
			g_error_free(error);
			error = NULL;
		}

		return;
	}

	gchar * prop_str = g_base64_encode((guchar *)png_data, png_data_len);
	indicate_indicator_set_property(indicator, key, prop_str);

	g_free(prop_str);
	g_free(png_data);

	return;
}

/**
	indicate_indicator_set_property_time:
	@indicator: a #IndicateIndicator to act on
	@key: name of the property
	@data: time to set property with

	This is a helper function that wraps around #indicate_indicator_set_property
	but takes an #GTimeVal parameter.  It then takes the @data
	parameter converts it to an ISO 8601 time string and
	uses that data to call #indicate_indicator_set_property.
*/
void
indicate_indicator_set_property_time (IndicateIndicator * indicator, const gchar * key, GTimeVal * time)
{
	gchar * timestr = g_time_val_to_iso8601(time);
	if (timestr != NULL) {
		indicate_indicator_set_property(indicator, key, timestr);
		g_free(timestr);
	}
	return;
}

/**
	indicate_indicator_get_property:
	@indicator: a #IndicateIndicator to act on
	@key: name of the property

	Returns the value that is set for a property or %NULL if that
	property is not set.

	Return value: A constant string or NULL.
*/
const gchar *
indicate_indicator_get_property (IndicateIndicator * indicator, const gchar * key)
{
	IndicateIndicatorClass * class = INDICATE_INDICATOR_GET_CLASS(indicator);
	if (class->get_property == NULL) {
		return NULL;
	}

	return class->get_property(indicator, key);
}

/**
	indicate_indicator_list_properties:
	@indicator: a #IndicateIndicator to act on

	This function gets a list of all the properties that exist
	on a @indicator.  The array may have zero entries but almost
	always at least has 'type' in it.

	Return value: An array of strings that is the keys of all
		the properties on this indicator.
*/
GPtrArray *
indicate_indicator_list_properties (IndicateIndicator * indicator)
{
	IndicateIndicatorClass * class = INDICATE_INDICATOR_GET_CLASS(indicator);
	if (class->list_properties == NULL) {
		return g_ptr_array_new();
	}

	return class->list_properties(indicator);
}

static void
set_property (IndicateIndicator * indicator, const gchar * key, const gchar * data)
{
	g_return_if_fail(INDICATE_IS_INDICATOR(indicator));

	if (key != NULL && !g_strcmp0(key, "type")) {
		g_warning("Trying to set the 'type' of an indicator which should be done through subclassing.");
		return;
	}

	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	gchar * current = g_hash_table_lookup(priv->properties, key);
	if (current == NULL || g_strcmp0(current, data)) {
		/* If the value has changed or there is no value */
		gchar * newkey = g_strdup(key);
		/* g_debug("What is newkey? %s", newkey); */
		g_hash_table_insert(priv->properties, newkey, g_strdup(data));
		if (indicate_indicator_is_visible(indicator)) {
			/* g_debug("Indicator property modified: %s %s", key, data); */
			g_signal_emit(indicator, signals[MODIFIED], 0, key, TRUE);
		}
	}

	return;
}

static const gchar *
get_property (IndicateIndicator * indicator, const gchar * key)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), NULL);

	if (key != NULL && !g_strcmp0(key, "type")) {
		return indicate_indicator_get_indicator_type(indicator);
	}

	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	// TODO: Think about whether we should be strdup'ing this.  Seems like overkill, but might not be.
	return (const gchar *)g_hash_table_lookup(priv->properties, key);
}

static GPtrArray *
list_properties (IndicateIndicator * indicator)
{
	g_return_val_if_fail(INDICATE_IS_INDICATOR(indicator), g_ptr_array_new());
	IndicateIndicatorPrivate * priv = INDICATE_INDICATOR_GET_PRIVATE(indicator);

	GList * keys = g_hash_table_get_keys(priv->properties);
	GPtrArray * properties = g_ptr_array_sized_new(g_list_length(keys) + 1);

	g_ptr_array_add(properties, g_strdup("type"));
	for (; keys != NULL; keys = keys->next) {
		g_ptr_array_add(properties, g_strdup(keys->data));
	}

	return properties;
}
