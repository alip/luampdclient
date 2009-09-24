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
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include <mpd/connection.h>
#include <mpd/capabilities.h>
#include <mpd/entity.h>
#include <mpd/idle.h>
#include <mpd/list.h>
#include <mpd/mixer.h>
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

static int lmpdconn_cmp_server_version(lua_State *L)
{
	struct mpd_connection **conn;
	lua_Integer major, minor, patch;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	major = luaL_checkinteger(L, 2);
	minor = luaL_checkinteger(L, 3);
	patch = luaL_checkinteger(L, 4);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_cmp_server_version(*conn, major, minor, patch));

	return 1;
}

/* capabilities.h */
static int lmpdconn_send_allowed_commands(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_allowed_commands(*conn));

	return 1;
}

static int lmpdconn_send_disallowed_commands(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_disallowed_commands(*conn));

	return 1;
}

static int lmpdconn_recv_command_pair(lua_State *L)
{
	struct mpd_pair **pair;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	pair = (struct mpd_pair **) lua_newuserdata(L, sizeof(struct mpd_pair *));
	luaL_getmetatable(L, MPD_PAIR_T);
	lua_setmetatable(L, -2);

	*pair = mpd_recv_command_pair(*conn);
	if (*pair == NULL)
		lua_pushnil(L);
	return 1;
}

static int lmpdconn_send_list_url_schemes(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_url_schemes(*conn));

	return 1;
}

static int lmpdconn_recv_url_scheme_pair(lua_State *L)
{
	struct mpd_pair **pair;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	pair = (struct mpd_pair **) lua_newuserdata(L, sizeof(struct mpd_pair *));
	luaL_getmetatable(L, MPD_PAIR_T);
	lua_setmetatable(L, -2);

	*pair = mpd_recv_url_scheme_pair(*conn);
	if (*pair == NULL)
		lua_pushnil(L);
	return 1;
}

static int lmpdconn_send_list_tag_types(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_tag_types(*conn));

	return 1;
}

static int lmpdconn_recv_tag_type_pair(lua_State *L)
{
	struct mpd_pair **pair;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	pair = (struct mpd_pair **) lua_newuserdata(L, sizeof(struct mpd_pair *));
	luaL_getmetatable(L, MPD_PAIR_T);
	lua_setmetatable(L, -2);

	*pair = mpd_recv_tag_type_pair(*conn);
	if (*pair == NULL)
		lua_pushnil(L);
	return 1;
}

/* database.h */
static int lmpdconn_send_list_all(lua_State *L)
{
	const char *dir;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	dir = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_all(*conn, dir));

	return 1;
}

static int lmpdconn_send_list_all_meta(lua_State *L)
{
	const char *dir;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	dir = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_all_meta(*conn, dir));

	return 1;
}

static int lmpdconn_send_list_meta(lua_State *L)
{
	const char *dir;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	dir = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_meta(*conn, dir));

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

static int lmpdconn_recv_update_id(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_recv_update_id(*conn));

	return 1;
}

static int lmpdconn_run_update(lua_State *L)
{
	const char *path;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	path = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_run_update(*conn, path));

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

static int lmpdconn_run_idle(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_run_idle(*conn));

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

/* mixer.h */
static int lmpdconn_send_set_volume(lua_State *L)
{
	int change;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	change = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_set_volume(*conn, change));

	return 1;
}

static int lmpdconn_run_set_volume(lua_State *L)
{
	int change;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	change = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_set_volume(*conn, change));

	return 1;
}

/* output.h */
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

static int lmpdconn_run_enable_output(lua_State *L)
{
	int output_id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	output_id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);
	assert(output_id >= 0);

	lua_pushboolean(L, mpd_run_enable_output(*conn, output_id));

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

static int lmpdconn_run_disable_output(lua_State *L)
{
	int output_id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	output_id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);
	assert(output_id >= 0);

	lua_pushboolean(L, mpd_run_disable_output(*conn, output_id));

	return 1;
}

/* password.h */
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

static int lmpdconn_run_password(lua_State *L)
{
	const char *password;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	password = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_password(*conn, password));

	return 1;
}

/* player.h */
static int lmpdconn_send_current_song(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_current_song(*conn));

	return 1;
}

static int lmpdconn_run_current_song(lua_State *L)
{
	struct mpd_song **song;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	song = (struct mpd_song **) lua_newuserdata(L, sizeof(struct mpd_song *));
	luaL_getmetatable(L, MPD_SONG_T);
	lua_setmetatable(L, -2);

	*song = mpd_run_current_song(*conn);

	return 1;
}

