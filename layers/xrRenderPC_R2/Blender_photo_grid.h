#pragma once

class CBlender_photo_grid : public IBlender
{
public:
	virtual LPCSTR getComment()
	{
		return "INTERNAL: Depth of field";
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

	CBlender_photo_grid();
	virtual ~CBlender_photo_grid();
};
