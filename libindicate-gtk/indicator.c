/*
A library to allow applictions to provide simple indications of
information to be displayed to users of the application through the
interface shell.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of either or both of the following licenses:

1) the GNU Lesser General Public License version 3, as published by the 
Free Software Foundation; and/or
2) the GNU Lesser General Public License version 2.1, as published by 
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the applicable version of the GNU Lesser General Public 
License for more details.

You should have received a copy of both the GNU Lesser General Public 
License version 3 and version 2.1 along with this program.  If not, see 
<http://www.gnu.org/licenses/>
*/

#include "indicator.h"

/**
	indicate_indicator_set_property_icon:
	@indicator: a #IndicateIndicator to act on
	@key: name of the property
	@data: icon to set property with

	This is a helper function that wraps around #indicate_indicator_set_property
	but takes an #GdkPixbuf parameter.  It then takes the @data
	parameter, turns it into a PNG, base64 encodes it and then
	uses that data to call #indicate_indicator_set_property.
*/
void
indicate_indicator_set_property_icon (IndicateIndicator * indicator, const gchar * key, const GdkPixbuf * data)
{
	if (!GDK_IS_PIXBUF(data)) {
		g_warning("Invalide GdkPixbuf");
		return;
	}

	GError * error = NULL;
	gchar * png_data;
	gsize png_data_len;

	if (!gdk_pixbuf_save_to_buffer((GdkPixbuf *)data, &png_data, &png_data_len, "png", &error, NULL)) {
		if (error == NULL) {
			g_warning("Unable to create pixbuf data stream: %d", png_data_len);
		} else {
			g_warning("Unable to create pixbuf data stream: %s", error->message);
			g_error_free(error);
			error = NULL;
		}

		return;
	}

	gchar * prop_str = g_base64_encode((guchar *)png_data, png_data_len);
	indicate_indicator_set_property(indicator, key, prop_str);

	g_free(prop_str);
	g_free(png_data);

	return;
}
