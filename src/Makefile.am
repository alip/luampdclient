SUBDIRS= .

AM_CFLAGS= $(lua_CFLAGS) $(libmpdclient_CFLAGS)
lua_LTLIBRARIES= mpdclient.la
# FIXME: is there a better way to set luadir?
luadir=$(libdir)/lua/`lua -v 2>&1| cut -d' ' -f2|cut -d'.' -f1,2`/
mpdclient_la_SOURCES= \
			  globals.h \
			  connection.c directory.c entity.c \
			  error.c idle.c output.c pair.c protocol.c \
			  stats.c status.c song.c playlist.c mpdclient.c
mpdclient_la_LDFLAGS = -module -avoid-version
mpdclient_la_LIBADD= $(lua_LIBS) $(libmpdclient_LIBS)
