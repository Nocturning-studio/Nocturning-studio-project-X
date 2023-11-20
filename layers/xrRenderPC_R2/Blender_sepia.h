///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////////////////////////
class CBlender_sepia : public IBlender
{
  public:
	virtual LPCSTR getComment()
	{
		return "INTERNAL: Chromatic abberation";
	}

	virtual BOOL canBeDetailed()
	{
		return FALSE;
	}

	virtual BOOL canBeLMAPped()
	{
		return FALSE;
	}

	virtual void Compile(CBlender_Compile& C);

	CBlender_sepia();
	virtual ~CBlender_sepia();
};
///////////////////////////////////////////////////////////////////////////////////