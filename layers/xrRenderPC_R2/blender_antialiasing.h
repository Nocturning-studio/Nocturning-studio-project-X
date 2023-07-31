#pragma once

class CBlender_antialiasing : public IBlender
{
public:
	virtual LPCSTR getComment()
	{
		return "INTERNAL: Anti-Aliasing";
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
