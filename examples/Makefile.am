
libexec_PROGRAMS = \
	indicate-and-crash \
	indicate-alot \
	listen-and-print \
	im-client \
	show-hide-server

indicate_and_crash_SOURCES = \
	indicate-and-crash.c

indicate_and_crash_CFLAGS = \
	-I $(srcdir)/.. \
	$(LIBINDICATE_CFLAGS)

indicate_and_crash_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

indicate_alot_SOURCES = \
	indicate-alot.c

indicate_alot_CFLAGS = \
	-I $(srcdir)/.. \
	$(LIBINDICATE_CFLAGS)

indicate_alot_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

listen_and_print_SOURCES = \
	listen-and-print.c

listen_and_print_CFLAGS = \
	-I $(srcdir)/.. \
	$(LIBINDICATE_CFLAGS) \
	$(LIBINDICATEGTK_CFLAGS)

listen_and_print_LDADD = \
	../libindicate/libindicate.la \
	../libindicate-gtk/libindicate-gtk.la \
	$(LIBINDICATE_LIBS) \
	$(LIBINDICATEGTK_LIBS)

im_client_SOURCES = \
	im-client.c

im_client_CFLAGS = \
	-I $(srcdir)/.. \
	$(LIBINDICATE_CFLAGS) \
	$(LIBINDICATEGTK_CFLAGS)

im_client_LDADD = \
	../libindicate/libindicate.la \
	../libindicate-gtk/libindicate-gtk.la \
	$(LIBINDICATE_LIBS) \
	$(LIBINDICATEGTK_LIBS)

show_hide_server_SOURCES = \
	show-hide-server.c

show_hide_server_CFLAGS = \
	-I $(srcdir)/.. \
	$(LIBINDICATE_CFLAGS)

show_hide_server_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

examplesdir = $(docdir)/examples/

examples_DATA = \
	$(indicate_and_crash_SOURCES) \
	$(indicate_alot_SOURCES) \
	$(listen_and_print_SOURCES) \
	$(im_client_SOURCES)

EXTRA_DIST = $(examples_DATA)
