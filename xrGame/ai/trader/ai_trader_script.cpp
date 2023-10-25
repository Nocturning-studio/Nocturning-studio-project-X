#include "ai_trader.h"
#include "pch_script.h"

using namespace luabind;

#pragma optimize("s", on)
void CAI_Trader::script_register(lua_State *L)
{
    module(L)[class_<CAI_Trader, CGameObject>("CAI_Trader").def(constructor<>())];
}
