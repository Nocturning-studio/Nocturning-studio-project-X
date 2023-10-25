#include "WeaponHPSA.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CWeaponHPSA::script_register(lua_State *L)
{
    module(L)[class_<CWeaponHPSA, CGameObject>("CWeaponHPSA").def(constructor<>())];
}
