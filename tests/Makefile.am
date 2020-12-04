if USE_GTK3
INDICATOR_LIB = -layatana-indicator3
else
INDICATOR_LIB = -layatana-indicator
GTK_DISABLE_DEPRECATED = -DGTK_DISABLE_DEPRECATED
endif

TESTS =
DISTCLEANFILES =
XFAIL_TESTS =

check_PROGRAMS = 

lib_LTLIBRARIES = \
	libdummy-indicator-blank.la \
	libdummy-indicator-null.la \
	libdummy-indicator-signaler.la \
	libdummy-indicator-simple.la \
	libdummy-indicator-visible.la \
	libdummy-indicator-entry-func.la

DBUS_RUNNER=dbus-test-runner --dbus-config /usr/share/dbus-test-runner/session.conf
XVFB_RUN=". $(srcdir)/run-xvfb.sh"

EXTRA_DIST = \
	run-xvfb.sh \
	session.conf.in \
	service-manager-connect.service.in \
	service-version-bad.service.in \
	service-version-good.service.in \
	org.ayatana.indicator.test.service.in

#############################
# Test Loader
#############################

check_PROGRAMS += test-loader

test_loader_SOURCES = \
	test-loader.c

test_loader_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir) \
	-DBUILD_DIR="\"$(builddir)\""

test_loader_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

#############################
# Test Desktop Shortcuts
#############################

check_PROGRAMS += test-desktop-shortcuts

test_desktop_shortcuts_SOURCES = \
	test-desktop-shortcuts.c

test_desktop_shortcuts_CFLAGS = \
	-Wall -Werror \
	-DSRCDIR="\"$(srcdir)\"" \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir) \
	-DBUILD_DIR="\"$(abs_builddir)\""

test_desktop_shortcuts_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

DS_XML_REPORT  = desktop-shortcuts-check-results.xml
DS_HTML_REPORT = desktop-shortcuts-check-results.html

test-desktop-shortcuts-tester: test-desktop-shortcuts Makefile.am
	@echo "#!/bin/bash" > $@
	@echo $(XVFB_RUN) >> $@
	@echo gtester -k --verbose -o=$(XML_REPORT) ./test-desktop-shortcuts >> $@
	@chmod +x $@

TESTS += test-desktop-shortcuts-tester
DISTCLEANFILES += test-desktop-shortcuts-tester \
	test-desktop-shortcuts-touch-test \
	$(DS_XML_REPORT) \
	$(DS_HTML_REPORT)
EXTRA_DIST += test-well-formed.desktop

#############################
# Dummy Indicator Blank
#############################

libdummy_indicator_blank_la_SOURCES = \
	dummy-indicator-blank.c

libdummy_indicator_blank_la_CFLAGS = \
	-Wall -Werror \
	$(GTK_DISABLE_DEPRECATED) \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_blank_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

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
	$(GTK_DISABLE_DEPRECATED) \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_null_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

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
	$(GTK_DISABLE_DEPRECATED) \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_signaler_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

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
	$(GTK_DISABLE_DEPRECATED) \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_simple_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

libdummy_indicator_simple_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Dummy Indicator Entry Func
#############################

libdummy_indicator_entry_func_la_SOURCES = \
	dummy-indicator-entry-func.c \
	dummy-indicator-entry-func.h

libdummy_indicator_entry_func_la_CFLAGS = \
	-Wall -Werror \
	$(GTK_DISABLE_DEPRECATED) \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_entry_func_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

libdummy_indicator_entry_func_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Dummy Indicator Visible
#############################

libdummy_indicator_visible_la_SOURCES = \
	dummy-indicator-visible.c

libdummy_indicator_visible_la_CFLAGS = \
	-Wall -Werror \
	$(GTK_DISABLE_DEPRECATED) \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

libdummy_indicator_visible_la_LIBADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

libdummy_indicator_visible_la_LDFLAGS = \
	-module \
	-avoid-version

#############################
# Service Shutdown Timeout
#############################

