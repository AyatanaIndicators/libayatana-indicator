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

/* Build up the class */
static void
indicator_desktop_shortcuts_class_init (IndicatorDesktopShortcutsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorDesktopShortcutsPrivate));

	object_class->dispose = indicator_desktop_shortcuts_dispose;
	object_class->finalize = indicator_desktop_shortcuts_finalize;

	return;
}

/* Initialize instance data */
static void
indicator_desktop_shortcuts_init (IndicatorDesktopShortcuts *self)
{
	IndicatorDesktopShortcutsPrivate * priv = INDICATOR_DESKTOP_SHORTCUTS_GET_PRIVATE(self);

	priv->keyfile = NULL;

	return;
}

/* Clear object references */
static void
indicator_desktop_shortcuts_dispose (GObject *object)
{

	G_OBJECT_CLASS (indicator_desktop_shortcuts_parent_class)->dispose (object);
	return;
}

/* Free all memory */
static void
indicator_desktop_shortcuts_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_desktop_shortcuts_parent_class)->finalize (object);
	return;
}

/* API */

/**
	indicator_desktop_shortcuts_new:
	@file: The desktop file that would be opened to
		find the actions.
	@identity: This is a string that represents the identity
		that should be used in searching those actions.  It 
		relates to the ShowIn and NotShownIn properties.
	
	This function creates the basic object.  It involves opening
	the file and parsing it.  It could potentially block on IO.  At
	the end of the day you'll have a fully functional object.

	Return value: A new #IndicatorDesktopShortcuts object.
*/
IndicatorDesktopShortcuts *
indicator_desktop_shortcuts_new (const gchar * file, const gchar * identity)
{

	return NULL;
}

/**
	indicator_desktop_shortcuts_get_nicks:
	@ids: The #IndicatorDesktopShortcuts object to look in

	Give you the list of commands that are available for this desktop
	file given the identity that was passed in at creation.  This will
	filter out the various items in the desktop file.  These nicks can
	then be used as keys for working with the desktop file.

	Return value: A #NULL terminated list of strings.  This memory
		is managed by the @ids object.
*/
const gchar **
indicator_desktop_shortcuts_get_nicks (IndicatorDesktopShortcuts * ids)
{


	return NULL;
}

/**
	indicator_desktop_shortcuts_nick_get_name:
	@ids: The #IndicatorDesktopShortcuts object to look in
	@nick: Which command that we're referencing.

	This function looks in a desktop file for a nick to find the
	user visible name for that shortcut.  The @nick parameter
	should be gotten from #indicator_desktop_shortcuts_get_nicks
	though it's not required that the exact memory location
	be the same.

	Return value: A user visible string for the shortcut or
		#NULL on error.
*/
const gchar *
indicator_desktop_shortcuts_nick_get_name (IndicatorDesktopShortcuts * ids, const gchar * nick)
{

	return NULL;
}

/**
	indicator_desktop_shortcuts_nick_exec:
	@ids: The #IndicatorDesktopShortcuts object to look in
	@nick: Which command that we're referencing.

	Here we take a @nick and try and execute the action that is
	associated with it.  The @nick parameter should be gotten
	from #indicator_desktop_shortcuts_get_nicks though it's not
	required that the exact memory location be the same.

	Return value: #TRUE on success or #FALSE on error.
*/
gboolean
indicator_desktop_shortcuts_nick_exec (IndicatorDesktopShortcuts * ids, const gchar * nick)
{

	return FALSE;
}
