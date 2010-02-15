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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator-desktop-shortcuts.h"

typedef struct _IndicatorDesktopShortcutsPrivate IndicatorDesktopShortcutsPrivate;
struct _IndicatorDesktopShortcutsPrivate {
	GKeyFile * keyfile;
};

#define INDICATOR_DESKTOP_SHORTCUTS_GET_PRIVATE(o) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_TYPE_DESKTOP_SHORTCUTS, IndicatorDesktopShortcutsPrivate))

static void indicator_desktop_shortcuts_class_init (IndicatorDesktopShortcutsClass *klass);
static void indicator_desktop_shortcuts_init       (IndicatorDesktopShortcuts *self);
static void indicator_desktop_shortcuts_dispose    (GObject *object);
static void indicator_desktop_shortcuts_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorDesktopShortcuts, indicator_desktop_shortcuts, G_TYPE_OBJECT);

static void
indicator_desktop_shortcuts_class_init (IndicatorDesktopShortcutsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorDesktopShortcutsPrivate));

	object_class->dispose = indicator_desktop_shortcuts_dispose;
	object_class->finalize = indicator_desktop_shortcuts_finalize;

	return;
}

static void
indicator_desktop_shortcuts_init (IndicatorDesktopShortcuts *self)
{
	IndicatorDesktopShortcutsPrivate * priv = INDICATOR_DESKTOP_SHORTCUTS_GET_PRIVATE(self);

	priv->keyfile = NULL;

	return;
}

static void
indicator_desktop_shortcuts_dispose (GObject *object)
{

	G_OBJECT_CLASS (indicator_desktop_shortcuts_parent_class)->dispose (object);
	return;
}

static void
indicator_desktop_shortcuts_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_desktop_shortcuts_parent_class)->finalize (object);
	return;
}

