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
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <mpd/connection.h>
#include <mpd/command.h>
#include <mpd/cpos.h>
#include <mpd/entity.h>
#include <mpd/idle.h>
#include <mpd/list.h>
#include <mpd/output.h>
#include <mpd/pair.h>
#include <mpd/response.h>
#include <mpd/status.h>

#include "globals.h"

/* connection.h */
static int lmpdconn_gc(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	if (*conn != NULL)
		mpd_connection_free(*conn);
	*conn = NULL;

	return 0;
}

static int lmpdconn_set_timeout(lua_State *L)
{
	struct mpd_connection **conn;
	double timeout;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	timeout = luaL_checknumber(L, 2);

	assert(*conn != NULL);

	mpd_connection_set_timeout(*conn, timeout);
	return 0;
}

static int lmpdconn_get_error(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_get_error(*conn));

	return 1;
}

static int lmpdconn_get_error_message(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushstring(L, mpd_get_error_message(*conn));

	return 1;
}

static int lmpdconn_get_server_error(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_get_server_error(*conn));

	return 1;
}

static int lmpdconn_clear_error(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_clear_error(*conn));

	return 1;
}

static int lmpdconn_get_server_version(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_get_server_version(*conn)[0]);
	lua_pushinteger(L, mpd_get_server_version(*conn)[1]);
	lua_pushinteger(L, mpd_get_server_version(*conn)[2]);

	return 3;
}

static int lmpdconn_recv_pair(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_pair *pair;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	pair = mpd_recv_pair(*conn);
	if (pair == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "mpd_recv_pair");
		return 2;
	}

	lua_newtable(L);

	lua_pushliteral(L, "name");
	lua_pushstring(L, pair->name);
	lua_settable(L, -3);

	lua_pushliteral(L, "value");
	lua_pushstring(L, pair->value);
	lua_settable(L, -3);

	mpd_return_pair(*conn, pair);
	return 1;
}

static int lmpdconn_recv_pair_named(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;
	struct mpd_pair *pair;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	pair = mpd_recv_pair_named(*conn, name);
	if (pair == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "mpd_recv_pair_named");
		return 2;
	}

	lua_newtable(L);

	lua_pushliteral(L, "name");
	lua_pushstring(L, pair->name);
	lua_settable(L, -3);

	lua_pushliteral(L, "value");
	lua_pushstring(L, pair->value);
	lua_settable(L, -3);

	mpd_return_pair(*conn, pair);
	return 1;
}

static int lmpdconn_recv_value_named(lua_State *L)
{
	char *value;
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	value = mpd_recv_value_named(*conn, name);

	lua_pushstring(L, value);
	free(value);

	return 1;
}

/* FIXME: Find a way to get this to work
 * libmpdclient no longer wants you to make pairs yourself
 * Maybe create another user data for the pair?
 */
#if 0
static int lmpdconn_enqueue_pair(lua_State *L)
{
	const char *name, *value;
	struct mpd_connection **conn;
	struct mpd_pair *pair;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	luaL_checktype(L, 2, LUA_TTABLE);

	assert(*conn != NULL);

	lua_pushliteral(L, "name");
	lua_gettable(L, -2);
	name = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	lua_pushliteral(L, "value");
	lua_gettable(L, -2);
	value = luaL_checkstring(L, -1);
	lua_pop(L, 1);

	pair = mpd_pair_new(name, value);
	if (pair == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}

	mpd_enqueue_pair(*conn, pair);
	lua_pushboolean(L, 1);
	return 1;
}
#endif

/* command.h */
static int lmpdconn_send_status(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_status(*conn));

	return 1;
}

static int lmpdconn_send_currentsong(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_currentsong(*conn));

	return 1;
}

static int lmpdconn_send_play(lua_State *L)
{
	int song_pos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_play(*conn, song_pos));

	return 1;
}

static int lmpdconn_send_playid(lua_State *L)
{
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_play(*conn, id));

	return 1;
}

static int lmpdconn_send_stop(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_stop(*conn));

	return 1;
}

