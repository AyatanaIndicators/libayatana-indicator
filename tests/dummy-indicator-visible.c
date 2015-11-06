/*
Test for libindicator

Copyright 2012 Canonical Ltd.

Authors:
    Charles Kerr <charles.kerr@canonical.com>

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

#define DUMMY_INDICATOR_VISIBLE_TYPE            (dummy_indicator_visible_get_type ())
#define DUMMY_INDICATOR_VISIBLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUMMY_INDICATOR_VISIBLE_TYPE, DummyIndicatorVisible))
#define DUMMY_INDICATOR_VISIBLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUMMY_INDICATOR_VISIBLE_TYPE, DummyIndicatorVisibleClass))
#define IS_DUMMY_INDICATOR_VISIBLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUMMY_INDICATOR_VISIBLE_TYPE))
#define IS_DUMMY_INDICATOR_VISIBLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUMMY_INDICATOR_VISIBLE_TYPE))
#define DUMMY_INDICATOR_VISIBLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUMMY_INDICATOR_VISIBLE_TYPE, DummyIndicatorVisibleClass))

typedef struct _DummyIndicatorVisible      DummyIndicatorVisible;
typedef struct _DummyIndicatorVisibleClass DummyIndicatorVisibleClass;

struct _DummyIndicatorVisibleClass {
	IndicatorObjectClass parent_class;
};

struct _DummyIndicatorVisible {
	IndicatorObject parent;
};

GType dummy_indicator_visible_get_type (void);

INDICATOR_SET_VERSION
INDICATOR_SET_TYPE(DUMMY_INDICATOR_VISIBLE_TYPE)

GtkLabel *
get_label (IndicatorObject * io)
{
	return GTK_LABEL(gtk_label_new("Visible Item"));
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
	return "Visible Item";
}

static void dummy_indicator_visible_class_init (DummyIndicatorVisibleClass *klass);
static void dummy_indicator_visible_init       (DummyIndicatorVisible *self);
static void dummy_indicator_visible_dispose    (GObject *object);
static void dummy_indicator_visible_finalize   (GObject *object);

G_DEFINE_TYPE (DummyIndicatorVisible, dummy_indicator_visible, INDICATOR_OBJECT_TYPE);

static void
dummy_indicator_entry_being_removed (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	IndicatorObjectClass * indicator_object_class = INDICATOR_OBJECT_CLASS (dummy_indicator_visible_parent_class);

	g_object_set_data(G_OBJECT(entry->label), "is-hidden", GINT_TO_POINTER(1));

	if (indicator_object_class->entry_being_removed != NULL) {
		indicator_object_class->entry_being_removed (io, entry);
	}
}

static void
dummy_indicator_entry_was_added (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	IndicatorObjectClass * indicator_object_class = INDICATOR_OBJECT_CLASS (dummy_indicator_visible_parent_class);

	g_object_steal_data(G_OBJECT(entry->label), "is-hidden");

	if (indicator_object_class->entry_was_added != NULL) {
		indicator_object_class->entry_was_added (io, entry);
	}
}

static void
dummy_indicator_visible_class_init (DummyIndicatorVisibleClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = dummy_indicator_visible_dispose;
	object_class->finalize = dummy_indicator_visible_finalize;

	IndicatorObjectClass * io_class = INDICATOR_OBJECT_CLASS(klass);

	io_class->get_label = get_label;
	io_class->get_image = get_icon;
	io_class->get_menu = get_menu;
	io_class->get_accessible_desc = get_accessible_desc;
	io_class->entry_being_removed = dummy_indicator_entry_being_removed;
	io_class->entry_was_added = dummy_indicator_entry_was_added;
}

static void
dummy_indicator_visible_init (DummyIndicatorVisible *self)
{
}

static void
dummy_indicator_visible_dispose (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_visible_parent_class)->dispose (object);
}

static void
dummy_indicator_visible_finalize (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_visible_parent_class)->finalize (object);
}
