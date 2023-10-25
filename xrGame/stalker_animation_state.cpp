////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_state.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state animations
////////////////////////////////////////////////////////////////////////////

#include "stalker_animation_state.h"
#include "../xrEngine/skeletonanimated.h"
#include "object_broker.h"
#include "stdafx.h"

CStalkerAnimationState::CStalkerAnimationState()
{
    m_in_place = xr_new<IN_PLACE_ANIMATIONS>();
}

CStalkerAnimationState::CStalkerAnimationState(const CStalkerAnimationState &stalker_animation_state)
{
    clone(stalker_animation_state.m_in_place, m_in_place);
}

CStalkerAnimationState::~CStalkerAnimationState()
{
    xr_delete(m_in_place);
}

void CStalkerAnimationState::Load(CKinematicsAnimated *kinematics, LPCSTR base_name)
{
    string256 S;
    m_global.Load(kinematics, base_name);
    m_torso.Load(kinematics, strconcat(sizeof(S), S, base_name, "torso_"));
    m_movement.Load(kinematics, base_name);
    m_in_place->Load(kinematics, base_name);
}
