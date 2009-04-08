/*
A library to allow applictions to provide simple indications of
information to be displayed to users of the application through the
interface shell.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of either or both of the following licenses:

1) the GNU Lesser General Public License version 3, as published by the 
Free Software Foundation; and/or
2) the GNU Lesser General Public License version 2.1, as published by 
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the applicable version of the GNU Lesser General Public 
License for more details.

You should have received a copy of both the GNU Lesser General Public 
License version 3 and version 2.1 along with this program.  If not, see 
<http://www.gnu.org/licenses/>
*/

#ifndef INDICATE_INTERESTS_PRIV_H_INCLUDED__
#define INDICATE_INTERESTS_PRIV_H_INCLUDED__ 1

#include <glib.h>

G_BEGIN_DECLS

#define INDICATE_INTEREST_STRING_SERVER_DISPLAY        "server-display"
#define INDICATE_INTEREST_STRING_SERVER_SIGNAL         "server-signal"
#define INDICATE_INTEREST_STRING_INDICATOR_DISPLAY     "indicator-display"
#define INDICATE_INTEREST_STRING_INDICATOR_SIGNAL      "indicator-signal"
#define INDICATE_INTEREST_STRING_INDICATOR_COUNT       "indicator-count"

G_END_DECLS

#endif /* INDICATE_INTERESTS_PRIV_H_INCLUDED__ */

