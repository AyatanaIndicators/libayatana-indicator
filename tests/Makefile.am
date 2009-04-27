
TESTS = \
	test_simple \
	test_interests \
	test_interests_multi \
	test_thousand_indicators

libexec_PROGRAMS = \
	test-interests-client \
	test-interests-server \
	test-interests-server1 \
	test-interests-server2 \
	test-interests-server3 \
	test-interests-server4 \
	test-interests-server5 \
	test-thousand-indicators-client \
	test-thousand-indicators-server \
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

test_interests_multi: test-interests-client test-interests-server1 test-interests-server2 test-interests-server3 test-interests-server4 test-interests-server5

test_interests_server1_SOURCES = \
	test-interests-server1.c

test_interests_server1_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_server1_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_interests_server2_SOURCES = \
	test-interests-server2.c

test_interests_server2_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_server2_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_interests_server3_SOURCES = \
	test-interests-server3.c

test_interests_server3_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_server3_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_interests_server4_SOURCES = \
	test-interests-server4.c

test_interests_server4_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_server4_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_interests_server5_SOURCES = \
	test-interests-server5.c

test_interests_server5_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_interests_server5_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_thousand_indicators: test-thousand-indicators-client test-thousand-indicators-server

test_thousand_indicators_client_SOURCES = \
	test-thousand-indicators-client.c

test_thousand_indicators_client_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_thousand_indicators_client_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_thousand_indicators_server_SOURCES = \
	test-thousand-indicators-server.c

test_thousand_indicators_server_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_thousand_indicators_server_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

