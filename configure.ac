AC_PREREQ([2.65])
AC_INIT(connman-gdbus, 0.1, daniel@zonque.org)
AM_INIT_AUTOMAKE(bfsender, 0.1)
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

AC_CONFIG_MACRO_DIR([m4])

# Checks for libraries.
PKG_CHECK_MODULES(GLIB, [ glib-2.0 >= 2.6.30,
			  gobject-2.0 >= 2.6.30,
			  gio-2.0 >= 2.6.30 ])

AC_OUTPUT([
	Makefile
	src/Makefile
])

