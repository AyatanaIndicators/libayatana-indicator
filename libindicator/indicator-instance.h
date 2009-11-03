#ifndef __INDICATOR_INSTANCE_H__
#define __INDICATOR_INSTANCE_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define INDICATOR_INSTANCE_TYPE            (indicator_instance_get_type ())
#define INDICATOR_INSTANCE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_INSTANCE_TYPE, IndicatorInstance))
#define INDICATOR_INSTANCE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_INSTANCE_TYPE, IndicatorInstanceClass))
#define INDICATOR_IS_INSTANCE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_INSTANCE_TYPE))
#define INDICATOR_IS_INSTANCE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_INSTANCE_TYPE))
#define INDICATOR_INSTANCE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_INSTANCE_TYPE, IndicatorInstanceClass))

typedef struct _IndicatorInstance      IndicatorInstance;
typedef struct _IndicatorInstanceClass IndicatorInstanceClass;

struct _IndicatorInstanceClass {
	GObjectClass parent_class;
};

struct _IndicatorInstance {
	GObject parent;
};

GType indicator_instance_get_type (void);

G_END_DECLS

#endif
