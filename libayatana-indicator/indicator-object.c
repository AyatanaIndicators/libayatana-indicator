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
#include "indicator-object-marshal.h"
#include "indicator-object-enum-types.h"

/**
	@ENTRY_INIT: The entry hasn't been initialized yet, so its
		visibility will depend upon the inital-visibility property.
	@ENTRY_VISIBLE: The entry is visible
	@ENTRY_INVISIBLE: The entry is invisible
*/
typedef enum {
    ENTRY_INIT,
    ENTRY_VISIBLE,
    ENTRY_INVISIBLE
}
EntryVisibility;

typedef struct _IndicatorObjectEntryPrivate {
     EntryVisibility visibility;
}
IndicatorObjectEntryPrivate;

/**
	IndicatorObjectPrivate:
	@module: The loaded module representing the object.  Note to
		subclasses: This will not be set when you're initalized.
	@entry: A default entry for objects that don't need all the
		fancy stuff.  This works with #get_entries_default.
	@gotten_entries: A check to see if the @entry has been
		populated intelligently yet.

	Structure to define the memory for the private area
	of the object instance.
*/
struct _IndicatorObjectPrivate {
	GModule * module;

	/* For get_entries_default */
	IndicatorObjectEntry entry;
	gboolean gotten_entries;

	/* Whether or not entries are visible by default */
	gboolean default_visibility;
	GHashTable * entry_privates;

	GStrv environments;
};

/* Signals Stuff */
enum {
	ENTRY_ADDED,
	ENTRY_REMOVED,
	ENTRY_MOVED,
	ENTRY_SCROLLED,
	MENU_SHOW,
	SHOW_NOW_CHANGED,
	ACCESSIBLE_DESC_UPDATE,
	SECONDARY_ACTIVATE,
	LAST_SIGNAL
};

/* Properties */
/* Enum for the properties so that they can be quickly
   found and looked up. */
enum {
	PROP_0,
	PROP_GSETTINGS_SCHEMA_ID,
	PROP_DEFAULT_VISIBILITY,
};


static guint signals[LAST_SIGNAL] = { 0 };

/* GObject stuff */
static void indicator_object_class_init (IndicatorObjectClass *klass);
static void indicator_object_init       (IndicatorObject *self);
static void indicator_object_dispose    (GObject *object);
static void indicator_object_finalize   (GObject *object);
static void set_property (GObject*, guint prop_id, const GValue*, GParamSpec* );
static void get_property (GObject*, guint prop_id,       GValue*, GParamSpec* );

/* entries' visibility */
static GList * get_entries_default               (IndicatorObject*);
static GList * get_all_entries                   (IndicatorObject*);
static void indicator_object_entry_being_removed (IndicatorObject*, IndicatorObjectEntry*);
static void indicator_object_entry_was_added     (IndicatorObject*, IndicatorObjectEntry*);
static gint indicator_object_real_get_position   (IndicatorObject*);
static IndicatorObjectEntryPrivate * entry_get_private (IndicatorObject*, IndicatorObjectEntry*);

G_DEFINE_TYPE_WITH_PRIVATE (IndicatorObject, indicator_object, G_TYPE_OBJECT);

/* Setup the class and put the functions into the
   class structure */