static int lmpdconn_send_pause(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_pause(*conn, mode));

	return 1;
}

static int lmpdconn_send_next(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_next(*conn));

	return 1;
}

static int lmpdconn_send_previous(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_previous(*conn));

	return 1;
}

static int lmpdconn_send_seek(lua_State *L)
{
	int song_pos, time;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);
	time = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_seek(*conn, song_pos, time));

	return 1;
}

static int lmpdconn_send_seekid(lua_State *L)
{
	int id, time;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);
	time = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_seekid(*conn, id, time));

	return 1;
}

static int lmpdconn_send_repeat(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_repeat(*conn, mode));

	return 1;
}

static int lmpdconn_send_random(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_random(*conn, mode));

	return 1;
}

static int lmpdconn_send_single(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_single(*conn, mode));

	return 1;
}

static int lmpdconn_send_consume(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_consume(*conn, mode));

	return 1;
}

static int lmpdconn_send_crossfade(lua_State *L)
{
	int seconds;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	seconds = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_crossfade(*conn, seconds));

	return 1;
}

static int lmpdconn_send_playlistinfo(lua_State *L)
{
	int song_pos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlistinfo(*conn, song_pos));

	return 1;
}

static int lmpdconn_send_playlistid(lua_State *L)
{
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlistinfo(*conn, id));

	return 1;
}

static int lmpdconn_send_plchanges(lua_State *L)
{
	double playlist;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	playlist = luaL_checknumber(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_plchanges(*conn, playlist));

	return 1;
}

static int lmpdconn_send_plchangesposid(lua_State *L)
{
	double playlist;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	playlist = luaL_checknumber(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_plchangesposid(*conn, playlist));

	return 1;
}

static int lmpdconn_send_add(lua_State *L)
{
	const char *file;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	file = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_add(*conn, file));

	return 1;
}

static int lmpdconn_send_addid(lua_State *L)
{
	const char *file;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	file = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_addid(*conn, file));

	return 1;
}

static int lmpdconn_send_delete(lua_State *L)
{
	int song_pos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_delete(*conn, song_pos));

	return 1;
}

static int lmpdconn_send_deleteid(lua_State *L)
{
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_deleteid(*conn, id));

	return 1;
}

static int lmpdconn_send_shuffle(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_shuffle(*conn));

	return 1;
}

static int lmpdconn_send_shuffle_range(lua_State *L)
{
	int start, end;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	start = luaL_checkinteger(L, 2);
	end = luaL_checkinteger(L, 3);

	assert(start > 0);
	assert(end > 0);
	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_shuffle_range(*conn, start, end));

	return 1;
}

static int lmpdconn_send_clear(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_clear(*conn));

	return 1;
}

static int lmpdconn_send_move(lua_State *L)
{
	int from, to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkinteger(L, 2);
	to = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_move(*conn, from, to));

	return 1;
}

static int lmpdconn_send_moveid(lua_State *L)
{
	int from, to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkinteger(L, 2);
	to = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_moveid(*conn, from, to));

	return 1;
}

static int lmpdconn_send_swap(lua_State *L)
{
	int pos1, pos2;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	pos1 = luaL_checkinteger(L, 2);
	pos2 = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_swap(*conn, pos1, pos2));

	return 1;
}

static int lmpdconn_send_swapid(lua_State *L)
{
	int id1, id2;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id1 = luaL_checkinteger(L, 2);
	id2 = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_swapid(*conn, id1, id2));

	return 1;
}

static int lmpdconn_send_listplaylist(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_listplaylist(*conn, name));

	return 1;
}

static int lmpdconn_send_listplaylistinfo(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_listplaylistinfo(*conn, name));

	return 1;
}

static int lmpdconn_send_playlistclear(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlistclear(*conn, name));

	return 1;
}

static int lmpdconn_send_playlistadd(lua_State *L)
{
	const char *name, *path;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	path = luaL_checkstring(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlistadd(*conn, name, path));

	return 1;
}

