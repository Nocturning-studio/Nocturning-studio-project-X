#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_occq.h"

CBlender_light_occq::CBlender_light_occq()
{
	description.CLS = 0;
}
CBlender_light_occq::~CBlender_light_occq()
{
}

void CBlender_light_occq::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0: // occlusion testing
		C.r_Pass("light_accumulating_stage_occlusion_culling", "light_accumulating_stage_occlusion_culling", false, FALSE, FALSE, FALSE);
		C.r_End();
		break;
	}
}
