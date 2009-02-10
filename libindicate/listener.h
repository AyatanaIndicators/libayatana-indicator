
#ifndef INDICATE_LISTENER_H_INCLUDED__
#define INDICATE_LISTENER_H_INCLUDED__ 1

#include <glib.h>
#include <glib-object.h>

#include "indicator.h"
#include "server.h"

G_BEGIN_DECLS

/* Boilerplate */
#define INDICATE_TYPE_LISTENER (indicate_listener_get_type ())
#define INDICATE_LISTENER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_LISTENER, IndicateListener))
#define INDICATE_IS_LISTENER(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_LISTENER))
#define INDICATE_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), INDICATE_TYPE_LISTENER, IndicateListenerClass))
#define INDICATE_IS_LISTENER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), INDICATE_TYPE_LISTENER))
#define INDICATE_LISTENER_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), INDICATE_TYPE_LISTENER, IndicateListenerClass))

#define INDICATE_LISTENER_SIGNAL_INDICATOR_ADDED       "indicator-added"
#define INDICATE_LISTENER_SIGNAL_INDICATOR_REMOVED     "indicator-removed"
#define INDICATE_LISTENER_SIGNAL_INDICATOR_MODIFIED    "indicator-modified"
#define INDICATE_LISTENER_SIGNAL_SERVER_ADDED          "server-added"
#define INDICATE_LISTENER_SIGNAL_SERVER_REMOVED        "server-removed"

#define INDICATE_LISTENER_SERVER_DBUS_NAME(server)   ((gchar *)server)
#define INDICATE_LISTENER_INDICATOR_ID(indicator)    (GPOINTER_TO_UINT(indicator))

typedef gchar IndicateListenerServer;
typedef guint IndicateListenerIndicator;

typedef struct _IndicateListener IndicateListener;
struct _IndicateListener {
	GObject parent;
};

typedef struct _IndicateListenerClass IndicateListenerClass;
struct _IndicateListenerClass {
	GObjectClass parent;

	/* Signals */
	void (* indicator_added) (IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type);
	void (* indicator_removed) (IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type);
	void (* indicator_modified) (IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * type, gchar * property);

	void (* server_added) (IndicateListenerServer * server, gchar * type);
	void (* server_removed) (IndicateListenerServer * server, gchar * type);
};

GType indicate_listener_get_type (void) G_GNUC_CONST;

typedef void (*indicate_listener_get_property_cb) (IndicateListener * listener, IndicateListenerServer * server, IndicateListenerIndicator * indicator, gchar * property, gchar * propertydata, gpointer data);
typedef void (*indicate_listener_get_type_cb) (IndicateListener * listener, IndicateListenerServer * server, gchar * type, gpointer data);
typedef void (*indicate_listener_get_desktop_cb) (IndicateListener * listener, IndicateListenerServer * server, gchar * desktop, gpointer data);

/* Create a new listener */
IndicateListener *    indicate_listener_new            (void);
void                  indicate_listener_get_property   (IndicateListener * listener,
                                                        IndicateListenerServer * server,
                                                        IndicateListenerIndicator * indicator,
                                                        gchar * property,
                                                        indicate_listener_get_property_cb callback,
                                                        gpointer data);
void                  indicate_listener_display        (IndicateListener * listener,
                                                        IndicateListenerServer * server,
                                                        IndicateListenerIndicator * indicator);
void                  indicate_listener_get_type       (IndicateListener * listener,
                                                        IndicateListenerServer * server,
                                                        indicate_listener_get_type_cb callback,
                                                        gpointer data);
void                  indicate_listener_get_desktop    (IndicateListener * listener,
                                                        IndicateListenerServer * server,
                                                        indicate_listener_get_desktop_cb callback,
                                                        gpointer data);





G_END_DECLS

#endif /* INDICATE_LISTENER_H_INCLUDED__ */

