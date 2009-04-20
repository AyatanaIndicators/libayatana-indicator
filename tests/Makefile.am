
TESTS = \
	test_simple

libexec_PROGRAMS = \
	test-simple-client \
	test-simple-server

test_simple: test-simple-client test-simple-server

test_simple_client_SOURCES = \
	test-simple-client.c

test_simple_client_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_simple_client_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_simple_server_SOURCES = \
	test-simple-server.c

test_simple_server_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_simple_server_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