static int lmpdconn_send_play(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_play(*conn));

	return 1;
}

static int lmpdconn_run_play(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_play(*conn));

	return 1;
}

static int lmpdconn_send_play_pos(lua_State *L)
{
	int song_pos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_play_pos(*conn, song_pos));

	return 1;
}

static int lmpdconn_run_play_pos(lua_State *L)
{
	int song_pos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_play_pos(*conn, song_pos));

	return 1;
}

static int lmpdconn_send_play_id(lua_State *L)
{
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_play_id(*conn, id));

	return 1;
}

static int lmpdconn_run_play_id(lua_State *L)
{
	int song_id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_play_id(*conn, song_id));

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

static int lmpdconn_run_stop(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_stop(*conn));

	return 1;
}

static int lmpdconn_send_toggle_pause(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_toggle_pause(*conn));

	return 1;
}

static int lmpdconn_run_toggle_pause(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_toggle_pause(*conn));

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

static int lmpdconn_run_pause(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_pause(*conn, mode));

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

static int lmpdconn_run_next(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_next(*conn));

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

static int lmpdconn_run_previous(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_previous(*conn));

	return 1;
}

static int lmpdconn_send_seek_pos(lua_State *L)
{
	int song_pos, time;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);
	time = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_seek_pos(*conn, song_pos, time));

	return 1;
}

static int lmpdconn_run_seek_pos(lua_State *L)
{
	int song_pos, time;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	song_pos = luaL_checkinteger(L, 2);
	time = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_seek_pos(*conn, song_pos, time));

	return 1;
}

static int lmpdconn_send_seek_id(lua_State *L)
{
	int id, time;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);
	time = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_seek_id(*conn, id, time));

	return 1;
}

static int lmpdconn_run_seek_id(lua_State *L)
{
	int id, time;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);
	time = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_seek_id(*conn, id, time));

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

static int lmpdconn_run_repeat(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_repeat(*conn, mode));

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

static int lmpdconn_run_random(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_random(*conn, mode));

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

static int lmpdconn_run_single(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_single(*conn, mode));

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

static int lmpdconn_run_consume(lua_State *L)
{
	int mode;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	mode = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_consume(*conn, mode));

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

static int lmpdconn_run_crossfade(lua_State *L)
{
	int seconds;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	seconds = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_crossfade(*conn, seconds));

	return 1;
}

/* playlist.h */
static int lmpdconn_recv_playlist(lua_State *L)
{
	struct mpd_playlist **playlist;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	playlist = (struct mpd_playlist **) lua_newuserdata(L, sizeof(struct mpd_playlist *));
	luaL_getmetatable(L, MPD_PLAYLIST_T);
	lua_setmetatable(L, -2);

	*playlist = mpd_recv_playlist(*conn);

	return 1;
}

static int lmpdconn_send_list_playlist(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_playlist(*conn, name));

	return 1;
}

static int lmpdconn_send_list_playlist_meta(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_playlist_meta(*conn, name));

	return 1;
}

static int lmpdconn_send_playlist_clear(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlist_clear(*conn, name));

	return 1;
}

static int lmpdconn_run_playlist_clear(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_playlist_clear(*conn, name));

	return 1;
}

static int lmpdconn_send_playlist_add(lua_State *L)
{
	const char *name, *path;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	path = luaL_checkstring(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlist_add(*conn, name, path));

	return 1;
}

static int lmpdconn_run_playlist_add(lua_State *L)
{
	const char *name, *path;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	path = luaL_checkstring(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_playlist_add(*conn, name, path));

	return 1;
}

static int lmpdconn_send_playlist_move(lua_State *L)
{
	int from, to;
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	from = luaL_checkinteger(L, 3);
	to = luaL_checkinteger(L, 4);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlist_move(*conn, name, from, to));

	return 1;
}

static int lmpdconn_send_playlist_delete(lua_State *L)
{
	int pos;
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	pos = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_playlist_delete(*conn, name, pos));

	return 1;
}

static int lmpdconn_run_playlist_delete(lua_State *L)
{
	int pos;
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);
	pos = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_playlist_delete(*conn, name, pos));

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

static int lmpdconn_run_save(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_save(*conn, name));

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

static int lmpdconn_run_load(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_load(*conn, name));

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

static int lmpdconn_run_rename(lua_State *L)
{
	const char *from, *to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkstring(L, 2);
	to = luaL_checkstring(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_rename(*conn, from, to));

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

static int lmpdconn_run_rm(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_rm(*conn, name));

	return 1;
}

