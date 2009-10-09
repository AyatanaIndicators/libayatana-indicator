
#include "libindicator/indicator.h"

INDICATOR_SET_VERSION
INDICATOR_SET_NAME("dummy-indicator-simple")

GtkLabel *
get_label (void)
{
	return GTK_LABEL(gtk_label_new("Simple Item"));
}

GtkImage *
get_icon (void)
{
	return GTK_IMAGE(gtk_image_new());
}

GtkMenu *
get_menu (void)
{
	return GTK_MENU(gtk_menu_new());
}
