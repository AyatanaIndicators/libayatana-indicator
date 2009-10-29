TESTS =
DISTCLEANFILES =

check_PROGRAMS = \
	test-loader \
	service-shutdown-timeout

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
	$(LIBINDICATOR_LIBS)

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
	$(LIBINDICATOR_LIBS)

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
	$(LIBINDICATOR_LIBS)

libdummy_indicator_simple_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Service Shutdown Timeout
#############################

service_shutdown_timeout_SOURCES = \
	service-shutdown-timeout.c

service_shutdown_timeout_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_shutdown_timeout_LDADD = \
	$(LIBINDICATOR_LIBS) \
	$(top_builddir)/libindicator/.libs/libindicator.a

#############################
# Test stuff
#############################

XML_REPORT  = loader-check-results.xml
HTML_REPORT = loader-check-results.html

loader-tester: test-loader libdummy-indicator-null.la libdummy-indicator-simple.la Makefile
	@echo "#!/bin/sh" > loader-tester
	@echo gtester -k --verbose -o=$(XML_REPORT) ./test-loader >> loader-tester
	@chmod +x loader-tester

TESTS += loader-tester
DISTCLEANFILES += loader-tester

DISTCLEANFILES += $(XML_REPORT) $(HTML_REPORT)