/* queue.h */
static int lmpdconn_send_list_queue_meta(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_list_queue_meta(*conn));

	return 1;
}

static int lmpdconn_send_get_queue_song_pos(lua_State *L)
{
	int pos;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	pos = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_get_queue_song_pos(*conn, pos));

	return 1;
}

static int lmpdconn_send_get_queue_song_id(lua_State *L)
{
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_get_queue_song_id(*conn, id));

	return 1;
}

static int lmpdconn_send_queue_changes_meta(lua_State *L)
{
	double playlist;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	playlist = luaL_checknumber(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_queue_changes_meta(*conn, playlist));

	return 1;
}

static int lmpdconn_send_queue_changes_brief(lua_State *L)
{
	double playlist;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	playlist = luaL_checknumber(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_queue_changes_brief(*conn, playlist));

	return 1;
}

static int lmpdconn_recv_queue_change_brief(lua_State *L)
{
	int position;
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_recv_queue_change_brief(*conn, &position, &id));

	lua_newtable(L);

	lua_pushinteger(L, position);
	lua_setfield(L, -2, "position");

	lua_pushinteger(L, id);
	lua_setfield(L, -2, "id");

	return 2;
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

static int lmpdconn_send_add_id(lua_State *L)
{
	const char *file;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	file = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_add_id(*conn, file));

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

static int lmpdconn_run_add_id(lua_State *L)
{
	const char *file;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	file = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	lua_pushinteger(L, mpd_run_add_id(*conn, file));

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

static int lmpdconn_send_delete_id(lua_State *L)
{
	int id;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id = luaL_checkinteger(L, 2);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_delete_id(*conn, id));

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

static int lmpdconn_run_shuffle(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_shuffle(*conn));

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

static int lmpdconn_run_shuffle_range(lua_State *L)
{
	int start, end;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	start = luaL_checkinteger(L, 2);
	end = luaL_checkinteger(L, 3);

	assert(start > 0);
	assert(end > 0);
	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_shuffle_range(*conn, start, end));

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

static int lmpdconn_run_clear(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_clear(*conn));

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

static int lmpdconn_run_move(lua_State *L)
{
	int from, to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkinteger(L, 2);
	to = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_move(*conn, from, to));

	return 1;
}

static int lmpdconn_send_move_id(lua_State *L)
{
	int from, to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkinteger(L, 2);
	to = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_move_id(*conn, from, to));

	return 1;
}

static int lmpdconn_run_move_id(lua_State *L)
{
	int from, to;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	from = luaL_checkinteger(L, 2);
	to = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_move_id(*conn, from, to));

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

static int lmpdconn_run_swap(lua_State *L)
{
	int pos1, pos2;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	pos1 = luaL_checkinteger(L, 2);
	pos2 = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_swap(*conn, pos1, pos2));

	return 1;
}

static int lmpdconn_send_swap_id(lua_State *L)
{
	int id1, id2;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id1 = luaL_checkinteger(L, 2);
	id2 = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_swap_id(*conn, id1, id2));

	return 1;
}

static int lmpdconn_run_swap_id(lua_State *L)
{
	int id1, id2;
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	id1 = luaL_checkinteger(L, 2);
	id2 = luaL_checkinteger(L, 3);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_run_swap_id(*conn, id1, id2));

	return 1;
}

/* recv.h */
static int lmpdconn_recv_pair(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_pair **pair;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	pair = (struct mpd_pair **) lua_newuserdata(L, sizeof(struct mpd_pair *));
	luaL_getmetatable(L, MPD_PAIR_T);
	lua_setmetatable(L, -2);

	*pair = mpd_recv_pair(*conn);
	if (*pair == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "mpd_recv_pair");
		return 2;
	}

	return 1;
}

static int lmpdconn_recv_pair_named(lua_State *L)
{
	const char *name;
	struct mpd_connection **conn;
	struct mpd_pair **pair;

	pair = (struct mpd_pair **) lua_newuserdata(L, sizeof(struct mpd_pair *));
	luaL_getmetatable(L, MPD_PAIR_T);
	lua_setmetatable(L, -2);

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	name = luaL_checkstring(L, 2);

	assert(*conn != NULL);

	*pair = mpd_recv_pair_named(*conn, name);
	if (*pair == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "mpd_recv_pair_named");
		return 2;
	}

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
	mpd_value_free(value);

	return 1;
}

