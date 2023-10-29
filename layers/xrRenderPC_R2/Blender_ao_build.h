#pragma once

class CBlender_ao_build : public IBlender
{
  public:
	virtual LPCSTR getComment()
	{
		return "INTERNAL: Ambient occlusion";
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

	CBlender_ao_build();
	virtual ~CBlender_ao_build();
};
