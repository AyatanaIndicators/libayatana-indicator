
#include "server.h"
#include "dbus-indicate-server.h"


/* Signals */
enum {
	INDICATOR_ADDED,
	INDICATOR_REMOVED,
	INDICATOR_MODIFIED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (IndicateServer, indicate_server, G_TYPE_OBJECT);

/* Prototypes */
static void indicate_server_finalize (GObject * obj);

/* Code */
static void
indicate_server_class_init (IndicateServerClass * class)
{
	GObjectClass * gobj;
	gobj = G_OBJECT_CLASS(class);

	gobj->finalize = indicate_server_finalize;


	return;
}

static void
indicate_server_init (IndicateServer * server)
{


	return;
}

static void
indicate_server_finalize (GObject * obj)
{


	return;
}
