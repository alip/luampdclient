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

#include <lua.h>
#include <lauxlib.h>

#include <mpd/error.h>

#include "globals.h"

void linit_error(lua_State *L)
{
	lua_pushliteral(L, "MPD_ERROR_SUCCESS");
	lua_pushinteger(L, MPD_ERROR_SUCCESS);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_OOM");
	lua_pushinteger(L, MPD_ERROR_OOM);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_ARG");
	lua_pushinteger(L, MPD_ERROR_ARG);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_STATE");
	lua_pushinteger(L, MPD_ERROR_STATE);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_TIMEOUT");
	lua_pushinteger(L, MPD_ERROR_TIMEOUT);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_SYSTEM");
	lua_pushinteger(L, MPD_ERROR_SYSTEM);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_UNKHOST");
	lua_pushinteger(L, MPD_ERROR_UNKHOST);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_CONNPORT");
	lua_pushinteger(L, MPD_ERROR_CONNPORT);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_NOTMPD");
	lua_pushinteger(L, MPD_ERROR_NOTMPD);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_SENDING");
	lua_pushinteger(L, MPD_ERROR_SENDING);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_MALFORMED");
	lua_pushinteger(L, MPD_ERROR_MALFORMED);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_CONNCLOSED");
	lua_pushinteger(L, MPD_ERROR_CONNCLOSED);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_ACK");
	lua_pushinteger(L, MPD_ERROR_ACK);
	lua_settable(L, -3);

	lua_pushliteral(L, "MPD_ERROR_BUFFEROVERRUN");
	lua_pushinteger(L, MPD_ERROR_BUFFEROVERRUN);
	lua_settable(L, -3);
}