static int lmpdconn_return_pair(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_pair **pair;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	pair = luaL_checkudata(L, 2, MPD_PAIR_T);

	assert(*conn != NULL);
	assert(*pair != NULL);

	mpd_return_pair(*conn, *pair);
	lua_pushboolean(L, 1);
	return 1;
}

static int lmpdconn_enqueue_pair(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_pair **pair;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);
	pair = luaL_checkudata(L, 2, MPD_PAIR_T);

	assert(*conn != NULL);
	assert(*pair != NULL);

	mpd_enqueue_pair(*conn, *pair);
	lua_pushboolean(L, 1);
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

/* status.h */
static int lmpdconn_send_status(lua_State *L)
{
	struct mpd_connection **conn;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	lua_pushboolean(L, mpd_send_status(*conn));

	return 1;
}

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

static int lmpdconn_run_status(lua_State *L)
{
	struct mpd_connection **conn;
	struct mpd_status **status;

	conn = luaL_checkudata(L, 1, MPD_CONNECTION_T);

	assert(*conn != NULL);

	status = (struct mpd_status **) lua_newuserdata(L, sizeof(struct mpd_status *));
	luaL_getmetatable(L, MPD_STATUS_T);
	lua_setmetatable(L, -2);

	*status = mpd_run_status(*conn);
	if (*status == NULL) {
		/* Push nil and error message */
		lua_pushnil(L);
		lua_pushliteral(L, "out of memory");
		return 2;
	}


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
	{"cmp_server_version",		lmpdconn_cmp_server_version},
	/* capabilities.h */
	{"send_allowed_commands",	lmpdconn_send_allowed_commands},
	{"send_disallowed_commands",	lmpdconn_send_disallowed_commands},
	{"recv_command_pair",		lmpdconn_recv_command_pair},
	{"send_list_url_schemes",	lmpdconn_send_list_url_schemes},
	{"recv_url_scheme_pair",	lmpdconn_recv_url_scheme_pair},
	{"send_list_tag_types",		lmpdconn_send_list_tag_types},
	{"recv_tag_type_pair",		lmpdconn_recv_tag_type_pair},
	/* database.h */
	{"send_list_all",		lmpdconn_send_list_all},
	{"send_list_all_meta",		lmpdconn_send_list_all_meta},
	{"send_list_meta",		lmpdconn_send_list_meta},
	{"send_update",			lmpdconn_send_update},
	{"recv_update_id",		lmpdconn_recv_update_id},
	{"run_update",			lmpdconn_run_update},
	/* entity.h */
	{"recv_entity",			lmpdconn_recv_entity},
	/* idle.h */
	{"send_idle",			lmpdconn_send_idle},
	{"send_noidle",			lmpdconn_send_noidle},
	{"recv_idle",			lmpdconn_recv_idle},
	{"run_idle",			lmpdconn_run_idle},
	/* list.h */
	{"command_list_begin",		lmpdconn_command_list_begin},
	{"command_list_end",		lmpdconn_command_list_end},
	/* mixer.h */
	{"send_set_volume",		lmpdconn_send_set_volume},
	{"run_set_volume",		lmpdconn_run_set_volume},
	/* output.h */
	{"recv_output",			lmpdconn_recv_output},
	{"send_outputs",		lmpdconn_send_outputs},
	{"send_enable_output",		lmpdconn_send_enable_output},
	{"run_enable_output",		lmpdconn_run_enable_output},
	{"send_disable_output",		lmpdconn_send_disable_output},
	{"run_disable_output",		lmpdconn_run_disable_output},
	/* password.h */
	{"send_password",		lmpdconn_send_password},
	{"run_password",		lmpdconn_run_password},
	/* player.h */
	{"send_current_song",		lmpdconn_send_current_song},
	{"run_current_song",		lmpdconn_run_current_song},
	{"send_play",			lmpdconn_send_play},
	{"run_play",			lmpdconn_run_play},
	{"send_play_pos",		lmpdconn_send_play_pos},
	{"run_play_pos",		lmpdconn_run_play_pos},
	{"send_play_id",		lmpdconn_send_play_id},
	{"run_play_id",			lmpdconn_run_play_id},
	{"send_stop",			lmpdconn_send_stop},
	{"run_stop",			lmpdconn_run_stop},
	{"send_toggle_pause",		lmpdconn_send_toggle_pause},
	{"run_toggle_pause",		lmpdconn_run_toggle_pause},
	{"send_pause",			lmpdconn_send_pause},
	{"run_pause",			lmpdconn_run_pause},
	{"send_next",			lmpdconn_send_next},
	{"run_next",			lmpdconn_run_next},
	{"send_previous",		lmpdconn_send_previous},
	{"run_previous",		lmpdconn_run_previous},
	{"send_seek_pos",		lmpdconn_send_seek_pos},
	{"run_seek_pos",		lmpdconn_run_seek_pos},
	{"send_seek_id",		lmpdconn_send_seek_id},
	{"run_seek_id",			lmpdconn_run_seek_id},
	{"send_repeat",			lmpdconn_send_repeat},
	{"run_repeat",			lmpdconn_run_repeat},
	{"send_random",			lmpdconn_send_random},
	{"run_random",			lmpdconn_run_random},
	{"send_single",			lmpdconn_send_single},
	{"run_single",			lmpdconn_run_single},
	{"send_consume",		lmpdconn_send_consume},
	{"run_consume",			lmpdconn_run_consume},
	{"send_crossfade",		lmpdconn_send_crossfade},
	{"run_crossfade",		lmpdconn_run_crossfade},
	/* playlist.h */
	{"recv_playlist",		lmpdconn_recv_playlist},
	{"send_list_playlist",		lmpdconn_send_list_playlist},
	{"send_list_playlist_meta",	lmpdconn_send_list_playlist_meta},
	{"send_playlist_clear",		lmpdconn_send_playlist_clear},
	{"run_playlist_clear",		lmpdconn_run_playlist_clear},
	{"send_playlist_add",		lmpdconn_send_playlist_add},
	{"run_playlist_add",		lmpdconn_run_playlist_add},
	{"send_playlist_move",		lmpdconn_send_playlist_move},
	{"send_playlist_delete",	lmpdconn_send_playlist_delete},
	{"run_playlist_delete",		lmpdconn_run_playlist_delete},
	{"send_save",			lmpdconn_send_save},
	{"run_save",			lmpdconn_run_save},
	{"send_load",			lmpdconn_send_load},
	{"run_load",			lmpdconn_run_load},
	{"send_rename",			lmpdconn_send_rename},
	{"run_rename",			lmpdconn_run_rename},
	{"send_rm",			lmpdconn_send_rm},
	{"run_rm",			lmpdconn_run_rm},
	/* queue.h */
	{"send_list_queue_meta",	lmpdconn_send_list_queue_meta},
	{"send_get_queue_song_pos",	lmpdconn_send_get_queue_song_pos},
	{"send_get_queue_song_id",	lmpdconn_send_get_queue_song_id},
	{"send_queue_changes_meta",	lmpdconn_send_queue_changes_meta},
	{"send_queue_changes_brief",	lmpdconn_send_queue_changes_brief},
	{"recv_queue_change_brief",	lmpdconn_recv_queue_change_brief},
	{"send_add",			lmpdconn_send_add},
	{"send_add_id",			lmpdconn_send_add_id},
	{"recv_song_id",		lmpdconn_recv_song_id},
	{"run_add_id",			lmpdconn_run_add_id},
	{"send_delete",			lmpdconn_send_delete},
	{"send_delete_id",		lmpdconn_send_delete_id},
	{"send_shuffle",		lmpdconn_send_shuffle},
	{"run_shuffle",			lmpdconn_run_shuffle},
	{"send_shuffle_range",		lmpdconn_send_shuffle_range},
	{"run_shuffle_range",		lmpdconn_run_shuffle_range},
	{"send_clear",			lmpdconn_send_clear},
	{"run_clear",			lmpdconn_run_clear},
	{"send_move",			lmpdconn_send_move},
	{"run_move",			lmpdconn_run_move},
	{"send_move_id",		lmpdconn_send_move_id},
	{"run_move_id",			lmpdconn_run_move_id},
	{"send_swap",			lmpdconn_send_swap},
	{"run_swap",			lmpdconn_run_swap},
	{"send_swap_id",		lmpdconn_send_swap_id},
	{"run_swap_id",			lmpdconn_run_swap_id},
	/* recv.h */
	{"recv_pair",			lmpdconn_recv_pair},
	{"recv_pair_named",		lmpdconn_recv_pair_named},
	{"recv_value_named",		lmpdconn_recv_value_named},
	{"return_pair",			lmpdconn_return_pair},
	{"enqueue_pair",		lmpdconn_enqueue_pair},
	/* response.h */
	{"response_finish",		lmpdconn_response_finish},
	{"reponse_next",		lmpdconn_response_next},
	/* status.h */
	{"send_status",			lmpdconn_send_status},
	{"recv_status",			lmpdconn_recv_status},
	{"run_status",			lmpdconn_run_status},
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
}

