
AC_INIT(src/applet-main.c)

AC_PREREQ(2.53)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(indicator-applet, 0.1.1)

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

GLIB_REQUIRED_VERSION=2.18
GIO_REQUIRED_VERSION=2.18
GDK_PIXBUF_REQUIRED_VERSION=2.12

PKG_CHECK_MODULES(LIBINDICATE, glib-2.0 >= $GLIB_REQUIRED_VERSION
                               gio-2.0 >= $GIO_REQUIRED_VERSION
                               gdk-pixbuf-2.0 >= $GDK_PIXBUF_REQUIRED_VERSION
                               dbus-glib-1 >= $DBUS_REQUIRED_VERSION)
AC_SUBST(LIBINDICATE_CFLAGS)
AC_SUBST(LIBINDICATE_LIBS)

###########################
# GObject Introspection
###########################

PKG_CHECK_MODULES(GOBJECT_INTROSPECTION, [gobject-introspection-1.0 >= 0.6])

G_IR_SCANNER=`$PKG_CONFIG --variable=g_ir_scanner gobject-introspection-1.0`
AC_SUBST(G_IR_SCANNER)
G_IR_COMPILER=`$PKG_CONFIG --variable=g_ir_compiler gobject-introspection-1.0`
AC_SUBST(G_IR_COMPILER)
G_IR_GENERATE=`$PKG_CONFIG --variable=g_ir_generate gobject-introspection-1.0`
AC_SUBST(G_IR_GENERATE)

GIRDIR=`$PKG_CONFIG --variable=girdir gobject-introspection-1.0`
AC_SUBST(GIRDIR)
TYPELIBDIR="$($PKG_CONFIG --variable=typelibdir gobject-introspection-1.0)"
AC_SUBST(TYPELIBDIR) 

###########################
# GTK Doc 
###########################

GTK_DOC_CHECK([1.4])

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
libindicate/indicate.pc
tests/Makefile
data/Makefile
po/Makefile.in
docs/Makefile
docs/reference/Makefile
docs/reference/version.xml
])

###########################
# Results
###########################

AC_MSG_NOTICE([

Indicator Applet Configuration:

	Prefix:        $prefix
])
