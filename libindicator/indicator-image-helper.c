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

static void
refresh_image (GtkImage * image)
{
	g_return_if_fail(GTK_IS_IMAGE(image));

	GIcon * icon_names = (GIcon *)g_object_get_data(G_OBJECT(image), INDICATOR_NAMES_DATA);
	g_return_if_fail(icon_names != NULL);

	/* Get the default theme */
	GtkIconTheme * default_theme = gtk_icon_theme_get_default();
	g_return_if_fail(default_theme != NULL);

	gint icon_size = 22;

	GtkStyle * style = gtk_widget_get_style(GTK_WIDGET(image));
	GValue styleprop = {0};
	gtk_style_get_style_property(style, GTK_TYPE_IMAGE, "x-ayatana-indicator-dynamic", &styleprop);

	if (G_VALUE_HOLDS_BOOLEAN(&styleprop) && g_value_get_boolean(&styleprop)) {
		PangoContext * context = gtk_widget_get_pango_context(GTK_WIDGET(image));
		PangoFontMetrics * metrics = pango_context_get_metrics(context, style->font_desc, pango_context_get_language(context));
		icon_size = PANGO_PIXELS(pango_font_metrics_get_ascent(metrics)) + PANGO_PIXELS(pango_font_metrics_get_descent(metrics));
		g_debug("Looking for icon size %d", icon_size);
		pango_font_metrics_unref(metrics);
	}

	/* Look through the themes for that icon */
	GtkIconInfo * icon_info = gtk_icon_theme_lookup_by_gicon(default_theme, icon_names, icon_size, 0);
	if (icon_info == NULL) {
		g_warning("Unable to find icon in theme.");
		return;
	}

	/* Grab the filename */
	const gchar * icon_filename = gtk_icon_info_get_filename(icon_info);
	g_return_if_fail(icon_filename != NULL); /* An error because we shouldn't get info without a filename */

	/* Build a pixbuf */
	GError * error = NULL;
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(icon_filename, &error);
	gtk_icon_info_free(icon_info);

	if (pixbuf == NULL) {
		g_error("Unable to load icon from file '%s' because: %s", icon_filename, error == NULL ? "I don't know" : error->message);
		return;
	}

	/* Scale icon if all we get is something too big. */
	if (gdk_pixbuf_get_height(pixbuf) > icon_size) {
		gfloat scale = (gfloat)icon_size / (gfloat)gdk_pixbuf_get_height(pixbuf);
		gint width = round(gdk_pixbuf_get_width(pixbuf) * scale);

		GdkPixbuf * scaled = gdk_pixbuf_scale_simple(pixbuf, width, icon_size, GDK_INTERP_BILINEAR);
		g_object_unref(G_OBJECT(pixbuf));
		pixbuf = scaled;
	}

	/* Put the pixbuf on the image */
	gtk_image_set_from_pixbuf(image, pixbuf);
	g_object_unref(G_OBJECT(pixbuf));

	return;
}

/* Handles the theme changed signal to refresh the icon to make
   sure that it changes appropriately */
static void
theme_changed_cb (GtkIconTheme * theme, gpointer user_data)
{
	GtkImage * image = GTK_IMAGE(user_data);
	refresh_image(image);
	return;
}

/* Removes the signal on the theme that was calling update on this
   image. */
static void
image_destroyed_cb (GtkImage * image, gpointer user_data)
{
	g_signal_handlers_disconnect_by_func(gtk_icon_theme_get_default(), theme_changed_cb, image);
	return;
}

GtkImage *
indicator_image_helper (const gchar * name)
{
	g_return_val_if_fail(name != NULL, NULL);
	g_return_val_if_fail(name[0] != '\0', NULL);

	/* Build us a GIcon */
	GIcon * icon_names = g_themed_icon_new_with_default_fallbacks(name);
	g_return_val_if_fail(icon_names != NULL, NULL);

	/* Build us an image */
	GtkImage * image = GTK_IMAGE(gtk_image_new());

	if (image == NULL) {
		g_error("Unable to create image from pixbuf on icon name '%s'", name);
		g_object_unref(icon_names);
		return NULL;
	}

	/* Attach our names to the image */
	g_object_set_data_full(G_OBJECT(image), INDICATOR_NAMES_DATA, icon_names, g_object_unref);

	/* Put the pixbuf in */
	refresh_image(image);

	/* Connect to all changes */
	g_signal_connect(G_OBJECT(gtk_icon_theme_get_default()), "changed", G_CALLBACK(theme_changed_cb), image);
	g_signal_connect(G_OBJECT(image), "destroy", G_CALLBACK(image_destroyed_cb), NULL);

	/* Return our built image */
	return image;
}