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

#ifndef _LUA_GLOBALS_H_
#define _LUA_GLOBALS_H_ 1

#include <lua.h>

#define MPD_CONNECTION_T	"MpdClient.Connection"
#define MPD_DIRECTORY_T		"MpdClient.Directory"
#define MPD_ENTITY_T		"MpdClient.Entity"
#define MPD_OUTPUT_T		"MpdClient.Output"
#define MPD_SONG_T		"MpdClient.Song"
#define MPD_STATUS_T		"MpdClient.Status"
#define MPD_STORED_PLAYLIST_T	"MpdClient.StoredPlaylist"

void linit_connection(lua_State *L);
void linit_directory(lua_State *L);
void linit_entity(lua_State *L);
void linit_error(lua_State *L);
void linit_idle(lua_State *L);
void linit_output(lua_State *L);
void linit_protocol(lua_State *L);
void linit_song(lua_State *L);
void linit_status(lua_State *L);
void linit_stored_playlist(lua_State *L);

/* Helper functions */
#if 0
#include <stdio.h>
static void dumpstack(lua_State *L)
{
    fprintf(stderr, "-------- Lua stack dump ---------\n");
    for(int i = lua_gettop(L); i; i--)
    {
        int t = lua_type(L, i);
        switch (t)
        {
          case LUA_TSTRING:
            fprintf(stderr, "%d: string: `%s'\n", i, lua_tostring(L, i));
            break;
          case LUA_TBOOLEAN:
            fprintf(stderr, "%d: bool:   %s\n", i, lua_toboolean(L, i) ? "true" : "false");
            break;
          case LUA_TNUMBER:
            fprintf(stderr, "%d: number: %g\n", i, lua_tonumber(L, i));
            break;
          case LUA_TNIL:
            fprintf(stderr, "%d: nil\n", i);
            break;
          default:
            fprintf(stderr, "%d: %s\t#%d\t%p\n", i, lua_typename(L, t),
                    (int) lua_objlen(L, i),
                    lua_topointer(L, i));
            break;
        }
    }
    fprintf(stderr, "------- Lua stack dump end ------\n");
}
#endif

#endif // _LUA_MPDCLIENT_H_
