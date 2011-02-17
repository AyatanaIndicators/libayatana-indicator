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

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
  INDICATOR_OBJECT_SCROLL_UP,
  INDICATOR_OBJECT_SCROLL_DOWN,
  INDICATOR_OBJECT_SCROLL_LEFT,
  INDICATOR_OBJECT_SCROLL_RIGHT
} IndicatorScrollDirection;

#define INDICATOR_OBJECT_TYPE            (indicator_object_get_type ())
#define INDICATOR_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_OBJECT_TYPE, IndicatorObject))
#define INDICATOR_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))
#define INDICATOR_IS_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_OBJECT_TYPE))
#define INDICATOR_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_OBJECT_TYPE, IndicatorObjectClass))

#define INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED       "entry-added"
#define INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED_ID    (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED     "entry-removed"
#define INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED_ID  (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED       "entry-moved"
#define INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED_ID    (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_SCROLL            "scroll"
#define INDICATOR_OBJECT_SIGNAL_SCROLL_ID         (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_SCROLL, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_SCROLL_ENTRY      "scroll-entry"
#define INDICATOR_OBJECT_SIGNAL_SCROLL_ENTRY_ID   (g_signal_lookup(#define INDICATOR_OBJECT_SIGNAL_SCROLL_ENTRY, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_MENU_SHOW         "menu-show"
#define INDICATOR_OBJECT_SIGNAL_MENU_SHOW_ID      (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_MENU_SHOW, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_SHOW_NOW_CHANGED  "show-now-changed"
#define INDICATOR_OBJECT_SIGNAL_SHOW_NOW_CHANGED_ID (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_SHOW_NOW_CHANGED, INDICATOR_OBJECT_TYPE))
#define INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE "accessible-desc-update"
#define INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE_ID (g_signal_lookup(INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE, INDICATOR_OBJECT_TYPE))

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
	@get_accessible_desc: Gets the accessible descriptionfor this
		object.
	@get_entries: Gets all of the entires for this object returning
		a #GList of #IndicatorObjectEntries.  The list should be
		under the ownership of the caller but the entires will
		not be.
	@get_location: Returns the location that a particular entry
		should be placed in.  This is really only relevant for
		indicators that have more than one entry.
	@get_show_now: Returns whether the entry is requesting to
		be shown "right now" in that it has something important
		to tell the user.
	@entry_activate: Should be called when the menus for a given
		entry are shown to the user.
	@entry_close: Called when the menu is closed.
	@entry_added: Slot for #IndicatorObject::entry-added
	@entry_removed: Slot for #IndicatorObject::entry-removed
	@entry_moved: Slot for #IndicatorObject::entry-moved
	@menu_show: Slot for #IndicatorObject::menu-show
	@show_now_changed: Slot for #IndicatorObject::show-now-changed
	@accessible_desc_update: Slot for #IndicatorObject::accessible-desc-update
*/
struct _IndicatorObjectClass {
	GObjectClass parent_class;

	/* Virtual Functions */
	GtkLabel * (*get_label) (IndicatorObject * io);
	GtkImage * (*get_image) (IndicatorObject * io);
	GtkMenu  * (*get_menu)  (IndicatorObject * io);
	const gchar * (*get_accessible_desc) (IndicatorObject * io);

	GList *    (*get_entries) (IndicatorObject * io);
	guint      (*get_location) (IndicatorObject * io, IndicatorObjectEntry * entry);
	gboolean   (*get_show_now) (IndicatorObject * io, IndicatorObjectEntry * entry);

	void       (*entry_activate) (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp);
	void       (*entry_close) (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp);

	/* Signals */
	void       (*entry_added)   (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data);
	void       (*entry_removed) (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data);
	void       (*entry_moved)   (IndicatorObject * io, IndicatorObjectEntry * entry, guint old_pos, guint new_pos, gpointer user_data);
	void       (*scroll)        (IndicatorObject * io, gint delta, IndicatorScrollDirection direction);
	void       (*menu_show)     (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp, gpointer user_data);
	void       (*show_now_changed) (IndicatorObject * io, IndicatorObjectEntry * entry, gboolean show_now_state, gpointer user_data);
	void       (*scroll_entry)  (IndicatorObject * io, IndicatorObjectEntry * entry, gint delta, IndicatorScrollDirection direction);
	void       (*accessible_desc_update) (IndicatorObject * io, IndicatorObjectEntry * entry, gpointer user_data);

	/* Reserved */
	void       (*reserved1)     (void);
	void       (*reserved2)     (void);
	void       (*reserved3)     (void);
	void       (*reserved4)     (void);
	void       (*reserved5)     (void);
	void       (*reserved6)     (void);
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
	@accessible_desc: The accessible description
		of the indicator

	@reserved1: Reserved for future use
	@reserved2: Reserved for future use
	@reserved3: Reserved for future use
	@reserved4: Reserved for future use
*/
struct _IndicatorObjectEntry {
	GtkLabel * label;
	GtkImage * image;
	GtkMenu  * menu;
	const gchar * accessible_desc;

	void       (*reserved1)     (void);
	void       (*reserved2)     (void);
	void       (*reserved3)     (void);
	void       (*reserved4)     (void);
};

GType indicator_object_get_type (void);
IndicatorObject * indicator_object_new_from_file (const gchar * file);

GList * indicator_object_get_entries (IndicatorObject * io);
guint   indicator_object_get_location (IndicatorObject * io, IndicatorObjectEntry * entry);
guint   indicator_object_get_show_now (IndicatorObject * io, IndicatorObjectEntry * entry);
void    indicator_object_entry_activate (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp);
void    indicator_object_entry_close (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp);

G_END_DECLS

#endif
