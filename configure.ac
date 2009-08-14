dnl vim: set sw=4 sts=4 ts=4 noet ft=config foldmethod=marker foldmarker={{{,}}} :

dnl {{{ Program
AC_PREREQ(2.59)
AC_INIT([src/mpdclient.c])

AM_INIT_AUTOMAKE(luampdclient, [2.0~git])
dnl }}}

dnl {{{ Toolchain checks
AC_PROG_CC_C99
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
dnl }}}

dnl {{{ Extra CFLAGS
LIBMPDCLIENT_LUA_CFLAGS=
WANTED_CFLAGS="-pedantic -Wall -W -Wextra -Wvla -Wformat=2 -Wformat-security -Wformat-nonliteral -Wlogical-op -Winit-self -Wpointer-arith -Wfloat-equal -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wmissing-noreturn -Wshadow -Wcast-align -Winline"
for flag in $WANTED_CFLAGS ; do
	AX_CHECK_COMPILER_FLAGS([$flag], [LIBMPDCLIENT_LUA_CFLAGS="$LIBMPDCLIENT_LUA_CFLAGS $flag"],)
done
dnl }}}

dnl {{{ Library checks
PKG_PROG_PKG_CONFIG([0.20.0])
PKG_CHECK_MODULES([lua], [lua >= 5.1],,
				  [AC_MSG_ERROR([luampdclient requires lua-5.1 or newer])])
PKG_CHECK_MODULES([libmpdclient], [libmpdclient >= 2.0],,
				  AC_MSG_ERROR([luampdclient requires libmpdclient-2.0 or newer]))
dnl }}}

dnl {{{
AM_CONFIG_HEADER(config.h)
AC_OUTPUT(
		  Makefile
		  src/Makefile
)
dnl }}}
