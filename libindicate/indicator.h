
#ifndef INDICATE_INDICATOR_H_INCLUDED__
#define INDICATE_INDICATOR_H_INCLUDED__ 1

/* Boilerplate */
#define INDICATE_TYPE_INDICATOR (indicate_indicator_get_type ())
#define INDICATE_INDICATOR(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), INDICATE_TYPE_INDICATOR, IndicateIndicator))
#define INDICATE_IS_INDICATOR(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), INDICATE_TYPE_INDICATOR))

/* This is a signal that signals to the indicator that the user
 * has done an action where they'd like this indicator to be
 * displayed. */
#define INDICATE_INDICATOR_SIGNAL_DISPLAY  "user-display"

typedef struct _IndicateIndicator IndicateIndicator;

IndicateIndicator * indicate_indicator_new (void);

/* Should these just be GObject properties? */
void indicate_indicator_set_property (IndicateIndicator * indicator, const gchar * property_name, const gchar * property_value);

/* Show and hide this indicator */
void indicate_indicator_show (IndicateIndicator * indicator);
void indicate_indicator_hide (IndicateIndicator * indicator);

/* Every entry has an ID, here's how to get it */
guint indicate_indicator_get_id (IndicateIndicator * indicator);

/* Every entry has a type.  This should be created by the
 * subclass and exported through this pretty function */
const gchar * indicate_indicator_get_type (IndicateIndicator * indicator);


#endif /* INDICATE_INDICATOR_H_INCLUDED__ */

