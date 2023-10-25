////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_brain_inline.h
//	Created 	: 06.10.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster brain class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CALifeMonsterBrain::object_type &CALifeMonsterBrain::object() const
{
    VERIFY(m_object);
    return (*m_object);
}

IC CALifeMonsterBrain::movement_manager_type &CALifeMonsterBrain::movement() const
{
    VERIFY(m_movement_manager);
    return (*m_movement_manager);
}