static void
indicator_object_class_init (IndicatorObjectClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = indicator_object_dispose;
	object_class->finalize = indicator_object_finalize;
	object_class->set_property = set_property;
	object_class->get_property = get_property;

	klass->get_label =  NULL;
	klass->get_menu  =  NULL;
	klass->get_image =  NULL;
	klass->get_accessible_desc = NULL;
	klass->get_entries = get_entries_default;
	klass->get_location = NULL;
	klass->entry_being_removed = NULL;
	klass->entry_was_added = NULL;
	klass->get_position = indicator_object_real_get_position;

	klass->entry_activate = NULL;
	klass->entry_activate_window = NULL;
	klass->entry_close = NULL;

	/**
		IndicatorObject::entry-added:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			is being added.

		Signaled when a new entry is added and should
		be shown by the person using this object.
	*/
	signals[ENTRY_ADDED] = g_signal_new (INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,
	                                     G_TYPE_FROM_CLASS(klass),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET (IndicatorObjectClass, entry_added),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__POINTER,
	                                     G_TYPE_NONE, 1, G_TYPE_POINTER, G_TYPE_NONE);

	/**
		IndicatorObject::entry-removed:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			is being removed.

		Signaled when an entry is removed and should
		be removed by the person using this object.
	*/
	signals[ENTRY_REMOVED] = g_signal_new (INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED,
	                                       G_TYPE_FROM_CLASS(klass),
	                                       G_SIGNAL_RUN_LAST,
	                                       G_STRUCT_OFFSET (IndicatorObjectClass, entry_removed),
	                                       NULL, NULL,
	                                       g_cclosure_marshal_VOID__POINTER,
	                                       G_TYPE_NONE, 1, G_TYPE_POINTER, G_TYPE_NONE);
	/**
		IndicatorObject::entry-moved:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			is being moved.
		@arg2: The old location of the entry
		@arg3: The new location of the entry

		When the order of the entries change, then this signal
		is sent to tell the new location.
	*/
	signals[ENTRY_MOVED] = g_signal_new (INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED,
	                                     G_TYPE_FROM_CLASS(klass),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET (IndicatorObjectClass, entry_moved),
	                                     NULL, NULL,
	                                     _indicator_object_marshal_VOID__POINTER_UINT_UINT,
	                                     G_TYPE_NONE, 3, G_TYPE_POINTER, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_NONE);
	/**
		IndicatorObject::entry-scrolled:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			receives the scroll event.
		@arg2: The delta of the scroll event
		@arg3: The orientation of the scroll event.

		When the indicator receives a mouse scroll wheel event
		from the user, this signal is emitted.
	*/
	signals[ENTRY_SCROLLED] = g_signal_new (INDICATOR_OBJECT_SIGNAL_ENTRY_SCROLLED,
	                                G_TYPE_FROM_CLASS(klass),
	                                G_SIGNAL_RUN_LAST,
	                                G_STRUCT_OFFSET (IndicatorObjectClass, entry_scrolled),
	                                NULL, NULL,
	                                _indicator_object_marshal_VOID__POINTER_UINT_ENUM,
	                                G_TYPE_NONE, 3, G_TYPE_POINTER, G_TYPE_UINT,
	                                INDICATOR_OBJECT_TYPE_SCROLL_DIRECTION);
	/**
		IndicatorObject::secondary-activate:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			receives the secondary activate event.
		@arg2: The timestamp of the event

		When the indicator receives a secondary activation event
		from the user, this signal is emitted.
	*/
	signals[SECONDARY_ACTIVATE] = g_signal_new (INDICATOR_OBJECT_SIGNAL_SECONDARY_ACTIVATE,
	                                G_TYPE_FROM_CLASS(klass),
	                                G_SIGNAL_RUN_LAST,
	                                G_STRUCT_OFFSET (IndicatorObjectClass, secondary_activate),
	                                NULL, NULL,
	                                _indicator_object_marshal_VOID__POINTER_UINT,
	                                G_TYPE_NONE, 2, G_TYPE_POINTER, G_TYPE_UINT);

	/**
		IndicatorObject::menu-show:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			is being shown.
		@arg2: The timestamp of the event

		Used when the indicator wants to signal up the stack
		that the menu should be shown.
	*/
	signals[MENU_SHOW] = g_signal_new (INDICATOR_OBJECT_SIGNAL_MENU_SHOW,
	                                   G_TYPE_FROM_CLASS(klass),
	                                   G_SIGNAL_RUN_LAST,
	                                   G_STRUCT_OFFSET (IndicatorObjectClass, menu_show),
	                                   NULL, NULL,
	                                   _indicator_object_marshal_VOID__POINTER_UINT,
	                                   G_TYPE_NONE, 2, G_TYPE_POINTER, G_TYPE_UINT);

	/**
		IndicatorObject::show-now-changed:
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry that
			is changing it's state
		@arg2: The state of whether the entry should be shown

		Whether the entry should be shown or not has changed so we need
		to tell whoever is displaying it.
	*/
	signals[SHOW_NOW_CHANGED] = g_signal_new (INDICATOR_OBJECT_SIGNAL_SHOW_NOW_CHANGED,
	                                          G_TYPE_FROM_CLASS(klass),
	                                          G_SIGNAL_RUN_LAST,
	                                          G_STRUCT_OFFSET (IndicatorObjectClass, show_now_changed),
	                                          NULL, NULL,
	                                          _indicator_object_marshal_VOID__POINTER_BOOLEAN,
	                                          G_TYPE_NONE, 2, G_TYPE_POINTER, G_TYPE_BOOLEAN);

	/**
		IndicatorObject::accessible-desc-update::
		@arg0: The #IndicatorObject object
		@arg1: A pointer to the #IndicatorObjectEntry whos
			accessible description has been updated.

		Signaled when an indicator's accessible description
		has been updated, so that the displayer of the
		indicator can fetch the new description.
	*/
	signals[ACCESSIBLE_DESC_UPDATE] = g_signal_new (INDICATOR_OBJECT_SIGNAL_ACCESSIBLE_DESC_UPDATE,
	                                     G_TYPE_FROM_CLASS(klass),
	                                     G_SIGNAL_RUN_LAST,
	                                     G_STRUCT_OFFSET (IndicatorObjectClass, accessible_desc_update),
	                                     NULL, NULL,
	                                     g_cclosure_marshal_VOID__POINTER,
	                                     G_TYPE_NONE, 1, G_TYPE_POINTER, G_TYPE_NONE);

	/* Properties */

	GParamSpec * pspec = g_param_spec_boolean (INDICATOR_OBJECT_DEFAULT_VISIBILITY,
			"default visibility",
			"Whether or not entries should initially be visible.",
			TRUE,
			G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
	g_object_class_install_property (object_class, PROP_DEFAULT_VISIBILITY, pspec);
}

/* Initialize an instance */
static void
indicator_object_init (IndicatorObject *self)
{
	IndicatorObjectPrivate * priv = indicator_object_get_instance_private(self);

	priv->module = NULL;

	priv->entry.parent_object = self;
	priv->entry.menu = NULL;
	priv->entry.label = NULL;
	priv->entry.image = NULL;
	priv->entry.accessible_desc = NULL;
	priv->entry.name_hint = NULL;
	priv->entry.parent_window = 0;

	priv->gotten_entries = FALSE;
	priv->default_visibility = TRUE;
	priv->entry_privates = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);

	priv->environments = NULL;

	self->priv = priv;

	GObject * o = G_OBJECT(self);
	/* Invoke the entry-being-removed virtual function first */
	g_signal_connect (o, INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED,
	                  G_CALLBACK(indicator_object_entry_being_removed), NULL);
	/* Invoke the entry-was-added virtual function last */
	g_signal_connect_after (o, INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,
	                        G_CALLBACK(indicator_object_entry_was_added), NULL);
}

