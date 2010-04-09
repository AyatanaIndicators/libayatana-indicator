
/* Generated data (by glib-mkenums) */

#include "indicator-object-enum-types.h"
/* enumerations from "indicator-object.h" */
#include "indicator-object.h"
GType
indicator_scroll_direction_get_type (void) {
  static GType enum_type_id = 0;
  if (G_UNLIKELY (!enum_type_id))
    {
      static const GEnumValue values[] = {
        { INDICATOR_OBJECT_SCROLL_UP, "INDICATOR_OBJECT_SCROLL_UP", "up" },
        { INDICATOR_OBJECT_SCROLL_DOWN, "INDICATOR_OBJECT_SCROLL_DOWN", "down" },
        { INDICATOR_OBJECT_SCROLL_LEFT, "INDICATOR_OBJECT_SCROLL_LEFT", "left" },
        { INDICATOR_OBJECT_SCROLL_RIGHT, "INDICATOR_OBJECT_SCROLL_RIGHT", "right" },
        { 0, NULL, NULL }
      };
      enum_type_id = g_enum_register_static (g_intern_static_string ("IndicatorScrollDirection"), values);
    }
  return enum_type_id;
}

/* Generated data ends here */

