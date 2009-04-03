
INCLUDES= \
	-DG_LOG_DOMAIN=\"libindicate\"

EXTRA_DIST = \
	indicate-interface.xml \
	indicate-listener.xml \
	listener-marshal.list \
	indicate.pc.in

BUILT_SOURCES = \
	dbus-indicate-server.h \
	dbus-indicate-client.h \
	dbus-listener-server.h \
	dbus-listener-client.h \
	listener-marshal.c \
	listener-marshal.h

lib_LTLIBRARIES = \
	libindicate.la

libindicateincludedir=$(includedir)/libindicate-0.1/libindicate

indicate_headers = \
	indicator.h \
	indicator-message.h \
	listener.h \
	server.h \
	interests.h

libindicateinclude_HEADERS = \
	$(indicate_headers)

libindicate_la_SOURCES = \
	$(indicate_headers) \
	dbus-indicate-server.h \
	dbus-indicate-client.h \
	dbus-listener-server.h \
	dbus-listener-client.h \
	server.c \
	listener.c \
	listener-marshal.c \
	listener-marshal.h \
	indicator.c \
	indicator-message.c \
	interests-priv.h

libindicate_la_LDFLAGS = \
	-version-info $(LIBINDICATE_CURRENT):$(LIBINDICATE_REVISION):$(LIBINDICATE_AGE) \
	-no-undefined \
	-export-symbols-regex "^[^_d].*"

libindicate_la_CFLAGS = \
	$(LIBINDICATE_CFLAGS)

libindicate_la_LIBADD = \
	$(LIBINDICATE_LIBS)

dbus-indicate-server.h: indicate-interface.xml
	dbus-binding-tool \
		--prefix=indicate_server \
		--mode=glib-server \
		--output=dbus-indicate-server.h \
		$(srcdir)/indicate-interface.xml

dbus-indicate-client.h: indicate-interface.xml
	dbus-binding-tool \
		--prefix=indicate_client \
		--mode=glib-client \
		--output=dbus-indicate-client.h \
		$(srcdir)/indicate-interface.xml

dbus-listener-server.h: indicate-listener.xml
	dbus-binding-tool \
		--prefix=indicate_listener \
		--mode=glib-server \
		--output=dbus-listener-server.h \
		$(srcdir)/indicate-listener.xml

dbus-listener-client.h: indicate-listener.xml
	dbus-binding-tool \
		--prefix=indicate_listener \
		--mode=glib-client \
		--output=dbus-listener-client.h \
		$(srcdir)/indicate-listener.xml

listener-marshal.h: $(srcdir)/listener-marshal.list
	glib-genmarshal --header \
		--prefix=_indicate_listener_marshal $(srcdir)/listener-marshal.list \
		> listener-marshal.h

listener-marshal.c: $(srcdir)/listener-marshal.list
	glib-genmarshal --body \
		--prefix=_indicate_listener_marshal $(srcdir)/listener-marshal.list \
		> listener-marshal.c

pkgconfig_DATA = indicate.pc
pkgconfigdir = $(libdir)/pkgconfig

if USE_GIR

gobjectintrospection_gir_DATA = \
	Indicate-0.1.gir
gobjectintrospection_girdir = $(datadir)/gir

gobjectintrospection_type_DATA = \
	Indicate-0.1.typelib
gobjectintrospection_typedir = $(libdir)/girepository

irscanner_headers = $(patsubst %,$(srcdir)/%,$(indicate_headers))
Indicate-0.1.gir: $(irscanner_headers)
	$(G_IR_SCANNER) \
		-v --namespace Indicate \
		--nsversion=0.1 \
		--add-include-path=$(srcdir) \
		--include=GObject-2.0 \
		--include=GLib-2.0 \
		--include=GdkPixbuf-2.0 \
		--library=indicate --pkg indicate \
		--output Indicate-0.1.gir $(irscanner_headers)

Indicate-0.1.typelib: Indicate-0.1.gir
	$(G_IR_COMPILER) \
		--includedir=$(srcdir) Indicate-0.1.gir \
		-o Indicate-0.1.typelib

DISTCLEANFILES = \
	Indicate-0.1.gir \
	Indicate-0.1.typelib

endif
