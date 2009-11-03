/*
An object to represent loadable indicator modules to make loading
them easy and objectified.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 3.0 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License version 3.0 for more details.

You should have received a copy of the GNU General Public
License along with this library. If not, see
<http://www.gnu.org/licenses/>.
*/

#ifndef __INDICATOR_OBJECT_H__
#define __INDICATOR_OBJECT_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_OBJECT_TYPE            (indicator_object_get_type ())
#define INDICATOR_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_OBJECT_TYPE, IndicatorObject))
#define INDICATOR_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))
#define IS_INDICATOR_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_OBJECT_TYPE))
#define IS_INDICATOR_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))

typedef struct _IndicatorObject        IndicatorObject;
typedef struct _IndicatorObjectClass   IndicatorObjectClass;
typedef struct _IndicatorObjectPrivate IndicatorObjectPrivate;
typedef struct _IndicatorObjectEntry   IndicatorObjectEntry;

/**
	IndicatorObjectClass:
	@parent_class: #GObjectClass
	@get_label: Gets the label for this object.  Should be set
		to #NULL if @get_entries is set.  Should NOT ref the
		object.
	@get_image: Gets the image for this object.  Should be set
		to #NULL if @get_entries is set.  Should NOT ref the
		object.
	@get_menu: Gets the image for this object.  Should be set
		to #NULL if @get_entries is set.  Should NOT ref the
		object.
	@get_entires: Gets all of the entires for this object returning
		a #GList of #IndicatorObjectEntries.  The list should be
		under the ownership of the caller but the entires will
		not be.
	@entry_added: Slot for #IndicatorObject::entry-added
	@entry_removed: Slot for #IndicatorObject::entry-removed
	@indicator_object_reserved_1: Reserved for future use
	@indicator_object_reserved_2: Reserved for future use
	@indicator_object_reserved_3: Reserved for future use
	@indicator_object_reserved_4: Reserved for future use
*/
struct _IndicatorObjectClass {
	GObjectClass parent_class;
	
	/* Virtual Functions */
	GtkLabel * get_label (IndicatorObject * io);
	GtkImage * get_image (IndicatorObject * io);
	GtkMenu  * get_menu  (IndicatorObject * io);

	GList *    get_entries (IndicatorObject * io);

	/* Signals */
	void       entry_added (IndicatorObject * io, IndicatorEntry * entry, gpointer user_data);
	void       entry_removed (IndicatorObject * io, IndicatorEntry * entry, gpointer user_data);

	/* Reserved */
	void (* indicator_object_reserved_1) (void);
	void (* indicator_object_reserved_2) (void);
	void (* indicator_object_reserved_3) (void);
	void (* indicator_object_reserved_4) (void);
};

/**
	IndicatorObject:
	@parent: #GObject
	@priv: A cached reference to the private data for the
		instance.
*/
struct _IndicatorObject {
	GObject parent;
	IndicatorObjectPrivate * priv;
};

/**
	IndicatorObjectEntry:
	@label: The label to be shown on the panel
	@image: The image to be shown on the panel
	@menu: The menu to be added to the menubar
*/
struct _IndicatorObjectEntry {
	GtkLabel * label;
	GtkImage * image;
	GtkMenu  * menu;
};

GType indicator_object_get_type (void);
IndicatorObject * indicator_object_new_from_file (const gchar * file);

GList * indicator_object_get_entries (IndicatorObject * io);

G_END_DECLS

#endif
