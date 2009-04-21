
#ifndef __LIBINDICATOR_INDICATOR_H_SEEN__
#define __LIBINDICATOR_INDICATOR_H_SEEN__ 1

#include <gtk/gtk.h>

#define INDICATOR_GET_LABEL_S  "get_label"
GtkLabel * get_label (void);

#define INDICATOR_GET_ICON_S   "get_icon"
GtkImage * get_icon  (void);

#define INDICATOR_GET_MENU_S   "get_menu"
GtkMenu * get_menu (void);

#endif /* __LIBINDICATOR_INDICATOR_H_SEEN__ */