check_PROGRAMS += service-shutdown-timeout

service_shutdown_timeout_SOURCES = \
	service-shutdown-timeout.c

service_shutdown_timeout_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_shutdown_timeout_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

service-shutdown-timeout-tester: service-shutdown-timeout Makefile
	@echo "#!/bin/sh" > service-shutdown-timeout-tester
	@echo $(DBUS_RUNNER) --task ./service-shutdown-timeout >> service-shutdown-timeout-tester
	@chmod +x service-shutdown-timeout-tester

TESTS += service-shutdown-timeout-tester
DISTCLEANFILES += service-shutdown-timeout-tester

#############################
# Service Manager No Connect
#############################

check_PROGRAMS += service-manager-no-connect

service_manager_no_connect_SOURCES = \
	service-manager-no-connect.c

service_manager_no_connect_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_no_connect_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

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
	sed -e "s|\@servicedir\@|$(abs_builddir)|" $< > $@

service-manager-connect.service: $(srcdir)/service-manager-connect.service.in Makefile.am
	sed -e "s|\@builddir\@|$(abs_builddir)|" $< > $@

check_PROGRAMS += service-manager-connect

service_manager_connect_SOURCES = \
	service-manager-connect.c

service_manager_connect_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_connect_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

check_PROGRAMS += service-manager-connect-service

service_manager_connect_service_SOURCES = \
	service-manager-connect-service.c

service_manager_connect_service_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_connect_service_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

service-manager-connect-tester: service-manager-connect service-manager-connect-service session.conf service-manager-connect.service Makefile.am
	@echo "#!/bin/sh" > service-manager-connect-tester
	@echo dbus-test-runner --dbus-config $(builddir)/session.conf --task ./service-manager-connect >> service-manager-connect-tester
	@chmod +x service-manager-connect-tester

TESTS += service-manager-connect-tester
DISTCLEANFILES += service-manager-connect-tester session.conf service-manager-connect.service

#############################
# Service Versions
#############################

service-version-good.service: $(srcdir)/service-version-good.service.in Makefile.am
	sed -e "s|\@builddir\@|$(abs_builddir)|" $< > $@

service-version-bad.service: $(srcdir)/service-version-bad.service.in Makefile.am
	sed -e "s|\@builddir\@|$(abs_builddir)|" $< > $@

check_PROGRAMS += service-version-manager

service_version_manager_SOURCES = \
	service-version-values.h \
	service-version-manager.c

service_version_manager_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_version_manager_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

check_PROGRAMS += service-version-bad-service

service_version_bad_service_SOURCES = \
	service-version-values.h \
	service-version-bad-service.c

service_version_bad_service_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_version_bad_service_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

check_PROGRAMS += service-version-good-service

service_version_good_service_SOURCES = \
	service-version-values.h \
	service-version-good-service.c

service_version_good_service_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_version_good_service_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

service-version-tester: service-version-manager service-version-bad-service service-version-good-service session.conf service-version-bad.service service-version-good.service Makefile.am
	@echo "#!/bin/sh" > $@
	@echo dbus-test-runner --dbus-config $(builddir)/session.conf --task ./service-version-manager >> $@
	@chmod +x $@

TESTS += service-version-tester
DISTCLEANFILES += service-version-tester service-version-bad.service service-version-good.service

#############################
# Service Versions
#############################

check_PROGRAMS += service-version-multiwatch-manager

service_version_multiwatch_manager_SOURCES = \
	service-version-values.h \
	service-version-multiwatch-manager.c

service_version_multiwatch_manager_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_version_multiwatch_manager_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

check_PROGRAMS += service-version-multiwatch-manager-impolite

service_version_multiwatch_manager_impolite_SOURCES = \
	service-version-values.h \
	service-version-multiwatch-manager-impolite.c

service_version_multiwatch_manager_impolite_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_version_multiwatch_manager_impolite_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

check_PROGRAMS += service-version-multiwatch-service

service_version_multiwatch_service_SOURCES = \
	service-version-values.h \
	service-version-multiwatch-service.c

