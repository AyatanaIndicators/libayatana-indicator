#ifndef __INDICATOR_NG_H__
#define __INDICATOR_NG_H__

#include "indicator-object.h"

#define INDICATOR_TYPE_NG            (indicator_ng_get_type ())
#define INDICATOR_NG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), INDICATOR_TYPE_NG, IndicatorNg))
#define INDICATOR_NG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), INDICATOR_TYPE_NG, IndicatorNgClass))
#define INDICATOR_IS_NG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INDICATOR_TYPE_NG))
#define INDICATOR_IS_NG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), INDICATOR_TYPE_NG))
#define INDICATOR_NG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), INDICATOR_TYPE_NG, IndicatorNgClass))

typedef struct _IndicatorNg   IndicatorNg;
typedef IndicatorObjectClass IndicatorNgClass;

GType              indicator_ng_get_type     (void);

IndicatorNg *      indicator_ng_new          (const gchar  *service_file,
                                              GError      **error);

#endif
