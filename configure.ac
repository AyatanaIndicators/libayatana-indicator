
AC_INIT(src/applet-main.c)

AC_PREREQ(2.53)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(indicator-applet, 0.1)

AM_MAINTAINER_MODE

IT_PROG_INTLTOOL([0.35.0])
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_C_O
AC_STDC_HEADERS
AC_PROG_LIBTOOL

AC_SUBST(VERSION)
AC_CONFIG_MACRO_DIR([m4])

###########################
# Libindicate versioning 
###########################

LIBINDICATE_CURRENT=0
LIBINDICATE_REVISION=1
LIBINDICATE_AGE=0

AC_SUBST(LIBINDICATE_CURRENT)
AC_SUBST(LIBINDICATE_REVISION)
AC_SUBST(LIBINDICATE_AGE)

###########################
# Dependencies 
###########################

GTK_REQUIRED_VERSION=2.12
PANEL_REQUIRED_VERSION=2.0.0
DBUS_REQUIRED_VERSION=0.76

PKG_CHECK_MODULES(APPLET, gtk+-2.0 >= $GTK_REQUIRED_VERSION
                          libpanelapplet-2.0 >= $PANEL_REQUIRED_VERSION)
AC_SUBST(APPLET_CFLAGS)
AC_SUBST(APPLET_LIBS)

AC_PATH_PROG(GCONFTOOL, gconftool-2)
AM_GCONF_SOURCE_2

PKG_CHECK_MODULES(LIBINDICATE, gtk+-2.0 >= $GTK_REQUIRED_VERSION
                               dbus-glib-1 >= $DBUS_REQUIRED_VERSION)
AC_SUBST(LIBINDICATE_CFLAGS)
AC_SUBST(LIBINDICATE_LIBS)

###########################
# Internationalization
###########################

GETTEXT_PACKAGE=indicator-applet
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of the default gettext domain])

AM_GLIB_GNU_GETTEXT

###########################
# Files
###########################

AC_OUTPUT([
Makefile
src/Makefile
libindicate/Makefile
libindicate/tests/Makefile
data/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/22x22/Makefile
data/icons/24x24/Makefile
data/icons/32x32/Makefile
data/icons/48x48/Makefile
data/icons/scalable/Makefile
data/icons/16x16/panel/Makefile
data/icons/22x22/panel/Makefile
data/icons/24x24/panel/Makefile
data/icons/32x32/panel/Makefile
data/icons/48x48/panel/Makefile
data/icons/scalable/panel/Makefile
po/Makefile.in
])

###########################
# Results
###########################

AC_MSG_NOTICE([

Indicator Applet Configuration:

	Prefix:        $prefix
])
