/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet autoindent cindent fdm=syntax : */

/* libmpdclient Lua bindings
   (c) 2009 Ali Polatel <polatel@gmail.com>

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

#include "globals.h"

static int lmpddirectory_gc(lua_State *L)
{
	struct mpd_directory **dir;

	dir = luaL_checkudata(L, 1, MPD_DIRECTORY_T);

	if (*dir != NULL)
		mpd_directory_free(*dir);
	*dir = NULL;

	return 0;
}

static int lmpddirectory_dup(lua_State *L)
{
	struct mpd_directory **dir;
	struct mpd_directory **newdir;

	dir = luaL_checkudata(L, 1, MPD_DIRECTORY_T);

	assert(*dir != NULL);

	newdir = (struct mpd_directory **) lua_newuserdata(L, sizeof(struct mpd_directory *));
	luaL_getmetatable(L, MPD_DIRECTORY_T);
	lua_setmetatable(L, -2);

	*newdir = mpd_directory_dup(*dir);
	if (*newdir == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}

	return 1;
}

static int lmpddirectory_index(lua_State *L)
{
	const char *key;
	struct mpd_directory **dir;

	dir = luaL_checkudata(L, 1, MPD_DIRECTORY_T);
	key = luaL_checkstring(L, 2);

	assert(*dir != NULL);

	if (strncmp(key, "dup", 4) == 0) {
		lua_pushcfunction(L, lmpddirectory_dup);
		return 1;
	}
	else if (strncmp(key, "path", 5) == 0) {
		if (mpd_directory_get_path(*dir) != NULL)
			lua_pushstring(L, mpd_directory_get_path(*dir));
		else
			lua_pushnil(L);
		return 1;
	}
	else
		return luaL_error(L, "Invalid key `%s'", key);
}

static const luaL_reg lreg_directory[] = {
	{"__gc",	lmpddirectory_gc},
	{"__index",	lmpddirectory_index},
	{NULL,		NULL},
};

void linit_directory(lua_State *L)
{
	/* Register MPD_DIRECTORY_T metatable */
	luaL_newmetatable(L, MPD_DIRECTORY_T);
	luaL_register(L, NULL, lreg_directory);
	lua_pop(L, 1);
}