/* Unref the objects that we're holding on to. */
static void
indicator_object_dispose (GObject *object)
{
	/* Ensure that hidden entries are re-added so their widgetry will
	   be cleaned up properly by the client */
	indicator_object_set_visible (INDICATOR_OBJECT (object), TRUE);

	G_OBJECT_CLASS (indicator_object_parent_class)->dispose (object);
}

/* A small helper function that closes a module but
   in the function prototype of a GSourceFunc. */
static gboolean
module_unref (gpointer data)
{
	if (!g_module_close((GModule *)data)) {
		/* All we can do is warn. */
		g_warning("Unable to close module!");
	}
	return FALSE;
}

/* Free memory */
static void
indicator_object_finalize (GObject *object)
{

	IndicatorObject * obj = INDICATOR_OBJECT (object);
	IndicatorObjectPrivate * priv = indicator_object_get_instance_private(obj);

	if (priv->entry_privates != NULL) {
		g_hash_table_destroy (priv->entry_privates);
		priv->entry_privates = NULL;
	}

	if (priv->environments != NULL) {
		g_strfreev(priv->environments);
		priv->environments = NULL;
	}

	if (priv->module != NULL) {
		/* Wow, this is convoluted.  So basically we want to unref
		   the module which will cause the code it included to be
		   removed.  But, since its finalize function is the function
		   that called this one, we can't really remove it before
		   it finishes being executed.  So we're putting the job into
		   the main loop to remove it the next time it gets a chance.
		   Slightly non-deterministic, but should work. */
		g_idle_add(module_unref, priv->module);
		priv->module = NULL;
	}

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
	GObject * object = NULL;
	GModule * module = NULL;

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
	module = g_module_open(file,
                           G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
	if (module == NULL) {
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

	/* The function for grabbing a label from the module
	   execute it, and make sure everything is a-okay */
	get_type_t lget_type = NULL;
	if (!g_module_symbol(module, INDICATOR_GET_TYPE_S, (gpointer *)(&lget_type))) {
		g_warning("Unable to get '" INDICATOR_GET_TYPE_S "' symbol from module: %s", file);
		goto unrefandout;
	}
	if (lget_type == NULL) {
		g_warning("Symbol '" INDICATOR_GET_TYPE_S "' is (null) in module: %s", file);
		goto unrefandout;
	}

	/* A this point we allocate the object, any code beyond
	   here needs to deallocate it if we're returning in an
	   error'd state. */
	object = g_object_new(lget_type(), NULL);
	if (object == NULL) {
		g_warning("Unable to build an object if type '%d' in module: %s", (gint)lget_type(), file);
		goto unrefandout;
	}
	if (!INDICATOR_IS_OBJECT(object)) {
		g_warning("Type '%d' in file %s is not a subclass of IndicatorObject.", (gint)lget_type(), file);
		goto unrefandout;
	}

	/* Now we can track the module */
	IndicatorObject * obj = INDICATOR_OBJECT(object);
	IndicatorObjectPrivate * priv = indicator_object_get_instance_private(obj);
	priv->module = module;

	return obj;

	/* Error, let's drop the object and return NULL.  Sad when
	   this happens. */
unrefandout:
	g_clear_object (&object);
	g_clear_object (&module);
	g_warning("Error building IndicatorObject from file: %s", file);
	return NULL;
}

/* The default get entries function uses the other single
   entries in the class to create an entry structure and
   put it into a list.  This makes it simple for simple objects
   to create the list.  Small changes from the way they
   previously were. */
static GList *
get_entries_default (IndicatorObject * io)
{
	IndicatorObjectPrivate * priv = indicator_object_get_instance_private(io);

	if (!priv->gotten_entries) {
		IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

		priv->entry.parent_object = io;

		if (class->get_label) {
			priv->entry.label = class->get_label(io);
		}

		if (class->get_image) {
			priv->entry.image = class->get_image(io);
		}

		if (priv->entry.image == NULL && priv->entry.label == NULL) {
			g_warning("IndicatorObject class does not create an image or a label.  We need one of those.");
			return NULL;
		}

		if (class->get_menu) {
			priv->entry.menu = class->get_menu(io);
		}

		if (priv->entry.menu == NULL) {
			g_warning("IndicatorObject class does not create a menu.  We need one of those.");
			return NULL;
		}

		if (class->get_accessible_desc) {
			priv->entry.accessible_desc = class->get_accessible_desc(io);
		}

		if (priv->entry.accessible_desc == NULL) {
			g_warning("IndicatorObject class does not have an accessible description.");
		}

		if (class->get_name_hint) {
			priv->entry.name_hint = class->get_name_hint(io);
		}

		if (class->get_parent_window) {
			priv->entry.parent_window = class->get_parent_window(io);
		}

		priv->gotten_entries = TRUE;
	}

	return g_list_append(NULL, &(priv->entry));
}

/* returns a list of all IndicatorObjectEntries, visible or not */
static GList*
get_all_entries (IndicatorObject * io)
{
	GList * all_entries = NULL, *l;

	g_return_val_if_fail(INDICATOR_IS_OBJECT(io), NULL);
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	if (class->get_entries == NULL)
		g_error("No get_entries function on object.  It must have been deleted?!?!");
	else
	{
		all_entries = class->get_entries(io);

		for (l = all_entries; l; l = l->next)
		{
			IndicatorObjectEntry *entry = l->data;

			if (entry)
				entry->parent_object = io;
		}
	}

	return all_entries;
}

/* get the private structure that corresponds to a caller-specified entry */
static IndicatorObjectEntryPrivate *
entry_get_private (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	g_return_val_if_fail (INDICATOR_IS_OBJECT(io), NULL);
	g_return_val_if_fail (io->priv != NULL, NULL);

	GHashTable * h = io->priv->entry_privates;
	IndicatorObjectEntryPrivate * priv = g_hash_table_lookup (h, entry);
	if (priv == NULL)
	{
		priv = g_new0 (IndicatorObjectEntryPrivate, 1);
		priv->visibility = ENTRY_INIT;
		g_hash_table_insert (h, entry, priv);
	}

	return priv;
}

/**
	indicator_object_get_entries:
	@io: #IndicatorObject to query

	This function returns a list of visible entries. The list is built
	by calling the object's #IndicatorObjectClass::get_entries
	virtual function and testing each of the results for visibility.
	Callers should free the GList with g_list_free(), but the entries
	are owned by the IndicatorObject and should not be freed.

	Return value: (element-type IndicatorObjectEntry) (transfer container):
		A list if #IndicatorObjectEntry structures or NULL on error.
*/
GList *
indicator_object_get_entries (IndicatorObject * io)
{
	GList * l;
	GList * ret = NULL;
	GList * all_entries = get_all_entries (io);
	IndicatorObjectPrivate * priv = indicator_object_get_instance_private(io);
	const gboolean default_visibility = priv->default_visibility;

	for (l=all_entries; l!=NULL; l=l->next)
	{
		gboolean show_me;
		IndicatorObjectEntry * entry = l->data;

		switch (entry_get_private(io,entry)->visibility) {
			case ENTRY_VISIBLE:   show_me = TRUE; break;
			case ENTRY_INVISIBLE: show_me = FALSE; break;
			case ENTRY_INIT:      show_me = default_visibility; break;
			default:              show_me = TRUE; g_warn_if_reached(); break;
		}

		if (show_me)
			ret = g_list_prepend (ret, entry);
	}

	g_list_free (all_entries);
	return g_list_reverse (ret);
}

/**
	indicator_object_get_location:
	@io: #IndicatorObject to query
	@entry: The #IndicatorObjectEntry to look for.

	This function looks on the class for the object and calls
	it's #IndicatorObjectClass::get_location function.  If the
	function doesn't exist it returns zero.

	Return value: Location of the @entry in the display or
		zero if no location is specified.
*/
guint
indicator_object_get_location (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	g_return_val_if_fail(INDICATOR_IS_OBJECT(io), 0);
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	if (class->get_location) {
		return class->get_location(io, entry);
	}

	return 0;
}

/**
	indicator_object_get_show_now:
	@io: #IndicatorObject to query
	@entry: The #IndicatorObjectEntry to look for.

	This function returns whether the entry should be shown with
	priority on the panel.  If the object does not support checking
	it assumes that its entries should never have priority.

	Return value: Whether the entry should be shown with priority.
*/
guint
indicator_object_get_show_now (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	g_return_val_if_fail(INDICATOR_IS_OBJECT(io), 0);
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	if (class->get_show_now) {
		return class->get_show_now(io, entry);
	}

	return FALSE;
}

/**
	indicator_object_entry_activate_window:
	@io: #IndicatorObject to query
	@entry: The #IndicatorObjectEntry whose entry was shown
	@windowid: ID of the window that is currently focused (or will
		be very shortly)
	@timestamp: The X11 timestamp of the event

	Used to signal to the indicator that the menu on an entry has
	been clicked on.  This can either be an activate or a showing
	of the menu.  Also includes a window ID so that we can know what
	application is going to be getting focused soon.  If there is
	no override of this function, it is the same as calling
	indicator_object_entry_activate and in general is preferable
	if you have that information available.
*/
void
indicator_object_entry_activate_window (IndicatorObject * io, IndicatorObjectEntry * entry, guint windowid, guint timestamp)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(io));
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	if (class->entry_activate_window != NULL) {
		return class->entry_activate_window(io, entry, windowid, timestamp);
	} else {
		indicator_object_entry_activate(io, entry, timestamp);
	}

	return;
}

