#ifndef __INDICATOR_SERVICE_MANAGER_H__
#define __INDICATOR_SERVICE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_SERVICE_MANAGER_TYPE            (indicator_service_manager_get_type ())
#define INDICATOR_SERVICE_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_SERVICE_MANAGER_TYPE, IndicatorServiceManager))
#define INDICATOR_SERVICE_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_SERVICE_MANAGER_TYPE, IndicatorServiceManagerClass))
#define INDICATOR_IS_SERVICE_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_SERVICE_MANAGER_TYPE))
#define INDICATOR_IS_SERVICE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_SERVICE_MANAGER_TYPE))
#define INDICATOR_SERVICE_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_SERVICE_MANAGER_TYPE, IndicatorServiceManagerClass))

#define INDICATOR_SERVICE_MANAGER_SIGNAL_CONNECTION_CHANGE   "connection-change"

typedef struct _IndicatorServiceManager      IndicatorServiceManager;
typedef struct _IndicatorServiceManagerClass IndicatorServiceManagerClass;

/**
	IndicatorServiceManagerClass:
	@parent: #GObjectClass
	@connection_changed: Slot for #IndicatorServiceManager::connection-changed.
	@indicator_service_manager_reserved1: Reserved for future use.
	@indicator_service_manager_reserved2: Reserved for future use.
	@indicator_service_manager_reserved3: Reserved for future use.
	@indicator_service_manager_reserved4: Reserved for future use.

*/
struct _IndicatorServiceManagerClass {
	GObjectClass parent_class;

	/* Signals */
	void (*connection_change) (IndicatorServiceManager * sm, gboolean connected, gpointer user_data);

	/* Buffer */
	void (*indicator_service_manager_reserved1) (void);
	void (*indicator_service_manager_reserved2) (void);
	void (*indicator_service_manager_reserved3) (void);
	void (*indicator_service_manager_reserved4) (void);
};

/**
	IndicatorServiceManager:
	@parent: #GObject

*/
struct _IndicatorServiceManager {
	GObject parent;

};

GType indicator_service_manager_get_type (void);

IndicatorServiceManager *   indicator_service_manager_new         (gchar * dbus_name);
IndicatorServiceManager *   indicator_service_manager_new_version (gchar * dbus_name,
                                                                   guint version);
gboolean                    indicator_service_manager_connected   (IndicatorServiceManager * sm);
void                        indicator_service_manager_set_refresh (IndicatorServiceManager * sm,
                                                                   guint time_in_ms);

G_END_DECLS

#endif
