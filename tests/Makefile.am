
check_PROGRAMS = \
	test-loader

lib_LTLIBRARIES = \
	libdummy-indicator-blank.la \
	libdummy-indicator-null.la \
	libdummy-indicator-simple.la

#############################
# Test Loader
#############################

test_loader_SOURCES = \
	test-loader.c

test_loader_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir) \
	-DBUILD_DIR="\"$(builddir)\""

test_loader_LDADD = \
	$(LIBINDICATOR_LIBS) $(top_builddir)/libindicator/.libs/libindicator.a

#############################
# Dummy Indicator Blank
#############################

libdummy_indicator_blank_la_SOURCES = \
	dummy-indicator-blank.c

libdummy_indicator_blank_la_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_blank_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libindicator/.libs \
	-lindicator

libdummy_indicator_blank_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Dummy Indicator NULL
#############################

libdummy_indicator_null_la_SOURCES = \
	dummy-indicator-null.c

libdummy_indicator_null_la_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_null_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libindicator/.libs \
	-lindicator

libdummy_indicator_null_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Dummy Indicator Simple
#############################

libdummy_indicator_simple_la_SOURCES = \
	dummy-indicator-simple.c

libdummy_indicator_simple_la_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_simple_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libindicator/.libs \
	-lindicator

libdummy_indicator_simple_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Test stuff
#############################

XML_REPORT  = loader-check-results.xml
HTML_REPORT = loader-check-results.html

loader-tester: test-loader libdummy-indicator-null.la libdummy-indicator-simple.la
	@gtester -k --verbose -o=$(XML_REPORT) ./test-loader

check-local: loader-tester

DISTCLEANFILES = $(XML_REPORT) $(HTML_REPORT)

