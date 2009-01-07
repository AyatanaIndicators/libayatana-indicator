
INCLUDES= \
	-DG_LOG_DOMAIN=\"indicate\"

EXTRA_DIST = \
	indicate-server.xml

libindicateincludedir=$(includedir)/libindicate-1.0/libindicate

indicate_headers = \
	indicator.h \
	server.h

libindicateinclude_HEADERS = \
	$(indicate_headers)

libindicate_SOURCES = \
	$(indicate_headers) \
	dbus-indicate-server.h
	server.c \
	indicator.c

libindicate_LDFLAGS = \
	-version-info 0:0:1 \
	-no-undefined \
	-export-symbols-regex "^[^_].*"

dbus-indicate-server.h: indicate-server.xml
	libtool --mode=execute dbus-binding-tool \
		--prefix=indicate_server \
		--mode=glib-server \
		--output=dbus-indicate-server.h \
		$(srcdir)/indicate-server.xml
