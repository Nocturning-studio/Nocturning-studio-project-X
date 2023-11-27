#include "stdafx.h"
#pragma hdrstop

#include "Blender_autoexposure.h"

CBlender_autoexposure::CBlender_autoexposure()
{
	description.CLS = 0;
}
CBlender_autoexposure::~CBlender_autoexposure()
{
}

void CBlender_autoexposure::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "postprocess_stage_autoexposure_1", false, FALSE, FALSE, FALSE);
		C.r_Sampler_gaussian("s_image", r2_RT_generic0);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "postprocess_stage_autoexposure_2", false, FALSE, FALSE, FALSE);
		C.r_Sampler_gaussian("s_image", r2_RT_autoexposure_t64);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "postprocess_stage_autoexposure_3", false, FALSE, FALSE, FALSE);
		C.r_Sampler_gaussian("s_image", r2_RT_autoexposure_t8);
		C.r_Sampler_gaussian("s_autoexposure", r2_RT_autoexposure_src);
		C.r_End();
		break;
	}
}
