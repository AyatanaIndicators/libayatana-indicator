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

typedef struct _IndicatorService      IndicatorService;
typedef struct _IndicatorServiceClass IndicatorServiceClass;

struct _IndicatorServiceClass {
	GObjectClass parent_class;

};

struct _IndicatorService {
	GObject parent;

};

GType indicator_service_get_type (void);

G_END_DECLS

#endif
