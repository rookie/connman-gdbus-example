INCLUDES = @GLIB_CFLAGS@ @GOBJECT_CFLAGS@ @GIO_CFLAGS@ @GIO_UNIX_CFLAGS@

sbin_PROGRAMS = connman-test

connman_test_SOURCES = 		\
	main.c 			\
	connman-generated.c	\
	connman-manager.c	\
	connman-service.c	\
	connman-technology.c

connman_test_CFLAGS = -Wall -O0 -g

connman_test_LDADD = @GLIB_LIBS@ @GOBJECT_LIBS@ @GIO_LIBS@ @GIO_UNIX_CFLAGS@

