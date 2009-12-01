TESTS =
DISTCLEANFILES =

check_PROGRAMS = \
	test-loader \
	service-manager-no-connect \
	service-manager-connect \
	service-manager-connect-service \
	service-manager-nostart-connect \
	service-shutdown-timeout

lib_LTLIBRARIES = \
	libdummy-indicator-blank.la \
	libdummy-indicator-null.la \
	libdummy-indicator-signaler.la \
	libdummy-indicator-simple.la

DBUS_RUNNER=dbus-test-runner --dbus-config /usr/share/dbus-test-runner/session.conf

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
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libindicator/.libs \
	-lindicator

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
# Dummy Indicator Signaler
#############################

libdummy_indicator_signaler_la_SOURCES = \
	dummy-indicator-signaler.c

libdummy_indicator_signaler_la_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_signaler_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libindicator/.libs \
	-lindicator

libdummy_indicator_signaler_la_LDFLAGS = \
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

service-shutdown-timeout-tester: service-shutdown-timeout Makefile
	@echo "#!/bin/sh" > service-shutdown-timeout-tester
	@echo $(DBUS_RUNNER) --task ./service-shutdown-timeout >> service-shutdown-timeout-tester
	@chmod +x service-shutdown-timeout-tester

TESTS += service-shutdown-timeout-tester
DISTCLEANFILES += service-shutdown-timeout-tester

#############################
# Service Manager No Connect
#############################

service_manager_no_connect_SOURCES = \
	service-manager-no-connect.c

service_manager_no_connect_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_no_connect_LDADD = \
	$(LIBINDICATOR_LIBS) \
	$(top_builddir)/libindicator/.libs/libindicator.a

service-manager-no-connect-tester: service-manager-no-connect Makefile.am
	@echo "#!/bin/sh" > service-manager-no-connect-tester
	@echo $(DBUS_RUNNER) --task ./service-manager-no-connect >> service-manager-no-connect-tester
	@chmod +x service-manager-no-connect-tester

TESTS += service-manager-no-connect-tester
DISTCLEANFILES += service-manager-no-connect-tester

#############################
# Service Manager Connect
#############################

session.conf: $(srcdir)/session.conf.in Makefile.am
	sed -e "s|\@servicedir\@|$(abspath $(builddir))|" $< > $@

service-manager-connect.service: $(srcdir)/service-manager-connect.service.in Makefile.am
	sed -e "s|\@builddir\@|$(abspath $(builddir))|" $< > $@

service_manager_connect_SOURCES = \
	service-manager-connect.c

service_manager_connect_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_connect_LDADD = \
	$(LIBINDICATOR_LIBS) \
	$(top_builddir)/libindicator/.libs/libindicator.a

service_manager_connect_service_SOURCES = \
	service-manager-connect-service.c

service_manager_connect_service_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_connect_service_LDADD = \
	$(LIBINDICATOR_LIBS) \
	$(top_builddir)/libindicator/.libs/libindicator.a

service-manager-connect-tester: service-manager-connect service-manager-connect-service session.conf service-manager-connect.service Makefile.am
	@echo "#!/bin/sh" > service-manager-connect-tester
	@echo dbus-test-runner --dbus-config $(builddir)/session.conf --task ./service-manager-connect >> service-manager-connect-tester
	@chmod +x service-manager-connect-tester

TESTS += service-manager-connect-tester
DISTCLEANFILES += service-manager-connect-tester session.conf service-manager-connect.service

#############################
# Service Manager Shutdown
#############################

service_manager_nostart_connect_SOURCES = \
	service-manager-nostart-connect.c

service_manager_nostart_connect_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_nostart_connect_LDADD = \
	$(LIBINDICATOR_LIBS) \
	$(top_builddir)/libindicator/.libs/libindicator.a

service-manager-connect-nostart-tester: service-manager-nostart-connect service-manager-connect-service Makefile.am
	@echo "#!/bin/sh" > $@
	@echo dbus-test-runner --task ./service-manager-nostart-connect --task ./service-manager-connect-service >> $@
	@chmod +x $@

TESTS += service-manager-connect-nostart-tester
DISTCLEANFILES += service-manager-connect-nostart-tester

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

