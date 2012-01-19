/*
Test for libindicator

Copyright 2012 Canonical Ltd.

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

#include "dummy-indicator-entry-func.h"


GType dummy_indicator_entry_func_get_type (void);

INDICATOR_SET_VERSION
INDICATOR_SET_TYPE(DUMMY_INDICATOR_ENTRY_FUNC_TYPE)


GtkLabel *
get_label (IndicatorObject * io)
{
	return NULL;
}

GtkImage *
get_icon (IndicatorObject * io)
{
	return NULL;
}

GtkMenu *
get_menu (IndicatorObject * io)
{
	return NULL;
}
const gchar *
get_accessible_desc (IndicatorObject * io)
{
	return NULL;
}

static void
entry_activate (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp)
{
	DummyIndicatorEntryFunc * self = DUMMY_INDICATOR_ENTRY_FUNC(io);
	self->entry_activate_called = TRUE;
	return;
}

static void
entry_activate_window (IndicatorObject * io, IndicatorObjectEntry * entry, guint windowid, guint timestamp)
{
	DummyIndicatorEntryFunc * self = DUMMY_INDICATOR_ENTRY_FUNC(io);
	self->entry_activate_window_called = TRUE;
	return;
}

static void
entry_close (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp)
{
	DummyIndicatorEntryFunc * self = DUMMY_INDICATOR_ENTRY_FUNC(io);
	self->entry_close_called = TRUE;
	return;
}


static void dummy_indicator_entry_func_class_init (DummyIndicatorEntryFuncClass *klass);
static void dummy_indicator_entry_func_init       (DummyIndicatorEntryFunc *self);
static void dummy_indicator_entry_func_dispose    (GObject *object);
static void dummy_indicator_entry_func_finalize   (GObject *object);

G_DEFINE_TYPE (DummyIndicatorEntryFunc, dummy_indicator_entry_func, INDICATOR_OBJECT_TYPE);

static void
dummy_indicator_entry_func_class_init (DummyIndicatorEntryFuncClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = dummy_indicator_entry_func_dispose;
	object_class->finalize = dummy_indicator_entry_func_finalize;

	IndicatorObjectClass * io_class = INDICATOR_OBJECT_CLASS(klass);

	io_class->get_label = get_label;
	io_class->get_image = get_icon;
	io_class->get_menu = get_menu;
	io_class->get_accessible_desc = get_accessible_desc;

	io_class->entry_activate = entry_activate;
	io_class->entry_activate_window = entry_activate_window;
	io_class->entry_close = entry_close;

	return;
}

static void
dummy_indicator_entry_func_init (DummyIndicatorEntryFunc *self)
{

	return;
}

static void
dummy_indicator_entry_func_dispose (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_entry_func_parent_class)->dispose (object);
	return;
}

static void
dummy_indicator_entry_func_finalize (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_entry_func_parent_class)->finalize (object);
	return;
}
