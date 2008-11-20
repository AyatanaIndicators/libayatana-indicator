
#ifndef SLIDER_MENU_ITEM_H__
#define SLIDER_MENU_ITEM_H__

#include <gtk/gtk.h>
#include <gtk/gtkmenuitem.h>

G_BEGIN_DECLS

#define TYPE_SLIDER_MENU_ITEM         (slider_menu_item_get_type())
#define SLIDER_MENU_ITEM(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), TYPE_SLIDER_MENU_ITEM), SliderMenuItem)
#define SLIDER_MENU_ITEM_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), TYPE_SLIDER_MENU_ITEM), SliderMenuItemClass)
#define IS_SLIDER_MENU_ITEM(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), TYPE_SLIDER_MENU_ITEM))
#define IS_SLIDER_MENU_ITEM_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), TYPE_SLIDER_MENU_ITEM))
#define SLIDER_MENU_ITEM_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_SLIDER_MENU_ITEM), SliderMenuItemClass)

typedef struct _SliderMenuItem        SliderMenuItem;
typedef struct _SliderMenuItemClass   SliderMenuItemClass;

struct _SliderMenuItem
{
	GtkMenuItem  menu_item;

	GtkWidget *  label;
	GtkWidget *  slider;
	GtkWidget *  button_up;
	GtkWidget *  button_down;
};

struct _SliderMenuItemClass
{
	GtkMenuItemClass parent_class;

	/* Everyone else does this :) */
	void (*_gtk_reserved1) (void);
	void (*_gtk_reserved2) (void);
	void (*_gtk_reserved3) (void);
	void (*_gtk_reserved4) (void);
};


GType             slider_menu_item_get_type      (void) G_GNUC_CONST;
GtkWidget *       slider_menu_item_new           (const gchar * label,
                                                  gdouble min,
                                                  gdouble max,
                                                  gdouble step);
GtkWidget *       slider_menu_get_plus_button    (SliderMenuItem * smi);
GtkWidget *       slider_menu_get_minus_button   (SliderMenuItem * smi);
GtkAdjustment *   slider_menu_get_adjustment     (SliderMenuItem * smi);
gdouble           slider_menu_get_value          (SliderMenuItem * smi);
void              slider_menu_set_value          (SliderMenuItem * smi,
                                                  gdouble value);
void              slider_menu_slider_size_group  (SliderMenuItem * smi,
                                                  GtkSizeGroup   * size_group);

G_END_DECLS

#endif /* GTK_SLIDER_MENU_ITEM_H__ */

