#include "WeaponShotgun.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CWeaponShotgun::script_register(lua_State *L)
{
    module(L)[class_<CWeaponShotgun, CGameObject>("CWeaponShotgun").def(constructor<>())];
}
