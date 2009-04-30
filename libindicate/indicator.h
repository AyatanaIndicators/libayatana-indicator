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

#ifndef INDICATE_INDICATOR_H_INCLUDED__
#define INDICATE_INDICATOR_H_INCLUDED__ 1

#include <glib.h>
#include <glib-object.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

/* Boilerplate */
#define INDICATE_TYPE_INDICATOR (indicate_indicator_get_type ())
#define INDICATE_INDICATOR(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_INDICATOR, IndicateIndicator))
#define INDICATE_IS_INDICATOR(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_INDICATOR))
#define INDICATE_INDICATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), INDICATE_TYPE_INDICATOR, IndicateIndicatorClass))
#define INDICATE_IS_INDICATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), INDICATE_TYPE_INDICATOR))
#define INDICATE_INDICATOR_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), INDICATE_TYPE_INDICATOR, IndicateIndicatorClass))

/* This is a signal that signals to the indicator that the user
 * has done an action where they'd like this indicator to be
 * displayed. */
#define INDICATE_INDICATOR_SIGNAL_HIDE  "hide"
#define INDICATE_INDICATOR_SIGNAL_SHOW  "show"
#define INDICATE_INDICATOR_SIGNAL_DISPLAY  "user-display"
#define INDICATE_INDICATOR_SIGNAL_MODIFIED  "modified"

typedef struct _IndicateIndicator IndicateIndicator;
typedef struct _IndicateIndicatorClass IndicateIndicatorClass;

/**
	IndicateIndicator:

	The indicator object represents a single item that is shared over the
	indicator bus.  This could be something like one IM, one e-mail or 
	a single system update.  It should be accessed only through its
	accessors.
*/
struct _IndicateIndicator {
	GObject parent;
};

/**
	IndicateIndicatorClass:
	@parent_class: Parent class #GObjectClass.
	@hide: Slot for #IndicateIndicator::hide.
	@show: Slot for #IndicateIndicator::show.
	@user_display: Slot for #IndicateIndicator::user-display.
	@modified: Slot for #IndicateIndicator::modified.
	@get_type: Returns a constant string for the type of this indicator.
		Typically gets overridden by subclasses and defines the type of
		the indicator.  Is called by indicate_indicator_get_indicator_type().
	@set_property: Called when indicate_indicator_set_property() is called
		and should set the value.  While typically it is overridden by
		subclasses they usually handle special properties themselves and
		then call the superclass for storage.
	@get_property: Called when indicate_indicator_get_property() is called
		and should return the value requested.  Many times this is left alone.
	@list_properties: Called when indicate_indicator_list_properties() is called
		and returns a list of the properties available.  Again this can be
		overridden by subclasses to handle special properties.

	All of the functions that are used to modify or change data that is
	in the indicator.  Typically gets subclassed by other types of 
	indicators, for example #IndicateIndicatorMessages.

*/
struct _IndicateIndicatorClass {
	GObjectClass parent_class;

	void (*hide) (IndicateIndicator * indicator, gpointer data);
	void (*show) (IndicateIndicator * indicator, gpointer data);
	void (*user_display) (IndicateIndicator * indicator, gpointer data);
	void (*modified) (IndicateIndicator * indicator, gchar * property, gpointer data);

	const gchar * (*get_type) (IndicateIndicator * indicator);
	void (*set_property) (IndicateIndicator * indicator, const gchar * key, const gchar * data);
	const gchar * (*get_property) (IndicateIndicator * indicator, const gchar * key);
	GPtrArray * (*list_properties) (IndicateIndicator * indicator);
};

GType indicate_indicator_get_type(void) G_GNUC_CONST;

IndicateIndicator * indicate_indicator_new (void);

/* Show and hide this indicator */
void indicate_indicator_show (IndicateIndicator * indicator);
void indicate_indicator_hide (IndicateIndicator * indicator);

gboolean indicate_indicator_is_visible (IndicateIndicator * indicator);

/* Every entry has an ID, here's how to get it */
guint indicate_indicator_get_id (IndicateIndicator * indicator);

/* Every entry has a type.  This should be created by the
 * subclass and exported through this pretty function */
const gchar * indicate_indicator_get_indicator_type (IndicateIndicator * indicator);

void indicate_indicator_user_display (IndicateIndicator * indicator);

/* Properties handling */
void indicate_indicator_set_property (IndicateIndicator * indicator, const gchar * key, const gchar * data);
void indicate_indicator_set_property_icon (IndicateIndicator * indicator, const gchar * key, const GdkPixbuf * data);
void indicate_indicator_set_property_time (IndicateIndicator * indicator, const gchar * key, GTimeVal * time);
const gchar * indicate_indicator_get_property (IndicateIndicator * indicator, const gchar * key);
GPtrArray * indicate_indicator_list_properties (IndicateIndicator * indicator);

G_END_DECLS

#endif /* INDICATE_INDICATOR_H_INCLUDED__ */

