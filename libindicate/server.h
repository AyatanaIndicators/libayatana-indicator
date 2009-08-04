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
#define INDICATE_SERVER_SIGNAL_INTEREST_ADDED       "interest-added"
#define INDICATE_SERVER_SIGNAL_INTEREST_REMOVED     "interest-removed"

/**
	IndicateServer:

	This is the object that represents the overall connection
	between this application and DBus.  It acts as the proxy for
	incomming DBus calls and also sends the appropriate signals
	on DBus for events happening on other objects locally.  It
	provides some settings that effection how the application as
	a whole is perceived by listeners of the indicator protocol.
*/
typedef struct _IndicateServer IndicateServer;
struct _IndicateServer {
	GObject parent;
};

#include "indicator.h"

/**
	IndicateServerClass:
	@parent: Parent Class
	@indicator_added: Slot for #IndicateServer::indicator-added.
	@indicator_removed: Slot for #IndicateServer::indicator-removed.
	@indicator_modified: Slot for #IndicateServer::indicator-modified.
	@server_show: Slot for #IndicateServer::server-show.
	@server_hide: Slot for #IndicateServer::server-hide.
	@server_display: Slot for #IndicateServer::server-display.
	@interest_added: Slot for #IndicateServer::interest-added.
	@interest_removed: Slot for #IndicateServer::interest-removed.
	@get_indicator_count: Returns the number of indicators that are visible
		on the bus.  Hidden indicators should not be counted.
	@get_indicator_count_by_type: Returns the number of indicators that are
		of a given type and visible on the bus.
	@get_indicator_list: List all of the indicators that are visible.
	@get_indicator_list_by_type: List all of the indicators of a given
		type that are visible.
	@get_indicator_property: Get a property from a particular indicator.
	@get_indicator_property_group: Get the values for a set of properties
		as an array of entries, returning an array as well.
	@get_indicator_properties: Get a list of all the properties that are
		on a particular indicator.
	@show_indicator_to_user: Respond to someone on the bus asking to show
		a particular indicator to the user.
	@get_next_id: Get the next unused indicator ID.
	@show_interest: React to someone signifying that they are interested
		in this server.
	@remove_interest: Someone on the bus is no longer interest in this
		server, remove it's interest.
	@check_interest: Check to see if anyone on the bus is interested in this
		server for a particular feature.
	@indicate_server_reserved1: Reserved for future use
	@indicate_server_reserved2: Reserved for future use
	@indicate_server_reserved3: Reserved for future use
	@indicate_server_reserved4: Reserved for future use

	All of the functions and signals that make up the server class
	including those that are public API to the application and those
	that are public API to all of DBus.  Subclasses may need to
	implement a large portion of these.
*/
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
	void (* interest_added) (IndicateServer * server, IndicateInterests interest);
	void (* interest_removed) (IndicateServer * server, IndicateInterests interest);

	/* Virtual Functions */
	gboolean (*get_indicator_count) (IndicateServer * server, guint * count, GError **error);
	gboolean (*get_indicator_count_by_type) (IndicateServer * server, gchar * type, guint * count, GError **error);
	gboolean (*get_indicator_list) (IndicateServer * server, GArray ** indicators, GError ** error);
	gboolean (*get_indicator_list_by_type) (IndicateServer * server, gchar * type, GArray ** indicators, GError ** error);
	gboolean (*get_indicator_property) (IndicateServer * server, guint id, gchar * property, gchar ** value, GError **error);
	gboolean (*get_indicator_property_group) (IndicateServer * server, guint id, GPtrArray * properties, gchar *** value, GError **error);
	gboolean (*get_indicator_properties) (IndicateServer * server, guint id, gchar *** properties, GError **error);
	gboolean (*show_indicator_to_user) (IndicateServer * server, guint id, GError ** error);
	guint    (*get_next_id) (IndicateServer * server);
	gboolean (*show_interest) (IndicateServer * server, gchar * sender, IndicateInterests interest);
	gboolean (*remove_interest) (IndicateServer * server, gchar * sender, IndicateInterests interest);
	gboolean (*check_interest) (IndicateServer * server, IndicateInterests interest);

	/* Reserver for future use */
	void (*indicate_server_reserved1)(void);
	void (*indicate_server_reserved2)(void);
	void (*indicate_server_reserved3)(void);
	void (*indicate_server_reserved4)(void);
};

GType indicate_server_get_type (void) G_GNUC_CONST;

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

/* Check to see if there is someone, out there, who likes this */
gboolean indicate_server_check_interest (IndicateServer * server, IndicateInterests interest);


/* Signal emission functions for sub-classes of the server */
void indicate_server_emit_indicator_added (IndicateServer *server, guint id, const gchar *type);
void indicate_server_emit_indicator_removed (IndicateServer *server, guint id, const gchar *type);
void indicate_server_emit_indicator_modified (IndicateServer *server, guint id, const gchar *property);
void indicate_server_emit_server_display (IndicateServer *server);

/**
	SECTION:server
	@short_description: The representation of the application on DBus.
	@stability: Unstable
	@include: libindicate/server.h

	The server object is the object that provides the functions on
	to DBus for other applications to call.  It does this by implementing
	the DBus indicator spec, but it also allows for subclassing so that
	subclasses don't have to worry about the DBus-isms as much as
	the functionality that they're trying to express.

	For simple applications there is limited need to set anything
	more than the desktop file and the type of the server using
	indicate_server_set_desktop_file() and indicate_server_set_type().
	Each of these function sets the respective value and expresses
	it in a way that other applications on the bus can read it.

	More advanced applications might find the need to subclass the
	#IndicateServer object and make their own.  This is likely the
	case where applications have complex data stores that they don't
	want to turn into a large set of #GObjects that can take up a
	significant amount of memory in the program.

	In general, it is recommended that application authors go with
	the high memory path first, and then optimize by implementing
	their server on a second pass.
*/

G_END_DECLS

#endif /* INDICATE_SERVER_H_INCLUDED__ */

