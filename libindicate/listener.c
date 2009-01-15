
#include "listener.h"

/* Errors */
enum {
	LAST_ERROR
};

/* Signals */
enum {
	INDICATOR_ADDED,
	INDICATOR_REMOVED,
	INDICATOR_MODIFIED,
	SERVER_ADDED,
	SERVER_REMOVED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (IndicateListener, indicate_listener, G_TYPE_OBJECT);

/* Prototypes */
static void indicate_listener_finalize (GObject * obj);

/* Code */
static void
indicate_listener_class_init (IndicateListenerClass * class)
{
	g_debug("Listener Class Initialized");
	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	gobj->finalize = indicate_listener_finalize;

/* TODO, I need new marshallers here, bah humbug
	signals[INDICATOR_ADDED] = g_signal_new("indicator-added",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateListenerClass, indicator_added),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__POINTER_POINTER_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_OBJECT, G_TYPE_OBJECT, G_TYPE_STRING);
	signals[INDICATOR_REMOVED] = g_signal_new("indicator-removed",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_removed),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
	signals[INDICATOR_MODIFIED] = g_signal_new("indicator-modified",
	                                        G_TYPE_FROM_CLASS (class),
	                                        G_SIGNAL_RUN_LAST,
	                                        G_STRUCT_OFFSET (IndicateServerClass, indicator_modified),
	                                        NULL, NULL,
	                                        g_cclosure_marshal_VOID__UINT_POINTER,
	                                        G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_STRING);
*/

	return;
}

static void
indicate_listener_init (IndicateListener * server)
{
	g_debug("Listener Object Initialized");

	return;
}

static void
indicate_listener_finalize (GObject * obj)
{
	IndicateListener * listener = INDICATE_LISTENER(obj);

	return;
}

