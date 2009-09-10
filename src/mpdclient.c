/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet autoindent cindent fdm=syntax : */

/* libmpdclient Lua bindings
   (c) 2009 Ali Polatel <alip@exherbo.org>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Music Player Daemon nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <assert.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include <mpd/connection.h>
#include <mpd/song.h>

#include "globals.h"

LUALIB_API int luaopen_mpdclient(lua_State *L);

static int mpdclient_new(lua_State *L)
{
	const char *host;
	int port;
	double timeout;
	struct mpd_connection **conn;

	host = luaL_checkstring(L, 1);
	port = luaL_checkinteger(L, 2);
	timeout = luaL_checknumber(L, 3);

	conn = (struct mpd_connection **) lua_newuserdata(L, sizeof(struct mpd_connection *));
	luaL_getmetatable(L, MPD_CONNECTION_T);
	lua_setmetatable(L, -2);

	*conn = mpd_connection_new(host, port, timeout);
	if (*conn == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}

	return 1;
}

static int mpdclient_new_song(lua_State *L)
{
	const char *uri;
	struct mpd_song **song;

	uri = luaL_checkstring(L, 1);

	song = (struct mpd_song **) lua_newuserdata(L, sizeof(struct mpd_song *));
	luaL_getmetatable(L, MPD_SONG_T);
	lua_setmetatable(L, -2);

	*song = mpd_song_new(uri);
	if (*song == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}

	return 1;
}

static const luaL_reg reg_global[] = {
	{"new",		mpdclient_new},
	{"new_song",	mpdclient_new_song},
	{NULL,		NULL},
};

LUALIB_API int luaopen_mpdclient(lua_State *L)
{
	/* Register mpdclient module */
	luaL_register(L, "mpdclient", reg_global);

	linit_connection(L);
	linit_directory(L);
	linit_entity(L);
	linit_error(L);
	linit_idle(L);
	linit_output(L);
	linit_pair(L);
	linit_protocol(L);
	linit_song(L);
	linit_status(L);
	linit_stored_playlist(L);

	return 1;
}