service_version_multiwatch_service_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_version_multiwatch_service_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

service-version-multiwatch-tester: service-version-multiwatch-manager service-version-multiwatch-service service-version-multiwatch-manager-impolite Makefile.am
	@echo "#!/bin/sh" > $@
	@echo dbus-test-runner --task ./service-version-multiwatch-service --task-name Service --task ./service-version-multiwatch-manager --task-name Manager1 --task ./service-version-multiwatch-manager --task-name Manager2 --task ./service-version-multiwatch-manager --task-name Manager3 --task ./service-version-multiwatch-manager --task-name Manager4 --task ./service-version-multiwatch-manager --task-name Manager5 --task ./service-version-multiwatch-manager-impolite --task-name Impolite1 --task ./service-version-multiwatch-manager-impolite --task-name Impolite2 --task ./service-version-multiwatch-manager-impolite --task-name Impolite3 >> $@
	@chmod +x $@

TESTS += service-version-multiwatch-tester
DISTCLEANFILES += service-version-multiwatch-tester

#############################
# Service Manager Shutdown
#############################

check_PROGRAMS += service-manager-nostart-connect

service_manager_nostart_connect_SOURCES = \
	service-manager-nostart-connect.c

service_manager_nostart_connect_CFLAGS = \
	-Wall -Werror \
	$(LIBINDICATOR_CFLAGS) -I$(top_srcdir)

service_manager_nostart_connect_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)

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

loader-tester: test-loader libdummy-indicator-null.la libdummy-indicator-simple.la libdummy-indicator-entry-func.la Makefile
	@echo "#!/bin/bash" > loader-tester
	@echo $(XVFB_RUN) >> $@
	@echo gtester -k --verbose -o=$(XML_REPORT) ./test-loader >> loader-tester
	@chmod +x loader-tester

TESTS += loader-tester
DISTCLEANFILES += loader-tester

DISTCLEANFILES += $(XML_REPORT) $(HTML_REPORT)

#############################
# Indicator-ng
#############################

if USE_GTK3

org.ayatana.indicator.test.service: org.ayatana.indicator.test.service.in Makefile.am
	sed -e "s|\@builddir\@|$(abs_builddir)|" $< > $@

check_PROGRAMS += test-indicator-ng

test_indicator_ng_SOURCES = test-indicator-ng.c

test_indicator_ng_CFLAGS = \
	$(LIBINDICATOR_CFLAGS) \
	-I$(top_srcdir) \
	-DSRCDIR="\"$(srcdir)\"" \
	-DBUILD_DIR="\"$(abs_builddir)\"" \
	-Wall

test_indicator_ng_LDADD = \
	$(LIBINDICATOR_LIBS) \
	-L$(top_builddir)/libayatana-indicator/.libs \
	$(INDICATOR_LIB)


test-indicator-ng-dbus-runner: test-indicator-ng Makefile.am
	@echo "#!/bin/sh" > $@
	@echo $(DBUS_RUNNER) --task ./test-indicator-ng >> $@
	@chmod +x $@

test-indicator-ng-tester: test-indicator-ng-dbus-runner Makefile.am
	@echo "#!/bin/bash" > $@
	@echo $(XVFB_RUN) >> $@
	@echo gtester -k --verbose -o=$(XML_REPORT) ./test-indicator-ng-dbus-runner >> $@
	@chmod +x $@

TESTS += test-indicator-ng-tester
DISTCLEANFILES += test-indicator-ng-tester test-indicator-ng-dbus-runner test-indicator-ng

# Mock indicator service
check_PROGRAMS += indicator-test-service

indicator_test_service_SOURCES = indicator-test-service.c
indicator_test_service_CFLAGS = $(LIBINDICATOR_CFLAGS)
indicator_test_service_LDADD = $(LIBINDICATOR_LIBS)

EXTRA_indicator_test_service_DEPENDENCIES = org.ayatana.indicator.test.service

DISTCLEANFILES += indicator-test-service org.ayatana.indicator.test.service

endif
