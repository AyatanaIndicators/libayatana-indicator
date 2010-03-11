
#include <gtk/gtk.h>
#include "indicator-image-helper.h"

int
main (int argv, char * argc[])
{
	gtk_init(&argv, &argc);

	GtkImage * image = indicator_image_helper(argc[1]);

	GdkPixbuf * pixbuf = gtk_image_get_pixbuf(image);

	g_debug("Pixbuf width: %d", gdk_pixbuf_get_width(pixbuf));
	g_debug("Pixbuf height: %d", gdk_pixbuf_get_height(pixbuf));
	
	return;
}
