#ifndef __INDICATOR_SERVICE_H__
#define __INDICATOR_SERVICE_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_SERVICE_TYPE            (indicator_service_get_type ())
#define INDICATOR_SERVICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_SERVICE_TYPE, IndicatorService))
#define INDICATOR_SERVICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_SERVICE_TYPE, IndicatorServiceClass))
#define INDICATOR_IS_SERVICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_SERVICE_TYPE))
#define INDICATOR_IS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_SERVICE_TYPE))
#define INDICATOR_SERVICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_SERVICE_TYPE, IndicatorServiceClass))

#define INDICATOR_SERVICE_SIGNAL_SHUTDOWN  "shutdown"

typedef struct _IndicatorService      IndicatorService;
typedef struct _IndicatorServiceClass IndicatorServiceClass;

/**
	IndicatorServiceClass:
	@parent_class: #GObjectClass
	@shutdown: Slot for IndicatorServiceClass::shutdown
	@indicator_service_reserved1: Reserved for future use
	@indicator_service_reserved2: Reserved for future use
	@indicator_service_reserved3: Reserved for future use
	@indicator_service_reserved4: Reserved for future use

*/
struct _IndicatorServiceClass {
	GObjectClass parent_class;
	
	/* Signals */
	void (*shutdown) (IndicatorService * service, gpointer user_data);

	/* Reserved */
	void (*indicator_service_reserved1) (void);
	void (*indicator_service_reserved2) (void);
	void (*indicator_service_reserved3) (void);
	void (*indicator_service_reserved4) (void);
};

/**
	IndicatorService:
	@parent: #GObject

*/
struct _IndicatorService {
	GObject parent;

};

GType indicator_service_get_type (void);

IndicatorService *   indicator_service_new            (gchar * name);
IndicatorService *   indicator_service_new_version    (gchar * name,
                                                       guint version);

G_END_DECLS

#endif