/**
	indicator_object_entry_activate:
	@io: #IndicatorObject to query
	@entry: The #IndicatorObjectEntry whose entry was shown
	@timestamp: The X11 timestamp of the event

	Used to signal to the indicator that the menu on an entry has
	been clicked on.  This can either be an activate or a showing
	of the menu.  Note, this does not actually show the menu that's
	left up to the reader.
*/
void
indicator_object_entry_activate (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(io));
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	if (class->entry_activate != NULL) {
		return class->entry_activate(io, entry, timestamp);
	}

	return;
}

/**
	indicator_object_entry_close:
	@io: #IndicatorObject to query
	@entry: The #IndicatorObjectEntry whose menu was closed
	@timestamp: The X11 timestamp of the event

	Used to tell the indicator that a menu has been closed for the
	entry that is specified.
*/
void
indicator_object_entry_close (IndicatorObject * io, IndicatorObjectEntry * entry, guint timestamp)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(io));
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	if (class->entry_close != NULL) {
		return class->entry_close(io, entry, timestamp);
	}

	return;
}

static void
indicator_object_entry_being_removed (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(io));
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	entry_get_private (io, entry)->visibility = ENTRY_INVISIBLE;

	if (entry)
		entry->parent_object = NULL;

	if (class->entry_being_removed != NULL)
	{
		class->entry_being_removed (io, entry);
	}
}

