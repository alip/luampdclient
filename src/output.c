/* vim: set cino= fo=croql sw=8 ts=8 sts=0 autoindent cindent fdm=syntax : */

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

#include <mpd/output.h>

#include "globals.h"

static int lmpdoutput_gc(lua_State *L)
{
	struct mpd_output **output;

	output = luaL_checkudata(L, 1, MPD_OUTPUT_T);

	if (*output != NULL)
		mpd_output_free(*output);
	*output = NULL;

	return 0;
}

static int lmpdoutput_index(lua_State *L)
{
	const char *key;
	struct mpd_output **output;

	output = luaL_checkudata(L, 1, MPD_OUTPUT_T);
	key = luaL_checkstring(L, 2);

	assert(*output != NULL);

	if (strncmp(key, "id", 3) == 0)
		lua_pushinteger(L, mpd_output_get_id(*output));
	else if (strncmp(key, "name", 5) == 0)
		lua_pushstring(L, mpd_output_get_name(*output));
	else if (strncmp(key, "enabled", 8) == 0)
		lua_pushboolean(L, mpd_output_get_enabled(*output));
	else
		return luaL_error(L, "Invalid key `%s'", key);

	return 1;
}

static const luaL_reg lreg_output[] = {
	{"__gc",	lmpdoutput_gc},
	{"__index",	lmpdoutput_index},
	{NULL,		NULL},
};

void linit_output(lua_State *L)
{
	/* Register MPD_OUTPUT_T metatable */
	luaL_newmetatable(L, MPD_OUTPUT_T);
	luaL_register(L, NULL, lreg_output);
	lua_pop(L, 1);
}