static int lmpdconn_send_playlistmove(lua_State *L)
{
	int from, to;
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	from = luaL_checkinteger(L, 3);
	to = luaL_checkinteger(L, 4);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlistmove(*conn, name, from, to));

	return 1;
}

static int lmpdconn_send_playlistdelete(lua_State *L)
{
	int pos;
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	pos = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlistdelete(*conn, name, pos));

	return 1;
}

static int lmpdconn_send_save(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_save(*conn, name));

	return 1;
}

static int lmpdconn_send_load(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_load(*conn, name));

	return 1;
}

static int lmpdconn_send_rename(lua_State *L)
{
	const char *from, *to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkstring(L, 2);
	to = luaL_checkstring(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_rename(*conn, from, to));

	return 1;
}

static int lmpdconn_send_rm(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_rm(*conn, name));

	return 1;
}

static int lmpdconn_send_listall(lua_State *L)
{
	const char *dir;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	dir = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_listall(*conn, dir));

	return 1;
}

static int lmpdconn_send_listallinfo(lua_State *L)
{
	const char *dir;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	dir = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_listallinfo(*conn, dir));

	return 1;
}

static int lmpdconn_send_lsinfo(lua_State *L)
{
	const char *dir;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	dir = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_lsinfo(*conn, dir));

	return 1;
}

static int lmpdconn_send_update(lua_State *L)
{
	const char *path;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	path = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_update(*conn, path));

	return 1;
}

static int lmpdconn_send_setvol(lua_State *L)
{
	int change;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	change = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_setvol(*conn, change));

	return 1;
}

static int lmpdconn_send_volume(lua_State *L)
{
	int change;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	change = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_volume(*conn, change));

	return 1;
}

static int lmpdconn_send_password(lua_State *L)
{
	const char *password;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	password = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_password(*conn, password));

	return 1;
}

static int lmpdconn_send_commands(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_commands(*conn));

	return 1;
}

static int lmpdconn_send_notcommands(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_notcommands(*conn));

	return 1;
}

static int lmpdconn_send_urlhandlers(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_urlhandlers(*conn));

	return 1;
}

static int lmpdconn_send_tagtypes(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_tagtypes(*conn));

	return 1;
}

static int lmpdconn_send_outputs(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_outputs(*conn));

	return 1;
}

static int lmpdconn_send_enable_output(lua_State *L)
{
	int output_id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	output_id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);
	assert(output_id >= 0);

	lua_pushboolean(L, mpd_send_enable_output(*conn, output_id));

	return 1;
}

static int lmpdconn_send_disable_output(lua_State *L)
{
	int output_id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	output_id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);
	assert(output_id >= 0);

	lua_pushboolean(L, mpd_send_disable_output(*conn, output_id));

	return 1;
}

/* response.h */
static int lmpdconn_response_finish(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_response_finish(*conn));

	return 1;
}

static int lmpdconn_response_next(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_response_next(*conn));

	return 1;
}

static int lmpdconn_recv_song_id(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_recv_song_id(*conn));

	return 1;
}

static int lmpdconn_recv_update_id(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_recv_update_id(*conn));

	return 1;
}

static int lmpdconn_recv_command_name(lua_State *L)
{
	char *command;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	command = mpd_recv_command_name(*conn);
	if (command == NULL)
		lua_pushnil(L);
	else {
		lua_pushstring(L, command);
		free(command);
	}

	return 1;
}

static int lmpdconn_recv_handler(lua_State *L)
{
	char *handler;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	handler = mpd_recv_handler(*conn);
	if (handler == NULL)
		lua_pushnil(L);
	else {
		lua_pushstring(L, handler);
		free(handler);
	}

	return 1;
}

static int lmpdconn_recv_tag_type_name(lua_State *L)
{
	char *tag_type_name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	tag_type_name = mpd_recv_handler(*conn);
	if (tag_type_name == NULL)
		lua_pushnil(L);
	else {
		lua_pushstring(L, tag_type_name);
		free(tag_type_name);
	}

	return 1;
}