static void
indicator_object_entry_was_added (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(io));
	IndicatorObjectClass * class = INDICATOR_OBJECT_GET_CLASS(io);

	entry_get_private (io, entry)->visibility = ENTRY_VISIBLE;

	if (entry)
		entry->parent_object = io;

	if (class->entry_was_added != NULL)
	{
		class->entry_was_added (io, entry);
	}
}

static gint
indicator_object_real_get_position (IndicatorObject *io)
{
	g_return_val_if_fail (INDICATOR_IS_OBJECT (io), -1);

	return -1;
}

/**
	indicator_object_set_environment:
	@io: #IndicatorObject to set on
	@env: List of enviroment names to use

	Sets the names of the environment that the indicator is being
	loaded into.  This allows for indicators to behave differently
	in different hosts if need be.
*/
void
indicator_object_set_environment (IndicatorObject * io, GStrv env)
{
	/* FIXME: should this be a property? */
	g_return_if_fail(INDICATOR_IS_OBJECT(io));

	if (io->priv->environments != NULL) {
		g_strfreev(io->priv->environments);
		io->priv->environments = NULL;
	}

	io->priv->environments = g_strdupv(env);

	return;
}

/**
	indicator_object_get_environment:
	@io: #IndicatorObject to get the environment from

	Gets the list of environment strings that this object is
	placed into.

	Return value: (transfer none): Gets the list of strings that
	represent the environment or NULL if none were given.
*/
GStrv
indicator_object_get_environment (IndicatorObject * io)
{
	g_return_val_if_fail(INDICATOR_IS_OBJECT(io), NULL);
	return io->priv->environments;
}

