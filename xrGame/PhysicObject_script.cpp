#include "PhysicObject.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CPhysicObject::script_register(lua_State *L)
{
    module(L)[class_<CPhysicObject, CGameObject>("CPhysicObject").def(constructor<>())];
}
