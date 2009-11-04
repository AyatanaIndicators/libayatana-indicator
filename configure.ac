
AC_INIT(libindicator, 0.3.0, ted@canonical.com)

AC_PREREQ(2.53)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(libindicator, 0.3.0)

AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_C_O
AC_STDC_HEADERS
AC_PROG_LIBTOOL

AC_SUBST(VERSION)
AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

##############################
# Dependencies
##############################

GTK_REQUIRED_VERSION=2.18
DBUS_REQUIRED_VERSION=0.76

PKG_CHECK_MODULES(LIBINDICATOR, gtk+-2.0 >= $GTK_REQUIRED_VERSION
                                dbus-glib-1 >= $DBUS_REQUIRED_VERSION)

AC_SUBST(LIBINDICATOR_CFLAGS)
AC_SUBST(LIBINDICATOR_LIBS)

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
libindicator/Makefile
libindicator/indicator.pc
tests/Makefile
tools/Makefile
])

###########################
# Results
###########################

AC_MSG_NOTICE([

Libindicator Configuration:

	Prefix:                 $prefix
])