/**
	indicator_object_check_environment:
	@io: #IndicatorObject to check on
	@env: Environment that we're looking for

	Convience function to check to see if the specified environment
	@env is in our list of environments.

	Return Value: Whether we're in environment @env
*/
gboolean
indicator_object_check_environment (IndicatorObject * io, const gchar * env)
{
	g_return_val_if_fail(INDICATOR_IS_OBJECT(io), FALSE);
	g_return_val_if_fail(env != NULL, FALSE);

	if (io->priv->environments == NULL) {
		return FALSE;
	}

	int i;
	for (i = 0; io->priv->environments[i] != NULL; i++) {
		if (g_strcmp0(env, io->priv->environments[i]) == 0) {
			return TRUE;
		}
	}

	return FALSE;
}

/**
	indicator_object_set_visible:
	@io: #IndicatorObject to check on
	@visible: whether or not the entries should be visible

	Used to set all of an indicator's entries to be visible or hidden.
*/
void
indicator_object_set_visible (IndicatorObject * io, gboolean visible)
{
	g_return_if_fail(INDICATOR_IS_OBJECT(io));

	GList * l;
	GList * entries = get_all_entries (io);
	const guint signal_id = signals[visible ? ENTRY_ADDED : ENTRY_REMOVED];
	EntryVisibility visibility = visible ? ENTRY_VISIBLE : ENTRY_INVISIBLE;
	const GQuark detail = (GQuark)0;

	for (l=entries; l!=NULL; l=l->next) {
		IndicatorObjectEntry *entry = l->data;
		if (entry_get_private (io, entry)->visibility != visibility)
			g_signal_emit(io, signal_id, detail, entry);
	}
	g_list_free (entries);
}

