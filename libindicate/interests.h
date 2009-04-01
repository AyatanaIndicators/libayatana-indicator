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

#ifndef INDICATE_INTERESTS_H_INCLUDED__
#define INDICATE_INTERESTS_H_INCLUDED__ 1

#include <glib.h>

G_BEGIN_DECLS

typedef enum _IndicateInterests IndicateInterests;
enum _IndicateInterests {
	INDICATE_INTEREST_NONE,              /**< We're of no interest */
	INDICATE_INTEREST_SERVER_DISPLAY,    /**< Displays the server's existance to the user */
	INDICATE_INTEREST_SERVER_SIGNAL,     /**< Will send signals to the server to be displayed */
	INDICATE_INTEREST_INDICATOR_DISPLAY, /**< Displays indicators to the user */
	INDICATE_INTEREST_INDICATOR_SIGNAL,  /**< Will return signals based on individual indicators being responded to */
	INDICATE_INTEREST_INDICATOR_COUNT,   /**< Only displays a count of the indicators */
	INDICATE_INTEREST_INDICATOR_LAST     /**< Makes merges and counting easier */
};

G_END_DECLS

#endif /* INDICATE_INTERESTS_H_INCLUDED__ */

