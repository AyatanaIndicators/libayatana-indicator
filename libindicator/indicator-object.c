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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "indicator.h"
#include "indicator-object.h"

/**
	IndicatorObjectPrivate:
	@label: The label representing this indicator or #NULL if none.
	@icon: The icon representing this indicator or #NULL if none.
	@menu: The menu representing this indicator or #NULL if none.

	Structure to define the memory for the private area
	of the object instance.
*/
typedef struct _IndicatorObjectPrivate IndicatorObjectPrivate;
struct _IndicatorObjectPrivate {
	GtkLabel * label;
	GtkImage * icon;
	GtkMenu * menu;
};

#define INDICATOR_OBJECT_GET_PRIVATE(o) \
			(G_TYPE_INSTANCE_GET_PRIVATE ((o), INDICATOR_OBJECT_TYPE, IndicatorObjectPrivate))

static void indicator_object_class_init (IndicatorObjectClass *klass);
static void indicator_object_init       (IndicatorObject *self);
static void indicator_object_dispose    (GObject *object);
static void indicator_object_finalize   (GObject *object);

G_DEFINE_TYPE (IndicatorObject, indicator_object, G_TYPE_OBJECT);

/* Setup the class and put the functions into the
   class structure */
static void
indicator_object_class_init (IndicatorObjectClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IndicatorObjectPrivate));

	object_class->dispose = indicator_object_dispose;
	object_class->finalize = indicator_object_finalize;

	return;
}

/* Initialize an instance */
static void
indicator_object_init (IndicatorObject *self)
{
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(self);

	priv->label = NULL;
	priv->icon = NULL;
	priv->menu = NULL;

	return;
}

/* Unref the objects that we're holding on to. */
static void
indicator_object_dispose (GObject *object)
{
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(object);
	
	if (priv->label != NULL) {
		g_object_unref(priv->label);
		priv->label = NULL;
	}

	if (priv->icon != NULL) {
		g_object_unref(priv->icon);
		priv->icon = NULL;
	}

	if (priv->menu != NULL) {
		g_object_unref(priv->menu);
		priv->menu = NULL;
	}

	G_OBJECT_CLASS (indicator_object_parent_class)->dispose (object);
	return;
}

/* Free memory */
static void
indicator_object_finalize (GObject *object)
{

	G_OBJECT_CLASS (indicator_object_parent_class)->finalize (object);
	return;
}

/**
	indicator_object_new_from_file:
	@file: Filename containing a loadable module

	This function builds an #IndicatorObject using the symbols
	that are found in @file.  The module is loaded and the
	references are all kept by the object.  To unload the 
	module the object must be destroyed.

	Return value: A valid #IndicatorObject or #NULL if error.
*/
IndicatorObject *
indicator_object_new_from_file (const gchar * file)
{
	/* Check to make sure the name exists and that the
	   file itself exists */
	if (file == NULL) {
		g_warning("Invalid filename.");
		return NULL;
	}

	if (!g_file_test(file, G_FILE_TEST_EXISTS)) {
		g_warning("File '%s' does not exist.", file);
		return NULL;
	}

	/* Grab the g_module reference, pull it in but let's
	   keep the symbols local to avoid conflicts. */
	GModule * module = g_module_open(file,
                                     G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
	if(module == NULL) {
		g_warning("Unable to load module: %s", file);
		return NULL;
	}

	/* Look for the version function, error if not found. */
	get_version_t lget_version = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_VERSION_S, (gpointer *)(&lget_version))) {
		g_warning("Unable to get the symbol for getting the version.");
		return NULL;
	}

	/* Check the version with the macro and make sure we're
	   all talking the same language. */
	if (!INDICATOR_VERSION_CHECK(lget_version())) {
		g_warning("Indicator using API version '%s' we're expecting '%s'", lget_version(), INDICATOR_VERSION);
		return NULL;
	}

	/* A this point we allocate the object, any code beyond
	   here needs to deallocate it if we're returning in an
	   error'd state. */
	GObject * object = g_object_new(INDICATOR_OBJECT_TYPE, NULL);
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(object);

	/* The function for grabbing a label from the module
	   execute it, and make sure everything is a-okay */
	get_label_t lget_label = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_LABEL_S, (gpointer *)(&lget_label))) {
		g_warning("Unable to get '" INDICATOR_GET_LABEL_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_label == NULL) {
		g_warning("Symbol '" INDICATOR_GET_LABEL_S "' is (null) in module: %s", file);
		goto unrefandout;
	}
	priv->label = lget_label();
	if (priv->label) {
		g_object_ref(G_OBJECT(priv->label));
	}

	/* The function for grabbing an icon from the module
	   execute it, and make sure everything is a-okay */
	get_icon_t lget_icon = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_ICON_S, (gpointer *)(&lget_icon))) {
		g_warning("Unable to get '" INDICATOR_GET_ICON_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_icon == NULL) {
		g_warning("Symbol '" INDICATOR_GET_ICON_S "' is (null) in module: %s", file);
		goto unrefandout;
	}
	priv->icon = lget_icon();
	if (priv->icon) {
		g_object_ref(G_OBJECT(priv->icon));
	}

	/* The function for grabbing a menu from the module
	   execute it, and make sure everything is a-okay */
	get_menu_t lget_menu = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_MENU_S, (gpointer *)(&lget_menu))) {
		g_warning("Unable to get '" INDICATOR_GET_MENU_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_menu == NULL) {
		g_warning("Symbol '" INDICATOR_GET_MENU_S "' is (null) in module: %s", file);
		goto unrefandout;
	}
	priv->menu = lget_menu();
	if (priv->menu) {
		g_object_ref(G_OBJECT(priv->menu));
	}

	if (priv->label == NULL && priv->icon == NULL) {
		/* This is the case where there is nothing to display,
		   kinda odd that we'd have a module with nothing. */
		g_warning("No label or icon.  Odd.");
		goto unrefandout;
	}

	return INDICATOR_OBJECT(object);

	/* Error, let's drop the object and return NULL.  Sad when
	   this happens. */
unrefandout:
	g_object_unref(object);
	return NULL;
}

/**
	indicator_object_get_label:
	@io: An #IndicatorObject.

	A function to get the label for a particular object.  This
	function does not increase the refcount.  That's your job
	if you want to do it.

	Return value: A #GtkLabel or #NULL if unavailable.
*/
GtkLabel *
indicator_object_get_label (IndicatorObject * io)
{
	g_return_val_if_fail(IS_INDICATOR_OBJECT(io), NULL);
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(io);
	return priv->label;
}

/**
	indicator_object_get_icon:
	@io: An #IndicatorObject.

	A function to get the icon for a particular object.  This
	function does not increase the refcount.  That's your job
	if you want to do it.

	Return value: A #GtkImage or #NULL if unavailable.
*/
GtkImage *
indicator_object_get_icon (IndicatorObject * io)
{
	g_return_val_if_fail(IS_INDICATOR_OBJECT(io), NULL);
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(io);
	return priv->icon;
}

/**
	indicator_object_get_menu:
	@io: An #IndicatorObject.

	A function to get the menu for a particular object.  This
	function does not increase the refcount.  That's your job
	if you want to do it.

	Return value: A #GtkMenu or #NULL if unavailable.
*/
GtkMenu *
indicator_object_get_menu (IndicatorObject * io)
{
	g_return_val_if_fail(IS_INDICATOR_OBJECT(io), NULL);
	IndicatorObjectPrivate * priv = INDICATOR_OBJECT_GET_PRIVATE(io);
	return priv->menu;
}
