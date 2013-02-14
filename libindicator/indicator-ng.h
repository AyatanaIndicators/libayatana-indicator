/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *     Lars Uebernickel <lars.uebernickel@canonical.com>
 */

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

GType              indicator_ng_get_type            (void);

IndicatorNg *      indicator_ng_new                 (const gchar  *service_file,
                                                     GError      **error);

IndicatorNg *      indicator_ng_new_for_profile     (const gchar  *service_file,
                                                     const gchar  *profile,
                                                     GError      **error);

const gchar *      indicator_ng_get_service_file    (IndicatorNg *indicator);

const gchar *      indicator_ng_get_profile         (IndicatorNg *indicator);

#endif
