
#include "indicator-image-helper.h"

const gchar * INDICATOR_NAMES_DATA = "indicator-names-data";

GtkImage *
indicator_image_helper (const gchar * name)
{
	g_return_val_if_fail(name != NULL, NULL);
	g_return_val_if_fail(name[0] != '\0', NULL);

	/* Get the default theme */
	GtkIconTheme * default_theme = gtk_icon_theme_get_default();
	g_return_val_if_fail(default_theme != NULL, NULL);

	/* Build us a GIcon */
	GIcon * icon_names = g_themed_icon_new_with_default_fallbacks(name);
	g_return_val_if_fail(icon_names != NULL, NULL);

	/* Look through the themes for that icon */
	GtkIconInfo * icon_info = gtk_icon_theme_lookup_by_gicon(default_theme, icon_names, 22, 0);
	if (icon_info == NULL) {
		g_warning("Unable to find icon '%s' in theme.", name);
		g_object_unref(icon_names);
		return NULL;
	}

	/* Grab the filename */
	const gchar * icon_filename = gtk_icon_info_get_filename(icon_info);
	g_return_val_if_fail(icon_filename != NULL, NULL); /* An error because we shouldn't get info without a filename */

	/* Build a pixbuf */
	GError * error = NULL;
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(icon_filename, &error);
	gtk_icon_info_free(icon_info);

	if (pixbuf == NULL) {
		g_error("Unable to load icon from name '%s' file '%s' because: %s", name, icon_filename, error == NULL ? "I don't know" : error->message);
		g_object_unref(icon_names);
		return NULL;
	}

	/* Build us an image */
	GtkImage * image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf));
	g_object_unref(pixbuf);

	if (image == NULL) {
		g_error("Unable to create image from pixbuf on icon name '%s'", name);
		g_object_unref(icon_names);
		return NULL;
	}

	/* Attach our names to the image */
	g_object_set_data_full(G_OBJECT(image), INDICATOR_NAMES_DATA, icon_names, g_object_unref);

	/* Connect to all changes */
	/* TODO */

	/* Return our built image */
	return image;
}