/* status.h */
static int lmpdconn_recv_status(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_status **status;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	status = (struct mpd_status **) lua_newuserdata(L, sizeof(struct mpd_status *));
	luaL_getmetatable(L, MPD_STATUS_T);
	lua_setmetatable(L, -2);

	*status = mpd_recv_status(*conn);
	if (*status == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}

	return 1;
}

/* entity.h */
static int lmpdconn_recv_entity(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_entity **entity;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	entity = (struct mpd_entity **) lua_newuserdata(L, sizeof(struct mpd_entity *));
	luaL_getmetatable(L, MPD_ENTITY_T);
	lua_setmetatable(L, -2);

	*entity = mpd_recv_entity(*conn);
	if (*entity == NULL)
		lua_pushnil(L);
	return 1;
}

/* idle.h */
static int lmpdconn_send_idle(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_idle(*conn));

	return 1;
}

static int lmpdconn_send_noidle(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_noidle(*conn));

	return 1;
}

static int lmpdconn_recv_idle(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_recv_idle(*conn));

	return 1;
}

/* cpos.h */
static int lmpdconn_recv_cpos(lua_State *L)
{
	struct mpd_cpos cpos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	luaL_checktype(L, 2, LUA_TTABLE);

	assert(*conn != NULL);

	lua_pushliteral(L, "position");
	lua_gettable(L, -2);
	cpos.position = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_pushliteral(L, "id");
	lua_gettable(L, -2);
	cpos.id = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_pushboolean(L, mpd_recv_cpos(*conn, &cpos));

	return 1;
}

/* list.h */
static int lmpdconn_command_list_begin(lua_State *L)
{
	bool discrete_ok;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	discrete_ok = lua_toboolean(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_command_list_begin(*conn, discrete_ok));

	return 1;
}

static int lmpdconn_command_list_end(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_command_list_end(*conn));

	return 1;
}

static int lmpdconn_recv_output(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_output **output;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	output = (struct mpd_output **) lua_newuserdata(L, sizeof(struct mpd_output *));
	luaL_getmetatable(L, MPD_OUTPUT_T);
	lua_setmetatable(L, -2);

	*output = mpd_recv_output(*conn);
	if (*output == NULL)
		lua_pushnil(L);
	return 1;
}

