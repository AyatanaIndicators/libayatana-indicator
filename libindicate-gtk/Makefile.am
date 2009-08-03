
INCLUDES= \
	-DG_LOG_DOMAIN=\"libindicate-gtk\"

EXTRA_DIST = \
	indicate-gtk.pc.in

lib_LTLIBRARIES = \
	libindicate-gtk.la

libindicate_gtkincludedir=$(includedir)/libindicate-0.1/libindicate-gtk

indicate_gtk_headers = \
	indicator.h

libindicate_gtkinclude_HEADERS = \
	$(indicate_gtk_headers)

libindicate_gtk_la_SOURCES = \
	$(indicate_gtk_headers) \
	indicator.c

libindicate_gtk_la_LDFLAGS = \
	-version-info $(LIBINDICATE_CURRENT):$(LIBINDICATE_REVISION):$(LIBINDICATE_AGE) \
	-no-undefined \
	-export-symbols-regex "^[^_d].*"

libindicate_gtk_la_CFLAGS = \
	$(LIBINDICATEGTK_CFLAGS)

libindicate_gtk_la_LIBADD = \
	$(LIBINDICATEGTK_LIBS)

pkgconfig_DATA = indicate-gtk.pc
pkgconfigdir = $(libdir)/pkgconfig

if USE_GIR

gobjectintrospection_gir_DATA = \
	Indicate-gtk-0.1.gir
gobjectintrospection_girdir = $(datadir)/gir

gobjectintrospection_type_DATA = \
	Indicate-gtk-0.1.typelib
gobjectintrospection_typedir = $(libdir)/girepository

irscanner_headers = $(patsubst %,$(srcdir)/%,$(indicate_headers))
Indicate-gtk-0.1.gir: $(irscanner_headers)
	$(G_IR_SCANNER) \
		-v --namespace IndicateGtk \
		--nsversion=0.1 \
		--add-include-path=$(srcdir) \
		--include=GObject-2.0 \
		--include=GLib-2.0 \
		--include=GdkPixbuf-2.0 \
		--library=indicate --pkg indicate \
		--output Indicate-gtk-0.1.gir $(irscanner_headers)

Indicate-gtk-0.1.typelib: Indicate-gtk-0.1.gir
	$(G_IR_COMPILER) \
		--includedir=$(srcdir) Indicate-gtk-0.1.gir \
		-o Indicate-gtk-0.1.typelib

DISTCLEANFILES = \
	Indicate-gtk-0.1.gir \
	Indicate-gtk-0.1.typelib

endif
