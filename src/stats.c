/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet autoindent cindent fdm=syntax : */

/* libmpdclient Lua bindings
   (c) 2009 Ali Polatel <alip@exherbo.org>
   (c) 2009 Michael Forney <michael@obberon.com>

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
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <mpd/stats.h>

#include "globals.h"

static int lmpdstats_gc(lua_State *L)
{
	struct mpd_stats **stats;

	stats = luaL_checkudata(L, 1, MPD_STATS_T);

	if (*stats != NULL)
		mpd_stats_free(*stats);
	*stats = NULL;

	return 0;
}

static int lmpdstats_index(lua_State *L)
{
	const char *key;
	struct mpd_stats **stats;

	stats = luaL_checkudata(L, 1, MPD_STATS_T);
	key = luaL_checkstring(L, 2);

	assert(*stats != NULL);

	if (strncmp(key, "number_of_artists", 18) == 0) {
		lua_pushinteger(L, mpd_stats_get_number_of_artists(*stats));
	}
	else if (strncmp(key, "number_of_albums", 17) == 0) {
		lua_pushinteger(L, mpd_stats_get_number_of_albums(*stats));
	}
	else if (strncmp(key, "number_of_songs", 16) == 0) {
		lua_pushinteger(L, mpd_stats_get_number_of_songs(*stats));
	}
	else if (strncmp(key, "uptime", 7) == 0) {
		lua_pushinteger(L, mpd_stats_get_uptime(*stats));
	}
	else if (strncmp(key, "db_update_time", 15) == 0) {
		lua_pushinteger(L, mpd_stats_get_uptime(*stats));
	}
	else if (strncmp(key, "play_time", 10) == 0) {
		lua_pushinteger(L, mpd_stats_get_play_time(*stats));
	}
	else if (strncmp(key, "db_play_time", 13) == 0) {
		lua_pushinteger(L, mpd_stats_get_db_play_time(*stats));
	}
	else
		return luaL_error(L, "Invalid key `%s'", key);
	return 1;
}

static const luaL_reg lreg_stats[] = {
	{"__gc",	lmpdstats_gc},
	{"__index",	lmpdstats_index},
	{NULL,		NULL},
};

void linit_stats(lua_State *L)
{
	/* Register MPD_STATS_T metatable */
	luaL_newmetatable(L, MPD_STATS_T);
	luaL_register(L, NULL, lreg_stats);
	lua_pop(L, 1);
}
