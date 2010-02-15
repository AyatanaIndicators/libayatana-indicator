/*
A small file to parse through the actions that are available
in the desktop file and making those easily usable.

Copyright 2010 Canonical Ltd.

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

#ifndef __INDICATOR_DESKTOP_SHORTCUTS_H__
#define __INDICATOR_DESKTOP_SHORTCUTS_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_TYPE_DESKTOP_SHORTCUTS            (indicator_desktop_shortcuts_get_type ())
#define INDICATOR_DESKTOP_SHORTCUTS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcuts))
#define INDICATOR_DESKTOP_SHORTCUTS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcutsClass))
#define IS_INDICATOR_DESKTOP_SHORTCUTS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_TYPE_DESKTOP_SHORTCUTS))
#define IS_INDICATOR_DESKTOP_SHORTCUTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_TYPE_DESKTOP_SHORTCUTS))
#define INDICATOR_DESKTOP_SHORTCUTS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcutsClass))

typedef struct _IndicatorDesktopShortcuts      IndicatorDesktopShortcuts;
typedef struct _IndicatorDesktopShortcutsClass IndicatorDesktopShortcutsClass;

struct _IndicatorDesktopShortcutsClass {
	GObjectClass parent_class;
};

struct _IndicatorDesktopShortcuts {
	GObject parent;
};

GType indicator_desktop_shortcuts_get_type (void);

G_END_DECLS

#endif
