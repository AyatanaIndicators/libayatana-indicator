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

#ifndef __DUMMY_INDICATOR_ENTRY_FUNC__
#define __DUMMY_INDICATOR_ENTRY_FUNC__

#include <glib.h>
#include <glib-object.h>

#include "libayatana-indicator/indicator.h"
#include "libayatana-indicator/indicator-object.h"

G_BEGIN_DECLS

#define DUMMY_INDICATOR_ENTRY_FUNC_TYPE            (dummy_indicator_entry_func_get_type ())
#define DUMMY_INDICATOR_ENTRY_FUNC(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUMMY_INDICATOR_ENTRY_FUNC_TYPE, DummyIndicatorEntryFunc))
#define DUMMY_INDICATOR_ENTRY_FUNC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUMMY_INDICATOR_ENTRY_FUNC_TYPE, DummyIndicatorEntryFuncClass))
#define IS_DUMMY_INDICATOR_ENTRY_FUNC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUMMY_INDICATOR_ENTRY_FUNC_TYPE))
#define IS_DUMMY_INDICATOR_ENTRY_FUNC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUMMY_INDICATOR_ENTRY_FUNC_TYPE))
#define DUMMY_INDICATOR_ENTRY_FUNC_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUMMY_INDICATOR_ENTRY_FUNC_TYPE, DummyIndicatorEntryFuncClass))

typedef struct _DummyIndicatorEntryFunc      DummyIndicatorEntryFunc;
typedef struct _DummyIndicatorEntryFuncClass DummyIndicatorEntryFuncClass;

struct _DummyIndicatorEntryFuncClass {
	IndicatorObjectClass parent_class;
};

struct _DummyIndicatorEntryFunc {
	IndicatorObject parent;

	gboolean entry_activate_called;
	gboolean entry_activate_window_called;
	gboolean entry_close_called;
};

#endif /* __DUMMY_INDICATOR_ENTRY_FUNC__ */
