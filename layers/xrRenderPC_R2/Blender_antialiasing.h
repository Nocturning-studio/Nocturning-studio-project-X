///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////////////////////////
class CBlender_antialiasing : public IBlender
{
  public:
	virtual LPCSTR getComment()
	{
		return "INTERNAL: Antialiasing";
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

	CBlender_antialiasing();
	virtual ~CBlender_antialiasing();
};
///////////////////////////////////////////////////////////////////////////////////
