
#ifndef __LIBINDICATOR_INDICATOR_H_SEEN__
#define __LIBINDICATOR_INDICATOR_H_SEEN__ 1

#include <gtk/gtk.h>

#define INDICATOR_GET_LABEL_S  "get_label"
typedef GtkLabel * (*get_label_t)(void);
GtkLabel * get_label (void);

#define INDICATOR_GET_ICON_S   "get_icon"
typedef GtkImage * (*get_icon_t)  (void);
GtkImage * get_icon (void);

#define INDICATOR_GET_MENU_S   "get_menu"
typedef GtkMenu * (*get_menu_t) (void);
GtkMenu * get_menu (void);

#define INDICATOR_VERSION "0.2.0"
#define INDICATOR_SET_VERSION  gchar * indicator_version_symbol = INDICATOR_VERSION;
#define INDICATOR_VERSION_S "indicator_version_symbol"
#define INDICATOR_VERSION_CHECK(x)  (!g_strcmp0(x, INDICATOR_VERSION))

#endif /* __LIBINDICATOR_INDICATOR_H_SEEN__ */

