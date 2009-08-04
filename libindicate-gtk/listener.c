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

#include "listener.h"
#include <dbus/dbus-glib-bindings.h>

#include "../libindicate/dbus-indicate-client.h"
#include "../libindicate/listener-private.h"

typedef enum _get_property_type get_property_type;
enum _get_property_type {
	PROPERTY_TYPE_STRING,
	PROPERTY_TYPE_TIME,
	PROPERTY_TYPE_ICON
};

typedef struct _get_property_t get_property_t;
struct _get_property_t {
	GCallback cb;
	gpointer data;
	IndicateListener * listener;
	IndicateListenerServer * server;
	IndicateListenerIndicator * indicator;
	gchar * property;
	get_property_type type;
};

static void
get_property_cb (DBusGProxy *proxy, char * OUT_value, GError *error, gpointer userdata)
{
	get_property_t * get_property_data = (get_property_t *)userdata;

	if (error != NULL) {
		g_warning("Unable to get property data: %s", error->message);
		g_error_free(error);
		return;
	}

	switch (get_property_data->type) {
	case PROPERTY_TYPE_STRING: {
		indicate_listener_get_property_cb cb = (indicate_listener_get_property_cb)get_property_data->cb;
		cb(get_property_data->listener, get_property_data->server, get_property_data->indicator, get_property_data->property, OUT_value, get_property_data->data);
		break;
	}
	case PROPERTY_TYPE_ICON: {
		indicate_listener_get_property_icon_cb cb = (indicate_listener_get_property_icon_cb)get_property_data->cb;

		/* There is no icon */
		if (OUT_value == NULL || OUT_value[0] == '\0') {
			break;
		}

		gsize length = 0;
		guchar * icondata = g_base64_decode(OUT_value, &length);
		
		GInputStream * input = g_memory_input_stream_new_from_data(icondata, length, NULL);
		if (input == NULL) {
			g_warning("Cound not create input stream from icon property data");
			g_free(icondata);
			break;
		}

		GError * error = NULL;
		GdkPixbuf * icon = gdk_pixbuf_new_from_stream(input, NULL, &error);
		if (icon != NULL) {
			cb(get_property_data->listener, get_property_data->server, get_property_data->indicator, get_property_data->property, icon, get_property_data->data);
		}

		if (error != NULL) {
			g_warning("Unable to build Pixbuf from icon data: %s", error->message);
			g_error_free(error);
		}

		error = NULL;
		g_input_stream_close(input, NULL, &error);
		if (error != NULL) {
			g_warning("Unable to close input stream: %s", error->message);
			g_error_free(error);
		}
		g_free(icondata);
		break;
	}
	case PROPERTY_TYPE_TIME: {
		indicate_listener_get_property_time_cb cb = (indicate_listener_get_property_time_cb)get_property_data->cb;
		GTimeVal time;
		if (g_time_val_from_iso8601(OUT_value, &time)) {
			cb(get_property_data->listener, get_property_data->server, get_property_data->indicator, get_property_data->property, &time, get_property_data->data);
		}
		break;
	}
	}

	g_free(get_property_data->property);
	g_free(get_property_data);

	return;
};

static void
get_property_helper (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property, GCallback callback, gpointer data, get_property_type prop_type)
{
	/* g_debug("get_property_helper: %s %d", property, prop_type); */
	/* TODO: Do we need to somehow refcount the server/indicator while we're waiting on this? */
	get_property_t * get_property_data = g_new(get_property_t, 1);
	get_property_data->cb = callback;
	get_property_data->data = data;
	get_property_data->listener = listener;
	get_property_data->server = server;
	get_property_data->indicator = indicator;
	get_property_data->property = g_strdup(property);
	get_property_data->type = prop_type;
	
	org_freedesktop_indicator_get_indicator_property_async (server->proxy , INDICATE_LISTENER_INDICATOR_ID(indicator), property, get_property_cb, get_property_data);
	return;
}

void
indicate_listener_get_property_icon (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property, indicate_listener_get_property_icon_cb callback, gpointer data)
{
	return get_property_helper(listener, server, indicator, property, G_CALLBACK(callback), data, PROPERTY_TYPE_ICON);
}

