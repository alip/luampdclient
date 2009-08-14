/* vim: set cino= fo=croql sw=8 ts=8 sts=0 autoindent cindent fdm=syntax : */

/* libmpdclient Lua bindings
   (c) 2003-2009 The Music Player Daemon Project
   This project's homepage is: http://www.musicpd.org

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

#include <mpd/directory.h>
#include <mpd/entity.h>
#include <mpd/song.h>
#include <mpd/stored_playlist.h>

#include "globals.h"

static int lmpdentity_gc(lua_State *L)
{
	struct mpd_entity **entity;

	entity = luaL_checkudata(L, 1, MPD_ENTITY_T);

	if (*entity != NULL)
		mpd_entity_free(*entity);
	*entity = NULL;

	return 0;
}

static int lmpdentity_index(lua_State *L)
{
	const char *key;
	struct mpd_entity **entity;
	struct mpd_directory **dir;
	struct mpd_song **song;
	struct mpd_stored_playlist **spl;

	entity = luaL_checkudata(L, 1, MPD_ENTITY_T);
	key = luaL_checkstring(L, 2);

	assert(*entity != NULL);

	if (strncmp(key, "type", 5) == 0) {
		lua_pushinteger(L, (*entity)->type);
	}
	else if (strncmp(key, "directory", 10) == 0) {
		dir = (struct mpd_directory **) lua_newuserdata(L, sizeof(struct mpd_directory *));
		luaL_getmetatable(L, MPD_DIRECTORY_T);
		lua_setmetatable(L, -2);

		*dir = mpd_directory_dup((*entity)->info.directory);
		if (*dir == NULL)
			lua_pushnil(L);
	}
	else if (strncmp(key, "song", 5) == 0) {
		song = (struct mpd_song **) lua_newuserdata(L, sizeof(struct mpd_song *));
		luaL_getmetatable(L, MPD_SONG_T);
		lua_setmetatable(L, -2);

		*song = mpd_song_dup((*entity)->info.song);
		if (*song == NULL)
			lua_pushnil(L);
	}
	else if (strncmp(key, "playlistFile", 13) == 0) {
		spl = (struct mpd_stored_playlist **) lua_newuserdata(L, sizeof(struct mpd_stored_playlist *));
		luaL_getmetatable(L, MPD_STORED_PLAYLIST_T);
		lua_setmetatable(L, -2);

		*spl = mpd_stored_playlist_dup((*entity)->info.playlistFile);
		if (*spl == NULL)
			lua_pushnil(L);
	}
	else
		return luaL_error(L, "Invalid key `%s'", key);
	return 1;
}

static const luaL_reg lreg_entity[] = {
	{"__gc",	lmpdentity_gc},
	{"__index",	lmpdentity_index},
	{NULL,		NULL},
};

void linit_entity(lua_State *L)
{
	/* Register MPD_ENTITY_T metatable */
	luaL_newmetatable(L, MPD_ENTITY_T);
	luaL_register(L, NULL, lreg_entity);
	lua_pop(L, 1);

	/* Push constants */
	lua_pushliteral(L, "MPD_ENTITY_TYPE_DIRECTORY");
	lua_pushinteger(L, MPD_ENTITY_TYPE_DIRECTORY);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ENTITY_TYPE_SONG");
	lua_pushinteger(L, MPD_ENTITY_TYPE_SONG);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ENTITY_TYPE_PLAYLISTFILE");
	lua_pushinteger(L, MPD_ENTITY_TYPE_PLAYLISTFILE);
	lua_settable(L, -3);
}
