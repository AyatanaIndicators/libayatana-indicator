#include <glib.h>
#include <glib-object.h>

#include "libindicator/indicator.h"
#include "libindicator/indicator-object.h"

#define DUMMY_INDICATOR_SIGNALER_TYPE            (dummy_indicator_signaler_get_type ())
#define DUMMY_INDICATOR_SIGNALER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUMMY_INDICATOR_SIGNALER_TYPE, DummyIndicatorSignaler))
#define DUMMY_INDICATOR_SIGNALER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUMMY_INDICATOR_SIGNALER_TYPE, DummyIndicatorSignalerClass))
#define IS_DUMMY_INDICATOR_SIGNALER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUMMY_INDICATOR_SIGNALER_TYPE))
#define IS_DUMMY_INDICATOR_SIGNALER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUMMY_INDICATOR_SIGNALER_TYPE))
#define DUMMY_INDICATOR_SIGNALER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUMMY_INDICATOR_SIGNALER_TYPE, DummyIndicatorSignalerClass))

typedef struct _DummyIndicatorSignaler      DummyIndicatorSignaler;
typedef struct _DummyIndicatorSignalerClass DummyIndicatorSignalerClass;

struct _DummyIndicatorSignalerClass {
	IndicatorObjectClass parent_class;
};

struct _DummyIndicatorSignaler {
	IndicatorObject parent;
};

GType dummy_indicator_signaler_get_type (void);

INDICATOR_SET_VERSION
INDICATOR_SET_TYPE(DUMMY_INDICATOR_SIGNALER_TYPE)

GtkLabel *
get_label (IndicatorObject * io)
{
	return GTK_LABEL(gtk_label_new("Signaler Item"));
}

GtkImage *
get_icon (IndicatorObject * io)
{
	return GTK_IMAGE(gtk_image_new());
}

GtkMenu *
get_menu (IndicatorObject * io)
{
	GtkMenu * main_menu = GTK_MENU(gtk_menu_new());
	GtkWidget * loading_item = gtk_menu_item_new_with_label("Loading...");
	gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), loading_item);
	gtk_widget_show(GTK_WIDGET(loading_item));

	return main_menu;
}

static void dummy_indicator_signaler_class_init (DummyIndicatorSignalerClass *klass);
static void dummy_indicator_signaler_init       (DummyIndicatorSignaler *self);
static void dummy_indicator_signaler_dispose    (GObject *object);
static void dummy_indicator_signaler_finalize   (GObject *object);

G_DEFINE_TYPE (DummyIndicatorSignaler, dummy_indicator_signaler, INDICATOR_OBJECT_TYPE);

static void
dummy_indicator_signaler_class_init (DummyIndicatorSignalerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = dummy_indicator_signaler_dispose;
	object_class->finalize = dummy_indicator_signaler_finalize;

	IndicatorObjectClass * io_class = INDICATOR_OBJECT_CLASS(klass);

	io_class->get_label = get_label;
	io_class->get_image = get_icon;
	io_class->get_menu = get_menu;

	return;
}

static gboolean
idle_signal (gpointer data)
{
	DummyIndicatorSignaler * self = DUMMY_INDICATOR_SIGNALER(data);

	g_signal_emit(G_OBJECT(self), INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED_ID, 0, (gpointer)5, TRUE);
	g_signal_emit(G_OBJECT(self), INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED_ID, 0, (gpointer)5, TRUE);

	return FALSE; /* Don't queue again */
}

static void
dummy_indicator_signaler_init (DummyIndicatorSignaler *self)
{
	g_idle_add(idle_signal, self);
	return;
}

static void
dummy_indicator_signaler_dispose (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_signaler_parent_class)->dispose (object);
	return;
}

static void
dummy_indicator_signaler_finalize (GObject *object)
{

	G_OBJECT_CLASS (dummy_indicator_signaler_parent_class)->finalize (object);
	return;
}
