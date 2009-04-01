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

#ifndef INDICATE_SERVER_H_INCLUDED__
#define INDICATE_SERVER_H_INCLUDED__ 1

#include <glib.h>
#include <glib-object.h>

#include "indicator.h"
#include "interests.h"

G_BEGIN_DECLS

/* Boilerplate */
#define INDICATE_TYPE_SERVER (indicate_server_get_type ())
#define INDICATE_SERVER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_SERVER, IndicateServer))
#define INDICATE_IS_SERVER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_SERVER))
#define INDICATE_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), INDICATE_TYPE_SERVER, IndicateServerClass))
#define INDICATE_IS_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), INDICATE_TYPE_SERVER))
#define INDICATE_SERVER_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), INDICATE_TYPE_SERVER, IndicateServerClass))

#define INDICATE_SERVER_INDICATOR_NULL  (0)

#define INDICATE_SERVER_SIGNAL_INDICATOR_ADDED      "indicator-added"
#define INDICATE_SERVER_SIGNAL_INDICATOR_REMOVED    "indicator-removed"
#define INDICATE_SERVER_SIGNAL_INDICATOR_MODIFIED   "indicator-modified"
#define INDICATE_SERVER_SIGNAL_SERVER_SHOW          "server-show"
#define INDICATE_SERVER_SIGNAL_SERVER_HIDE          "server-hide"
#define INDICATE_SERVER_SIGNAL_SERVER_DISPLAY       "server-display"
#define INDICATE_SERVER_SIGNAL_LISTENER_INTERESTED  "listener-interested"

typedef struct _IndicateServer IndicateServer;
struct _IndicateServer {
	GObject parent;
};

typedef struct _IndicateServerClass IndicateServerClass;
struct _IndicateServerClass {
	GObjectClass parent;

	/* Signals */
	void (* indicator_added) (IndicateServer * server, guint id, gchar * type);
	void (* indicator_removed) (IndicateServer * server, guint id, gchar * type);
	void (* indicator_modified) (IndicateServer * server, guint id, gchar * property);
	void (* server_show) (IndicateServer * server, gchar * type);
	void (* server_hide) (IndicateServer * server, gchar * type);
	void (* server_display) (IndicateServer * server);
	void (* listener_interested) (IndicateServer * server, IndicateServerInterests * interests);

	/* Virtual Functions */
	gboolean (*get_indicator_count) (IndicateServer * server, guint * count, GError **error);
	gboolean (*get_indicator_count_by_type) (IndicateServer * server, gchar * type, guint * count, GError **error);
	gboolean (*get_indicator_list) (IndicateServer * server, GArray ** indicators, GError ** error);
	gboolean (*get_indicator_list_by_type) (IndicateServer * server, gchar * type, guint ** indicators, GError ** error);
	gboolean (*get_indicator_property) (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
	gboolean (*get_indicator_property_group) (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error);
	gboolean (*get_indicator_properties) (IndicateServer * server, guint id, gchar *** properties, GError **error);
	gboolean (*show_indicator_to_user) (IndicateServer * server, guint id, GError ** error);
	guint    (*get_next_id) (IndicateServer * server);

	/* Reserver for future use */
	void (*indicate_server_reserved1)(void);
	void (*indicate_server_reserved2)(void);
	void (*indicate_server_reserved3)(void);
	void (*indicate_server_reserved4)(void);
};

GType indicate_server_get_type (void) G_GNUC_CONST;

/* Create a new server */
IndicateServer * indicate_server_new (void);

/* Sets the object.  By default this is /org/freedesktop/indicators */
void indicate_server_set_dbus_object (const gchar * obj);

/* Sets the desktop file to get data like name and description
 * out of */
void indicate_server_set_desktop_file (IndicateServer * server, const gchar * path);
void indicate_server_set_type (IndicateServer * server, const gchar * type);

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
gboolean indicate_server_get_indicator_count (IndicateServer * server, guint * count, GError **error);
gboolean indicate_server_get_indicator_count_by_type (IndicateServer * server, gchar * type, guint * count, GError **error);
gboolean indicate_server_get_indicator_list (IndicateServer * server, GArray ** indicators, GError ** error);
gboolean indicate_server_get_indicator_list_by_type (IndicateServer * server, gchar * type, guint ** indicators, GError ** error);
gboolean indicate_server_get_indicator_property (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
gboolean indicate_server_get_indicator_property_group (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error);
gboolean indicate_server_get_indicator_properties (IndicateServer * server, guint id, gchar *** properties, GError **error);
gboolean indicate_server_show_indicator_to_user (IndicateServer * server, guint id, GError ** error);

/* Signal emission functions for sub-classes of the server */
void indicate_server_emit_indicator_added (IndicateServer *server, guint id, const gchar *type);
void indicate_server_emit_indicator_removed (IndicateServer *server, guint id, const gchar *type);
void indicate_server_emit_indicator_modified (IndicateServer *server, guint id, const gchar *property);
void indicate_server_emit_server_display (IndicateServer *server);

G_END_DECLS

#endif /* INDICATE_SERVER_H_INCLUDED__ */

