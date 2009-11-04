
#include "libindicator/indicator.h"

INDICATOR_SET_VERSION
INDICATOR_SET_NAME("dummy-indicator-null")

GtkLabel *
get_label (void)
{
	return NULL;
}

GtkImage *
get_icon (void)
{
	return NULL;
}

GtkMenu *
get_menu (void)
{
	return NULL;
}
