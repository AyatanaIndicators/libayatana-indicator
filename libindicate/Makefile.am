
INCLUDES= \
	-DG_LOG_DOMAIN=\"libindicate\"

EXTRA_DIST = \
	indicate-server.xml

BUILT_SOURCES = \
	dbus-indicate-server.h

lib_LTLIBRARIES = \
	libindicate.la

libindicateincludedir=$(includedir)/libindicate-1.0/libindicate

indicate_headers = \
	indicator.h \
	server.h

libindicateinclude_HEADERS = \
	$(indicate_headers)

libindicate_la_SOURCES = \
	$(indicate_headers) \
	dbus-indicate-server.h \
	server.c \
	indicator.c

libindicate_la_LDFLAGS = \
	-no-undefined \
	-export-symbols-regex "^[^_].*"

libindicate_la_CFLAGS = \
	$(LIBINDICATE_CFLAGS)

libindicate_la_LIBADD = \
	$(LIBINDICATE_LIBS)

dbus-indicate-server.h: indicate-server.xml
	libtool --mode=execute dbus-binding-tool \
		--prefix=indicate_server \
		--mode=glib-server \
		--output=dbus-indicate-server.h \
		$(srcdir)/indicate-server.xml
