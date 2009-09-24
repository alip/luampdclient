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
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <mpd/song.h>

#include "globals.h"

static int lmpdsong_gc(lua_State *L)
{
	struct mpd_song **song;

	song = luaL_checkudata(L, 1, MPD_SONG_T);

	if (*song != NULL)
		mpd_song_free(*song);
	*song = NULL;

	return 0;
}

static int lmpdsong_dup(lua_State *L)
{
	struct mpd_song **song;
	struct mpd_song **newsong;

	song = luaL_checkudata(L, 1, MPD_SONG_T);

	assert(*song != NULL);

	newsong = (struct mpd_song **) lua_newuserdata(L, sizeof(struct mpd_song *));
	luaL_getmetatable(L, MPD_SONG_T);
	lua_setmetatable(L, -2);

	*newsong = mpd_song_dup(*song);
	if (*newsong == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}

	return 1;
}

static int lmpdsong_add_tag(lua_State *L)
{
	int type;
	const char *value;
	struct mpd_song **song;

	song = luaL_checkudata(L, 1, MPD_SONG_T);
	type = luaL_checkinteger(L, 2);
	value = luaL_checkstring(L, 3);

	assert(*song != NULL);

	lua_pushboolean(L, mpd_song_add_tag(*song, type, value));

	return 1;
}

static int lmpdsong_clear_tag(lua_State *L)
{
	int type;
	struct mpd_song **song;

	song = luaL_checkudata(L, 1, MPD_SONG_T);
	type = luaL_checkinteger(L, 2);

	assert(*song != NULL);

	mpd_song_clear_tag(*song, type);

	return 0;
}

static int lmpdsong_get_tag(lua_State *L)
{
	int idx, type;
	struct mpd_song **song;

	song = luaL_checkudata(L, 1, MPD_SONG_T);
	type = luaL_checkinteger(L, 2);
	idx = luaL_checkinteger(L, 3);

	assert(*song != NULL);

	lua_pushstring(L, mpd_song_get_tag(*song, type, idx));

	return 1;
}

static int lmpdsong_index(lua_State *L)
{
	const char *key;
	struct mpd_song **song;

	song = luaL_checkudata(L, 1, MPD_SONG_T);
	key = luaL_checkstring(L, 2);

	assert(*song != NULL);

	if (strncmp(key, "dup", 4) == 0) {
		lua_pushcfunction(L, lmpdsong_dup);
		return 1;
	}
	else if (strncmp(key, "duration", 5) == 0) {
		lua_pushinteger(L, mpd_song_get_duration(*song));
		return 1;
	}
	else if (strncmp(key, "add_tag", 8) == 0) {
		lua_pushcfunction(L, lmpdsong_add_tag);
		return 1;
	}
	else if (strncmp(key, "clear_tag", 10) == 0) {
		lua_pushcfunction(L, lmpdsong_clear_tag);
		return 1;
	}
	else if (strncmp(key, "get_tag", 8) == 0) {
		lua_pushcfunction(L, lmpdsong_get_tag);
		return 1;
	}
	else if (strncmp(key, "uri", 4) == 0) {
		lua_pushstring(L, mpd_song_get_uri(*song));
		return 1;
	}
	else if (strncmp(key, "pos", 4) == 0) {
		lua_pushinteger(L, mpd_song_get_pos(*song));
		return 1;
	}
	else if (strncmp(key, "id", 3) == 0) {
		lua_pushinteger(L, mpd_song_get_id(*song));
		return 1;
	}
	else
		return luaL_error(L, "Invalid key `%s'", key);
}

static int lmpdsong_newindex(lua_State *L)
{
	int value;
	const char *key;
	struct mpd_song **song;

	song = luaL_checkudata(L, 1, MPD_SONG_T);
	key = luaL_checkstring(L, 2);
	value = luaL_checkinteger(L, 3);

	assert(*song != NULL);

	if (strncmp(key, "pos", 4) == 0)
		mpd_song_set_pos(*song, value);
	else
		return luaL_error(L, "Invalid key `%s'", key);

	return 0;
}

static const luaL_reg lreg_song[] = {
	{"__gc",	lmpdsong_gc},
	{"__index",	lmpdsong_index},
	{"__newindex",	lmpdsong_newindex},
	{NULL,		NULL},
};

void linit_song(lua_State *L)
{
	/* Register MPD_SONG_T metatable */
	luaL_newmetatable(L, MPD_SONG_T);
	luaL_register(L, NULL, lreg_song);
	lua_pop(L, 1);

	lua_pushliteral(L, "MPD_TAG_ANY");
	lua_pushinteger(L, MPD_TAG_ANY);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_FILE");
	lua_pushinteger(L, MPD_TAG_FILE);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_ARTIST");
	lua_pushinteger(L, MPD_TAG_ARTIST);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_ALBUM");
	lua_pushinteger(L, MPD_TAG_ALBUM);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_ALBUM_ARTIST");
	lua_pushinteger(L, MPD_TAG_ALBUM_ARTIST);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_TITLE");
	lua_pushinteger(L, MPD_TAG_TITLE);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_TRACK");
	lua_pushinteger(L, MPD_TAG_TRACK);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_NAME");
	lua_pushinteger(L, MPD_TAG_NAME);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_GENRE");
	lua_pushinteger(L, MPD_TAG_GENRE);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_DATE");
	lua_pushinteger(L, MPD_TAG_DATE);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_COMPOSER");
	lua_pushinteger(L, MPD_TAG_COMPOSER);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_PERFORMER");
	lua_pushinteger(L, MPD_TAG_PERFORMER);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_COMMENT");
	lua_pushinteger(L, MPD_TAG_COMMENT);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_DISC");
	lua_pushinteger(L, MPD_TAG_DISC);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_MUSICBRAINZ_ARTISTID");
	lua_pushinteger(L, MPD_TAG_MUSICBRAINZ_ARTISTID);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_MUSICBRAINZ_ALBUMID");
	lua_pushinteger(L, MPD_TAG_MUSICBRAINZ_ALBUMID);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_MUSICBRAINZ_ALBUMARTISTID");
	lua_pushinteger(L, MPD_TAG_MUSICBRAINZ_ALBUMARTISTID);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_MUSICBRAINZ_TRACKID");
	lua_pushinteger(L, MPD_TAG_MUSICBRAINZ_TRACKID);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_TAG_COUNT");
	lua_pushinteger(L, MPD_TAG_COUNT);
	lua_settable(L, -3);
}
