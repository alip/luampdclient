/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet autoindent cindent fdm=syntax : */

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

#include <mpd/pair.h>
#include <mpd/recv.h>

#include "globals.h"

static int lmpdpair_gc(lua_State *L)
{
	struct mpd_entity **pair;

	pair = luaL_checkudata(L, 1, MPD_PAIR_T);

	// if (*pair != NULL)
	//	mpd_return_pair(*pair);
	//*pair = NULL;

	return 0;
}

static int lmpdpair_index(lua_State *L)
{
	const char *key;
	struct mpd_pair **pair;

	pair = luaL_checkudata(L, 1, MPD_PAIR_T);
	key = luaL_checkstring(L, 2);

	assert(*pair != NULL);

	if (strncmp(key, "name", 4) == 0)
		lua_pushstring(L, (*pair)->name);
	else if (strncmp(key, "value", 5) == 0)
		lua_pushstring(L, (*pair)->value);
	else
		return luaL_error(L, "Invalid key `%s'", key);
	return 1;
}

static const luaL_reg lreg_pair[] = {
	{"__gc",	lmpdpair_gc},
	{"__index",	lmpdpair_index},
	{NULL,		NULL},
};

void linit_pair(lua_State *L)
{
	/* Register MPD_PAIR_T metatable */
	luaL_newmetatable(L, MPD_PAIR_T);
	luaL_register(L, NULL, lreg_pair);
	lua_pop(L, 1);
}
