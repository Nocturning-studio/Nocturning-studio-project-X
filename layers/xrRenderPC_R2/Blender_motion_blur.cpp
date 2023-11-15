///////////////////////////////////////////////////////////////////////////////////
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_motion_blur.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_motion_blur::CBlender_motion_blur()
{
	description.CLS = 0;
}

CBlender_motion_blur::~CBlender_motion_blur()
{
}

void CBlender_motion_blur::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "motion_blur_phase_combine", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		C.r_Sampler_rtf("s_previous_image", r2_RT_mblur_saved_frame);
		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "motion_blur_phase_save_frame", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
