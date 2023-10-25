#include "WeaponRPG7.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CWeaponRPG7::script_register(lua_State *L)
{
    module(L)[class_<CWeaponRPG7, CGameObject>("CWeaponRPG7").def(constructor<>())];
}
