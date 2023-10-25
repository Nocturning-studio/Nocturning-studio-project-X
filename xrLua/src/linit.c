/*
** $Id: linit.c,v 1.13 2005/08/26 17:36:32 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/

#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#ifndef COCO_DISABLE
#include "lcoco.h"
#endif

static const luaL_Reg lualibs[] = {{"", luaopen_base},
                                   {LUA_TABLIBNAME, luaopen_table},
                                   {LUA_IOLIBNAME, luaopen_io},
                                   {LUA_OSLIBNAME, luaopen_os},
                                   {LUA_STRLIBNAME, luaopen_string},
                                   {LUA_MATHLIBNAME, luaopen_math},
                                   {LUA_DBLIBNAME, luaopen_debug},
                                   {LUA_LOADLIBNAME, luaopen_package},
                                   {LUA_JITLIBNAME, luaopen_jit},
#ifndef COCO_DISABLE
                                   {LUA_COLIBNAME, luaopen_coco},
#endif
                                   {NULL, NULL}};

LUALIB_API void luaL_openlibs(lua_State *L)
{
    const luaL_Reg *lib = lualibs;
    for (; lib->func; lib++)
    {
        lua_pushcfunction(L, lib->func);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
    }
}