static void
get_property (GObject     * object,
              guint         prop_id,
              GValue      * value,
              GParamSpec  * pspec)
{
        IndicatorObject * self = INDICATOR_OBJECT(object);
        g_return_if_fail(self != NULL);

        IndicatorObjectPrivate * priv = indicator_object_get_instance_private(self);
        g_return_if_fail(priv != NULL);

        switch (prop_id) {
        /* *********************** */
        case PROP_DEFAULT_VISIBILITY:
                if (G_VALUE_HOLDS_BOOLEAN(value)) {
                        g_value_set_boolean(value, priv->default_visibility);
                } else {
                        g_warning("default-visibility property requires a boolean value.");
                }
                break;
        /* *********************** */
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
set_property (GObject       * object,
              guint           prop_id,
              const GValue  * value,
              GParamSpec    * pspec)
{
        IndicatorObject * self = INDICATOR_OBJECT(object);
        g_return_if_fail (self != NULL);

        IndicatorObjectPrivate * priv = indicator_object_get_instance_private(self);
        g_return_if_fail (priv != NULL);


        switch (prop_id) {

        /* *********************** */
        case PROP_DEFAULT_VISIBILITY:
                if (G_VALUE_HOLDS_BOOLEAN(value)) {
                        priv->default_visibility = g_value_get_boolean (value);
                } else {
                        g_warning("default-visibility property requires a boolean value.");
                }
                break;
    
    
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

gboolean
indicator_object_entry_is_visible (IndicatorObject * io, IndicatorObjectEntry * entry)
{
	g_return_val_if_fail (INDICATOR_IS_OBJECT (io), FALSE);

	return entry_get_private (io, entry)->visibility == ENTRY_VISIBLE;
}

gint
indicator_object_get_position (IndicatorObject *io)
{
	g_return_val_if_fail (INDICATOR_IS_OBJECT (io), FALSE);

	return INDICATOR_OBJECT_GET_CLASS (io)->get_position (io);
}
