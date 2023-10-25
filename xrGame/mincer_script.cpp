#include "mincer.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CMincer::script_register(lua_State *L)
{
    module(L)[class_<CMincer, CGameObject>("CMincer").def(constructor<>())];
}
