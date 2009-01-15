
SUBDIRS = tests

INCLUDES= \
	-DG_LOG_DOMAIN=\"libindicate\"

EXTRA_DIST = \
	indicate-interface.xml

BUILT_SOURCES = \
	dbus-indicate-server.h \
	dbus-indicate-client.h

lib_LTLIBRARIES = \
	libindicate.la

libindicateincludedir=$(includedir)/libindicate-1.0/libindicate

indicate_headers = \
	indicator.h \
	listener.h \
	server.h

libindicateinclude_HEADERS = \
	$(indicate_headers)

libindicate_la_SOURCES = \
	$(indicate_headers) \
	dbus-indicate-server.h \
	dbus-indicate-client.h \
	server.c \
	listener.c \
	indicator.c

libindicate_la_LDFLAGS = \
	-version-info $(LIBINDICATE_CURRENT):$(LIBINDICATE_REVISION):$(LIBINDICATE_AGE) \
	-no-undefined \
	-export-symbols-regex "^[^_].*"

libindicate_la_CFLAGS = \
	$(LIBINDICATE_CFLAGS)

libindicate_la_LIBADD = \
	$(LIBINDICATE_LIBS)

dbus-indicate-server.h: indicate-interface.xml
	libtool --mode=execute dbus-binding-tool \
		--prefix=indicate_server \
		--mode=glib-server \
		--output=dbus-indicate-server.h \
		$(srcdir)/indicate-interface.xml

dbus-indicate-client.h: indicate-interface.xml
	libtool --mode=execute dbus-binding-tool \
		--prefix=indicate_client \
		--mode=glib-client \
		--output=dbus-indicate-client.h \
		$(srcdir)/indicate-interface.xml

pkgconfig_DATA = indicate.pc
pkgconfigdir = $(libdir)/pkgconfig
