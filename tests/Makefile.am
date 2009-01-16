
noinst_PROGRAMS = \
	indicate-and-crash \
	indicate-alot \
	listen-and-print

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
	$(LIBINDICATE_CFLAGS)

listen_and_print_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)
