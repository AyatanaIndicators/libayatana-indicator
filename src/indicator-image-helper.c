/*
A little helper to make a themed image with fallbacks that
is only constrained in the vertical dimention.

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

#include <math.h>
#include "indicator-image-helper.h"

const gchar * INDICATOR_NAMES_DATA = "indicator-names-data";
const gint ICON_SIZE = 22;

static void
refresh_image (GtkImage * image)
{
	g_return_if_fail(GTK_IS_IMAGE(image));
	const gchar * icon_filename = NULL;
	GtkIconInfo * icon_info = NULL;

	GIcon * icon_names = (GIcon *)g_object_get_data(G_OBJECT(image), INDICATOR_NAMES_DATA);
	g_return_if_fail(G_IS_ICON (icon_names));

	/* Get the default theme */
	GtkIconTheme * default_theme = gtk_icon_theme_get_default();
	g_return_if_fail(default_theme != NULL);

	/* Look through the themes for that icon */
	icon_info = gtk_icon_theme_lookup_by_gicon(default_theme, icon_names, ICON_SIZE, 0);
	if (icon_info == NULL) {
		/* Maybe the icon was just added to the theme, see if a rescan helps */
		gtk_icon_theme_rescan_if_needed(default_theme);
		icon_info = gtk_icon_theme_lookup_by_gicon(default_theme, icon_names, ICON_SIZE, 0);
	}
	if (icon_info == NULL) {
		/* Try using the second item in the names, which should be the original filename supplied */
		const gchar * const * names = g_themed_icon_get_names(G_THEMED_ICON( icon_names ));
		if (names) {
			icon_filename = names[1];
		} else {
			g_warning("Unable to find icon\n");
			gtk_image_clear(image);
			return;
		}
	} else {
		/* Grab the filename */
		icon_filename = gtk_icon_info_get_filename(icon_info);
	}

	if (icon_filename == NULL && !G_IS_BYTES_ICON(icon_names)) {
		/* show a broken image if we don't have a filename or image data */
		gtk_image_set_from_icon_name(image, "image-missing", GTK_ICON_SIZE_LARGE_TOOLBAR);
		return;
	}

	if (icon_info != NULL && !G_IS_BYTES_ICON(icon_names)) {
		GdkPixbuf *pixbuf = gtk_icon_info_load_icon(icon_info, NULL);

		if (gdk_pixbuf_get_height(pixbuf) < ICON_SIZE) {
			gtk_image_set_from_file(image, icon_filename);
		} else {
			gtk_image_set_from_gicon(image, icon_names, GTK_ICON_SIZE_LARGE_TOOLBAR);
		}
		g_object_unref (pixbuf);
	} else if (icon_filename != NULL) {
		GError* error = NULL;
		GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_scale(icon_filename, ICON_SIZE, ICON_SIZE, TRUE, &error);
		if (pixbuf != NULL) {
			/* Put the pixbuf on the image */
			gtk_image_set_from_pixbuf(image, pixbuf);
			g_object_unref(G_OBJECT(pixbuf));
		} else {
			g_error_free(error);
			gtk_image_set_from_icon_name(image, "image-missing", ICON_SIZE);
		}
	} else if (G_IS_LOADABLE_ICON(icon_names)) {
		/* Build a pixbuf if needed */
		GdkPixbuf * pixbuf = NULL;
		GError * error = NULL;
		GInputStream * stream = g_loadable_icon_load(G_LOADABLE_ICON(icon_names), ICON_SIZE, NULL, NULL, &error);

		if (stream != NULL) {
			pixbuf = gdk_pixbuf_new_from_stream(stream, NULL, &error);
			g_input_stream_close (stream, NULL, NULL);
			g_object_unref (stream);

			if (pixbuf != NULL) {
				/* Scale icon if all we get is something too big. */
				if (gdk_pixbuf_get_height(pixbuf) > ICON_SIZE) {
					gfloat scale = (gfloat)ICON_SIZE / (gfloat)gdk_pixbuf_get_height(pixbuf);
					gint width = round(gdk_pixbuf_get_width(pixbuf) * scale);

					GdkPixbuf * scaled = gdk_pixbuf_scale_simple(pixbuf, width, ICON_SIZE, GDK_INTERP_BILINEAR);
					g_object_unref(G_OBJECT(pixbuf));
					pixbuf = scaled;
				}

				/* Put the pixbuf on the image */
				gtk_image_set_from_pixbuf(image, pixbuf);
				g_object_unref(G_OBJECT(pixbuf));
			} else {
				g_warning ("Unable to load icon from data: %s", error->message);
				g_error_free (error);
			}
		} else {
			g_warning ("Unable to load icon from data: %s", error->message);
			g_error_free (error);
		}
	}

	if (icon_info != NULL) {
#if GTK_CHECK_VERSION(3, 8, 0)
		g_object_unref(icon_info);
#else
		/* NOTE: Leaving this in for lower version as it seems
		   the object_unref() doesn't work on earlier versions. */
		gtk_icon_info_free (icon_info);
#endif
	}
}

