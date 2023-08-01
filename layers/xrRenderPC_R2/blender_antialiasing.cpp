#include "stdafx.h"
#pragma hdrstop

#include "blender_antialiasing.h"
#include "r2_types.h"

CBlender_antialiasing::CBlender_antialiasing()
{
	description.CLS = 0;
}

CBlender_antialiasing::~CBlender_antialiasing()
{
}

void CBlender_antialiasing::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "antialiasing_stage_none_aa", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_image", r2_RT_albedo);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "antialiasing_stage_dlaa", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_image", r2_RT_albedo);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "antialiasing_stage_dlaa_edge_detect", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_rtf("s_normal", r2_RT_N);
		C.r_Sampler_clf("s_image", r2_RT_albedo);
		C.r_End();
		break;
	case 3:
		C.r_Pass("null", "antialiasing_stage_fxaa", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_image", r2_RT_albedo);
		C.r_End();
		break;
	}
}
