
DBUS_RUNNER=/home/ted/Development/dbus-fun/dbus-test-runner --dbus-config /home/ted/Development/dbus-fun/session.conf

TESTS = \
	test_simple

test_simple: test_simple_c test_simple_s
	$(DBUS_RUNNER) --task ./test_simple_c --task-name Client --task ./test_simple_s --task-name Server

test_simple_c_SOURCES = \
	test-simple-c.c

test_simple_c_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_simple_c_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

test_simple_s_SOURCES = \
	test-simple-s.c

test_simple_s_CFLAGS = \
	$(LIBINDICATE_CFLAGS) -I$(srcdir)/..

test_simple_s_LDADD = \
	../libindicate/libindicate.la \
	$(LIBINDICATE_LIBS)