/* Handles the theme changed signal to refresh the icon to make
   sure that it changes appropriately */
static void
theme_changed_cb (__attribute__((unused)) GtkIconTheme * theme, gpointer user_data)
{
	GtkImage * image = GTK_IMAGE(user_data);
	refresh_image(image);
	return;
}

/* Removes the signal on the theme that was calling update on this
   image. */
static void
image_destroyed_cb (GtkImage * image, __attribute__((unused)) gpointer user_data)
{
	g_signal_handlers_disconnect_by_func(gtk_icon_theme_get_default(), theme_changed_cb, image);
	return;
}

/* Catch the style changing on the image to make sure
   we've got the latest. */
static void
image_style_change_cb (GtkImage * image, __attribute__((unused)) GtkStyle * previous_style, __attribute__((unused)) gpointer user_data)
{
	refresh_image(image);
	return;
}

/* Builds an image with the name and fallbacks and all kinds of fun
   stuff . */
GtkImage *
indicator_image_helper (const gchar * name)
{
	/* Build us an image */
	GtkImage * image = GTK_IMAGE(gtk_image_new());

	if (name)
		indicator_image_helper_update(image, name);

	return image;
}

/* Updates and image with all the fun stuff */
void
indicator_image_helper_update (GtkImage * image, const gchar * name)
{
	g_return_if_fail(name != NULL);
	g_return_if_fail(name[0] != '\0');
	g_return_if_fail(GTK_IS_IMAGE(image));

	/* Build us a GIcon */
	GIcon * icon_names = g_themed_icon_new_with_default_fallbacks(name);
	g_warn_if_fail(icon_names != NULL);
	g_return_if_fail(icon_names != NULL);

	indicator_image_helper_update_from_gicon (image, icon_names);

	g_object_unref (icon_names);
	return;
}

void
indicator_image_helper_update_from_gicon (GtkImage *image, GIcon *icon)
{
	gboolean seen_previously = FALSE;

	seen_previously = (g_object_get_data(G_OBJECT(image), INDICATOR_NAMES_DATA) != NULL);

	/* Attach our names to the image */
	g_object_set_data_full(G_OBJECT(image), INDICATOR_NAMES_DATA, g_object_ref (icon), g_object_unref);

	/* Put the pixbuf in */
	refresh_image(image);

	/* Connect to all changes */
	if (!seen_previously) {
		g_signal_connect(G_OBJECT(gtk_icon_theme_get_default()), "changed", G_CALLBACK(theme_changed_cb), image);
		g_signal_connect(G_OBJECT(image), "destroy", G_CALLBACK(image_destroyed_cb), NULL);
		g_signal_connect(G_OBJECT(image), "style-set", G_CALLBACK(image_style_change_cb), NULL);
	}

	return;
}
