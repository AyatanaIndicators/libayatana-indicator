
TESTS = \
	test_simple \
	test_interests

libexec_PROGRAMS = \
	test-interests-client \
	test-interests-server \
	test-simple-client \
	test-simple-server

EXTRA_DIST = \
	$(TESTS)

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

test_interests: test-interests-client test-interests-server

test_interests_client_SOURCES = \
	test-interests-client.c

test_interests_client_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_client_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_interests_server_SOURCES = \
	test-interests-server.c

test_interests_server_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_server_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

