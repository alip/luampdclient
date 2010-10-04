/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet autoindent cindent fdm=syntax : */

/* libmpdclient Lua bindings
   (c) 2009, 2010 Ali Polatel <alip@exherbo.org>

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

#include <mpd/audio_format.h>
#include <mpd/status.h>

#include "globals.h"

static int lmpdstatus_gc(lua_State *L)
{
	struct mpd_status **status;

	status = luaL_checkudata(L, 1, MPD_STATUS_T);

	if (*status != NULL)
		mpd_status_free(*status);
	*status = NULL;

	return 0;
}

static int lmpdstatus_index(lua_State *L)
{
	const char *key;
	const struct mpd_audio_format *audio_format;
	struct mpd_status **status;

	status = luaL_checkudata(L, 1, MPD_STATUS_T);
	key = luaL_checkstring(L, 2);

	assert(*status != NULL);

	if (strncmp(key, "volume", 7) == 0) {
		lua_pushinteger(L, mpd_status_get_volume(*status));
		return 1;
	}
	else if (strncmp(key, "repeat", 7) == 0) {
		lua_pushinteger(L, mpd_status_get_repeat(*status));
		return 1;
	}
	else if (strncmp(key, "random", 7) == 0) {
		lua_pushinteger(L, mpd_status_get_random(*status));
		return 1;
	}
	else if (strncmp(key, "single", 7) == 0) {
		lua_pushinteger(L, mpd_status_get_single(*status));
		return 1;
	}
	else if (strncmp(key, "consume", 8) == 0) {
		lua_pushinteger(L, mpd_status_get_consume(*status));
		return 1;
	}
	else if (strncmp(key, "queue_length", 16) == 0) {
		lua_pushnumber(L, mpd_status_get_queue_length(*status));
		return 1;
	}
	else if (strncmp(key, "queue_version", 17) == 0) {
		lua_pushnumber(L, mpd_status_get_queue_version(*status));
		return 1;
	}
	else if (strncmp(key, "state", 6) == 0) {
		lua_pushinteger(L, mpd_status_get_state(*status));
		return 1;
	}
	else if (strncmp(key, "crossfade", 10) == 0) {
		lua_pushinteger(L, mpd_status_get_crossfade(*status));
		return 1;
	}
	else if (strncmp(key, "song_pos", 5) == 0) {
		lua_pushinteger(L, mpd_status_get_song_pos(*status));
		return 1;
	}
	else if (strncmp(key, "song_id", 7) == 0) {
		lua_pushinteger(L, mpd_status_get_song_id(*status));
		return 1;
	}
	else if (strncmp(key, "elapsed_time", 13) == 0) {
		lua_pushinteger(L, mpd_status_get_elapsed_time(*status));
		return 1;
	}
	else if (strncmp(key, "total_time", 11) == 0) {
		lua_pushinteger(L, mpd_status_get_total_time(*status));
		return 1;
	}
	else if (strncmp(key, "kbit_rate", 10) == 0) {
		lua_pushinteger(L, mpd_status_get_kbit_rate(*status));
		return 1;
	}
	else if (strncmp(key, "audio_format", 13) == 0) {
		audio_format = mpd_status_get_audio_format(*status);
		lua_newtable(L);

		lua_pushinteger(L, audio_format->sample_rate);
		lua_setfield(L, -2, "sample_rate");

		lua_pushinteger(L, audio_format->bits);
		lua_setfield(L, -2, "bits");

		lua_pushinteger(L, audio_format->channels);
		lua_setfield(L, -2, "channels");

		return 1;
	}
	else if (strncmp(key, "update_id", 11) == 0) {
		lua_pushinteger(L, mpd_status_get_update_id(*status));
		return 1;
	}
	else if (strncmp(key, "error", 6) == 0) {
		lua_pushstring(L, mpd_status_get_error(*status));
		return 1;
	}
	else
		return luaL_error(L, "Invalid key `%s'", key);
}

static const luaL_reg lreg_status[] = {
	{"__gc",	lmpdstatus_gc},
	{"__index",	lmpdstatus_index},
	{NULL,		NULL},
};

void linit_status(lua_State *L)
{
	/* Register MPD_STATUS_T metatable */
	luaL_newmetatable(L, MPD_STATUS_T);
	luaL_register(L, NULL, lreg_status);
	lua_pop(L, 1);

	lua_pushliteral(L, "MPD_STATE_UNKNOWN");
	lua_pushinteger(L, MPD_STATE_UNKNOWN);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_STATE_STOP");
	lua_pushinteger(L, MPD_STATE_STOP);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_STATE_PLAY");
	lua_pushinteger(L, MPD_STATE_PLAY);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_STATE_PAUSE");
	lua_pushinteger(L, MPD_STATE_PAUSE);
	lua_settable(L, -3);
}