static const luaL_reg lreg_connection[] = {
	/* connection.h */
	{"__gc",			lmpdconn_gc},
	{"close",			lmpdconn_gc},
	{"set_timeout",			lmpdconn_set_timeout},
	{"get_error",			lmpdconn_get_error},
	{"get_error_message",		lmpdconn_get_error_message},
	{"get_server_error",		lmpdconn_get_server_error},
	{"clear_error",			lmpdconn_clear_error},
	{"get_server_version",		lmpdconn_get_server_version},
	{"recv_pair",			lmpdconn_recv_pair},
	{"recv_pair_named",		lmpdconn_recv_pair_named},
	{"recv_value_named",		lmpdconn_recv_value_named},
	//{"enqueue_pair",		lmpdconn_enqueue_pair},
	/* command.h */
	{"send_status",			lmpdconn_send_status},
	{"send_currentsong",		lmpdconn_send_currentsong},
	{"send_play",			lmpdconn_send_play},
	{"send_playid",			lmpdconn_send_playid},
	{"send_stop",			lmpdconn_send_stop},
	{"send_pause",			lmpdconn_send_pause},
	{"send_next",			lmpdconn_send_next},
	{"send_previous",		lmpdconn_send_previous},
	{"send_seek",			lmpdconn_send_seek},
	{"send_seekid",			lmpdconn_send_seekid},
	{"send_repeat",			lmpdconn_send_repeat},
	{"send_random",			lmpdconn_send_random},
	{"send_single",			lmpdconn_send_single},
	{"send_consume",		lmpdconn_send_consume},
	{"send_crossfade",		lmpdconn_send_crossfade},
	{"send_playlistinfo",		lmpdconn_send_playlistinfo},
	{"send_playlistid",		lmpdconn_send_playlistid},
	{"send_plchanges",		lmpdconn_send_plchanges},
	{"send_plchangesposid",		lmpdconn_send_plchangesposid},
	{"send_add",			lmpdconn_send_add},
	{"send_addid",			lmpdconn_send_addid},
	{"send_delete",			lmpdconn_send_delete},
	{"send_deleteid",		lmpdconn_send_deleteid},
	{"send_shuffle",		lmpdconn_send_shuffle},
	{"send_shuffle_range",		lmpdconn_send_shuffle_range},
	{"send_clear",			lmpdconn_send_clear},
	{"send_move",			lmpdconn_send_move},
	{"send_moveid",			lmpdconn_send_moveid},
	{"send_swap",			lmpdconn_send_swap},
	{"send_swapid",			lmpdconn_send_swapid},
	{"send_listplaylist",		lmpdconn_send_listplaylist},
	{"send_listplaylistinfo",	lmpdconn_send_listplaylistinfo},
	{"send_playlistclear",		lmpdconn_send_playlistclear},
	{"send_playlistadd",		lmpdconn_send_playlistadd},
	{"send_playlistmove",		lmpdconn_send_playlistmove},
	{"send_playlistdelete",		lmpdconn_send_playlistdelete},
	{"send_save",			lmpdconn_send_save},
	{"send_load",			lmpdconn_send_load},
	{"send_rename",			lmpdconn_send_rename},
	{"send_rm",			lmpdconn_send_rm},
	{"send_listall",		lmpdconn_send_listall},
	{"send_listallinfo",		lmpdconn_send_listallinfo},
	{"send_lsinfo",			lmpdconn_send_lsinfo},
	{"send_update",			lmpdconn_send_update},
	{"send_setvol",			lmpdconn_send_setvol},
	{"send_volume",			lmpdconn_send_volume},
	{"send_password",		lmpdconn_send_password},
	{"send_commands",		lmpdconn_send_commands},
	{"send_notcommands",		lmpdconn_send_notcommands},
	{"send_urlhandlers",		lmpdconn_send_urlhandlers},
	{"send_tagtypes",		lmpdconn_send_tagtypes},
	{"send_outputs",		lmpdconn_send_outputs},
	{"send_enable_output",		lmpdconn_send_enable_output},
	{"send_disabl_eoutput",		lmpdconn_send_disable_output},
	/* response.h */
	{"response_finish",		lmpdconn_response_finish},
	{"reponse_next",		lmpdconn_response_next},
	{"recv_song_id",		lmpdconn_recv_song_id},
	{"recv_update_id",		lmpdconn_recv_update_id},
	{"recv_command_name",		lmpdconn_recv_command_name},
	{"recv_handler",		lmpdconn_recv_handler},
	{"recv_tag_type_name",		lmpdconn_recv_tag_type_name},
	/* status.h */
	{"recv_status",			lmpdconn_recv_status},
	/* entity.h */
	{"recv_entity",			lmpdconn_recv_entity},
	/* idle.h */
	{"send_idle",			lmpdconn_send_idle},
	{"send_noidle",			lmpdconn_send_noidle},
	{"recv_idle",			lmpdconn_recv_idle},
	/* cpos.h */
	{"recv_cpos",			lmpdconn_recv_cpos},
	/* list.h */
	{"command_list_begin",		lmpdconn_command_list_begin},
	{"command_list_end",		lmpdconn_command_list_end},
	/* output.h */
	{"recv_output",			lmpdconn_recv_output},
	{NULL,				NULL},
};

void linit_connection(lua_State *L)
{
	/* Register MPD_CONNECTION_T metatable */
	luaL_newmetatable(L, MPD_CONNECTION_T);
	luaL_register(L, NULL, lreg_connection);
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2); /* push the metatable */
	lua_settable(L, -3); /* metatable.__index = metatable */
	lua_pop(L, 1);

	/* Push constants */
	lua_pushliteral(L, "MPD_PLAY_AT_BEGINNING");
	lua_pushinteger(L, MPD_PLAY_AT_BEGINNING);
	lua_settable(L, -3);
}
