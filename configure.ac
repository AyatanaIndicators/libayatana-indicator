AC_INIT([libindicator],
        [0.5.0],
        [http://bugs.launchpad.net/libindicator],
        [libindicator],
        [http://launchpad.net/libindicator])

AC_PREREQ([2.64])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_SUBST([ac_aux_dir])

AM_INIT_AUTOMAKE([1.11 -Wall tar-pax])

AM_MAINTAINER_MODE([enable])

AM_SILENT_RULES([yes])]

# Check for programs
AC_PROG_CC
AM_PROG_CC_C_O

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT

AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])


AC_ARG_ENABLE([deprecations],
  [AS_HELP_STRING([--enable-deprecations],
    [allow deprecated API usage @<:@default=yes@:>@])],
  [],
  [enable_deprecations=yes])
AS_IF([test "x$enable_deprecations" = xno],
  [CFLAGS="$CFLAGS -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -DGTK_DISABLE_SINGLE_INCLUDES"]
)

##############################
# Dependencies
##############################

GTK_REQUIRED_VERSION=2.18
GTK3_REQUIRED_VERSION=2.91
GIO_UNIX_REQUIRED_VERSION=2.22

AC_ARG_WITH([gtk],
  [AS_HELP_STRING([--with-gtk],
    [Which version of gtk to use @<:@default=3@:>@])],
  [],
  [with_gtk=3])
AS_IF([test "x$with_gtk" = x3],
        [PKG_CHECK_MODULES(LIBINDICATOR,  gtk+-3.0 >= $GTK3_REQUIRED_VERSION
                                          gmodule-2.0
                                          gio-unix-2.0 >= $GIO_UNIX_REQUIRED_VERSION)
        ],
      [test "x$with_gtk" = x2],
        [PKG_CHECK_MODULES(LIBINDICATOR,  gtk+-2.0 >= $GTK_REQUIRED_VERSION
                                          gmodule-2.0
                                          gio-unix-2.0 >= $GIO_UNIX_REQUIRED_VERSION)
        ],
      [AC_MSG_FAILURE([Value for --with-gtk was neither 2 nor 3])]
)
AM_CONDITIONAL(USE_GTK3, [test "x$with_gtk" = x3])

LT_LIB_M
LIBINDICATOR_LIBS+="$LIBM"

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

AC_DEFINE_PATH(PREFIX, "${prefix}", [prefix directory])
AC_DEFINE_PATH(SYSCONFDIR, "${sysconfdir}", [system configuration dir])
AC_DEFINE_PATH(LIBDIR, "${libdir}", [system configuration dir])

#########################
# Check if build tests
#########################
AC_ARG_ENABLE([tests],
	AC_HELP_STRING([--disable-tests], [disable tests]),,
	[enable_tests=yes])
AM_CONDITIONAL([WANT_TESTS], [test "x$enable_tests" != "xno"])

#########################
# Debug symbols
#########################
AC_ARG_ENABLE([debug],
              AS_HELP_STRING([--enable-debug],[build with debug symbols]),,
              [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
	CFLAGS="-g $CFLAGS"
	AC_DEFINE(ENABLE_DEBUG, 1, [build with extra debug information])
fi

AM_CONDITIONAL(ENABLE_DEBUG, test "$enable_debug" = "yes")

###########################
# gcov coverage reporting
###########################
m4_include([m4/gcov.m4])
AC_TDD_GCOV
AM_CONDITIONAL([HAVE_GCOV], [test "x$ac_cv_check_gcov" = xyes])
AM_CONDITIONAL([HAVE_LCOV], [test "x$ac_cv_check_lcov" = xyes])
AM_CONDITIONAL([HAVE_GCOVR], [test "x$ac_cv_check_gcovr" = xyes])
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_CXXFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

###########################
# Files
###########################

AC_CONFIG_FILES([
Makefile
libindicator/Makefile
libindicator/indicator-0.4.pc.in
libindicator/indicator3-0.4.pc.in
tests/Makefile
tools/Makefile
])
AC_OUTPUT

###########################
# Results
###########################

AC_MSG_NOTICE([

Libindicator Configuration:

	Prefix:                 $prefix
	GTK+ Version:           $with_gtk
	
	Enable tests:           $enable_tests
	Enable debugging:       $enable_debug
	Coverage reporting:     $use_gcov
])
