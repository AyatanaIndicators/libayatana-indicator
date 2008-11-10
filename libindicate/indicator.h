
#ifndef INDICATE_INDICATOR_H_INCLUDED__
#define INDICATE_INDICATOR_H_INCLUDED__ 1

#define INDICATE_TYPE_INDICATOR (indicator_get_type ())
#define INDICATE_INDICATOR(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_INDICATOR, IndicateIndicator))
#define INDICATE_IS_INDICATOR(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_INDICATOR))

#define INDICATE_INDICATOR_SIGNAL_DISPLAY  "user-display"

typedef struct _IndicateIndicator IndicateIndicator;

IndicateIndicator * indicate_indicator_new (void);
void indicate_indicator_set_property (IndicateIndicator * indicator, const gchar * property_name, const gchar * property_value);
void indicate_indicator_show (IndicateIndicator * indicator);

guint indicate_indicator_get_id (IndicateIndicator * indicator);


#endif /* INDICATE_INDICATOR_H_INCLUDED__ */

