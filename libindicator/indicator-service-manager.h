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

typedef struct _IndicatorServiceManager      IndicatorServiceManager;
typedef struct _IndicatorServiceManagerClass IndicatorServiceManagerClass;

struct _IndicatorServiceManagerClass {
	GObjectClass parent_class;

};

struct _IndicatorServiceManager {
	GObject parent;
};

GType indicator_service_manager_get_type (void);

G_END_DECLS

#endif
