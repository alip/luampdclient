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

#include <lua.h>
#include <lauxlib.h>

#include <mpd/protocol.h>

#include "globals.h"

void linit_protocol(lua_State *L)
{
	lua_pushliteral(L, "MPD_ACK_ERROR_UNK");
	lua_pushinteger(L, MPD_ACK_ERROR_UNK);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_NOT_LIST");
	lua_pushinteger(L, MPD_ACK_ERROR_NOT_LIST);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_ARG");
	lua_pushinteger(L, MPD_ACK_ERROR_ARG);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_PASSWORD");
	lua_pushinteger(L, MPD_ACK_ERROR_PASSWORD);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_PERMISSION");
	lua_pushinteger(L, MPD_ACK_ERROR_PERMISSION);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_UNKNOWN_CMD");
	lua_pushinteger(L, MPD_ACK_ERROR_UNKNOWN_CMD);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_NO_EXIST");
	lua_pushinteger(L, MPD_ACK_ERROR_NO_EXIST);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_PLAYLIST_MAX");
	lua_pushinteger(L, MPD_ACK_ERROR_PLAYLIST_MAX);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_SYSTEM");
	lua_pushinteger(L, MPD_ACK_ERROR_SYSTEM);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_PLAYLIST_LOAD");
	lua_pushinteger(L, MPD_ACK_ERROR_PLAYLIST_LOAD);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_UPDATE_ALREADY");
	lua_pushinteger(L, MPD_ACK_ERROR_UPDATE_ALREADY);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_PLAYER_SYNC");
	lua_pushinteger(L, MPD_ACK_ERROR_PLAYER_SYNC);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ACK_ERROR_EXIST");
	lua_pushinteger(L, MPD_ACK_ERROR_EXIST);
	lua_settable(L, -3);
}
