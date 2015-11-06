/*
Test for libindicator

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

#include <glib.h>
#include <glib-object.h>

#include "libayatana-indicator/indicator.h"
#include "libayatana-indicator/indicator-object.h"

#define DUMMY_INDICATOR_SIGNALER_TYPE            (dummy_indicator_signaler_get_type ())
#define DUMMY_INDICATOR_SIGNALER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUMMY_INDICATOR_SIGNALER_TYPE, DummyIndicatorSignaler))
#define DUMMY_INDICATOR_SIGNALER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUMMY_INDICATOR_SIGNALER_TYPE, DummyIndicatorSignalerClass))
#define IS_DUMMY_INDICATOR_SIGNALER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUMMY_INDICATOR_SIGNALER_TYPE))
#define IS_DUMMY_INDICATOR_SIGNALER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUMMY_INDICATOR_SIGNALER_TYPE))
#define DUMMY_INDICATOR_SIGNALER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUMMY_INDICATOR_SIGNALER_TYPE, DummyIndicatorSignalerClass))

typedef struct _DummyIndicatorSignaler      DummyIndicatorSignaler;
typedef struct _DummyIndicatorSignalerClass DummyIndicatorSignalerClass;

struct _DummyIndicatorSignalerClass {
	IndicatorObjectClass parent_class;
};

struct _DummyIndicatorSignaler {
	IndicatorObject parent;
	IndicatorObjectEntry *entries;
};

GType dummy_indicator_signaler_get_type (void);

INDICATOR_SET_VERSION
INDICATOR_SET_TYPE(DUMMY_INDICATOR_SIGNALER_TYPE)

GtkLabel *
get_label (IndicatorObject * io)
{
	return GTK_LABEL(gtk_label_new("Signaler Item"));
}

GtkImage *
get_icon (IndicatorObject * io)
{
	return GTK_IMAGE(gtk_image_new());
}

GtkMenu *
get_menu (IndicatorObject * io)
{
	GtkMenu * main_menu = GTK_MENU(gtk_menu_new());
	GtkWidget * loading_item = gtk_menu_item_new_with_label("Loading...");
	gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), loading_item);
	gtk_widget_show(GTK_WIDGET(loading_item));

	return main_menu;
}

const gchar *
get_accessible_desc (IndicatorObject * io)
{
	return "Signaler Item";
}

static void dummy_indicator_signaler_class_init (DummyIndicatorSignalerClass *klass);
static void dummy_indicator_signaler_init       (DummyIndicatorSignaler *self);
static void dummy_indicator_signaler_dispose    (GObject *object);
static void dummy_indicator_signaler_finalize   (GObject *object);

G_DEFINE_TYPE (DummyIndicatorSignaler, dummy_indicator_signaler, INDICATOR_OBJECT_TYPE);

static void
dummy_indicator_signaler_class_init (DummyIndicatorSignalerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = dummy_indicator_signaler_dispose;
	object_class->finalize = dummy_indicator_signaler_finalize;

	IndicatorObjectClass * io_class = INDICATOR_OBJECT_CLASS(klass);

	io_class->get_label = get_label;
	io_class->get_image = get_icon;
	io_class->get_menu = get_menu;
	io_class->get_accessible_desc = get_accessible_desc;
	io_class->entry_being_removed  = NULL;
	io_class->entry_was_added  = NULL;

	return;
}

static gboolean
idle_signal (gpointer data)
{
	DummyIndicatorSignaler * self = DUMMY_INDICATOR_SIGNALER(data);

	IndicatorObjectEntry *added_entry, *removed_entry, *moved_entry;

	added_entry = &self->entries[0];
	moved_entry = &self->entries[1];
	removed_entry = &self->entries[2];

	added_entry->name_hint = "added";
	moved_entry->name_hint = "moved";
	removed_entry->name_hint = "removed";

	g_signal_emit(G_OBJECT(self), INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED_ID, 0, added_entry);
	g_signal_emit(G_OBJECT(self), INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED_ID, 0, moved_entry, 0, 1);
	g_signal_emit(G_OBJECT(self), INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED_ID, 0, removed_entry);

	return FALSE; /* Don't queue again */
}

static void
dummy_indicator_signaler_init (DummyIndicatorSignaler *self)
{
	self->entries = g_new0(IndicatorObjectEntry, 3);
	g_idle_add(idle_signal, self);
	return;
}

static void
dummy_indicator_signaler_dispose (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_signaler_parent_class)->dispose (object);
	return;
}

static void
dummy_indicator_signaler_finalize (GObject *object)
{
	DummyIndicatorSignaler * self = DUMMY_INDICATOR_SIGNALER(object);
	g_free (self->entries);
	G_OBJECT_CLASS (dummy_indicator_signaler_parent_class)->finalize (object);
	return;
}
