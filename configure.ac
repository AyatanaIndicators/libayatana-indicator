
AC_INIT(src/applet-main.c)

AC_PREREQ(2.53)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(indicator-applet, 0.2.0dev)

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

LIBINDICATE_CURRENT=1
LIBINDICATE_REVISION=0
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
                          libpanelapplet-2.0 >= $PANEL_REQUIRED_VERSION
						  libgnomeui-2.0)
AC_SUBST(APPLET_CFLAGS)
AC_SUBST(APPLET_LIBS)

AC_PATH_PROG(GCONFTOOL, gconftool-2)
AM_GCONF_SOURCE_2

GLIB_REQUIRED_VERSION=2.18
GIO_REQUIRED_VERSION=2.18
GDK_PIXBUF_REQUIRED_VERSION=2.12
XML_REQUIRED_VERSION=2.6

PKG_CHECK_MODULES(LIBINDICATE, glib-2.0 >= $GLIB_REQUIRED_VERSION
                               gio-2.0 >= $GIO_REQUIRED_VERSION
                               gdk-pixbuf-2.0 >= $GDK_PIXBUF_REQUIRED_VERSION
                               dbus-glib-1 >= $DBUS_REQUIRED_VERSION
                               libxml-2.0 >= $XML_REQUIRED_VERSION)
AC_SUBST(LIBINDICATE_CFLAGS)
AC_SUBST(LIBINDICATE_LIBS)

###########################
# GObject Introspection
###########################

AC_ARG_ENABLE(gobject-introspection, AC_HELP_STRING([--enable-gobject-introspection], [enable building the gobject-introspection files]), [enable_gir=$enableval], [enable_gir=yes])

if test "x$enable_gir" = "xyes"; then
	PKG_CHECK_MODULES(GOBJECT_INTROSPECTION, [gobject-introspection-1.0 >= 0.6], gir_ok=yes, gir_ok=no)

	if test "x$gir_ok" = "xyes"; then
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
	fi
else
	gir_ok=no
fi

AM_CONDITIONAL(USE_GIR, test "x$gir_ok" = "xyes")

###########################
# GTK Doc 
###########################

GTK_DOC_CHECK([1.4])
GNOME_DOC_INIT
GNOME_COMPILE_WARNINGS

##############################
# Custom Junk
##############################

AC_DEFUN([AC_DEFINE_PATH], [
  test "x$prefix" = xNONE && prefix="$ac_default_prefix"
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  ac_define_path=`eval echo [$]$2`
  ac_define_path=`eval echo [$]ac_define_path`
  $1="$ac_define_path"
  AC_SUBST($1)
  ifelse($3, ,
    AC_DEFINE_UNQUOTED($1, "$ac_define_path"),
    AC_DEFINE_UNQUOTED($1, "$ac_define_path", $3))
])

###########################
# Internationalization
###########################

GETTEXT_PACKAGE=indicator-applet
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of the default gettext domain])
AC_DEFINE_PATH(GNOMELOCALEDIR, "${datadir}/locale", [locale directory])

AM_GLIB_GNU_GETTEXT

##############################
# ./configure Flags messings
##############################

cflags_set=${CFLAGS+set}
# find the actual value for $prefix that we'll end up with
REAL_PREFIX=
if test "x$prefix" = "xNONE"; then
  REAL_PREFIX=$ac_default_prefix
else
  REAL_PREFIX=$prefix
fi

# Have to go $sysconfdir->$prefix/etc->/usr/local/etc   
# if you actually know how to code shell then fix this :-)
SYSCONFDIR_TMP="$sysconfdir"
old_prefix=$prefix
prefix=$REAL_PREFIX
EXPANDED_SYSCONFDIR=`eval echo $SYSCONFDIR_TMP`
prefix=$old_prefix
AC_SUBST(EXPANDED_SYSCONFDIR)

dnl Save flags to aclocal
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

AC_DEFINE_PATH(PREFIX, "${prefix}", [prefix directory])
AC_DEFINE_PATH(SYSCONFDIR, "${sysconfdir}", [system configuration dir])
AC_DEFINE_PATH(LIBDIR, "${libdir}", [system configuration dir])

###########################
# Files
###########################

AC_OUTPUT([
Makefile
src/Makefile
libindicate/Makefile
libindicate/indicate.pc
libindicator/Makefile
libindicator/indicator.pc
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

	Prefix:                 $prefix
	GObject Introspection:  $gir_ok
])
