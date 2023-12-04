///////////////////////////////////////////////////////////////////////////////////
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_depth_of_field.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_depth_of_field::CBlender_depth_of_field()
{
	description.CLS = 0;
}

CBlender_depth_of_field::~CBlender_depth_of_field()
{
}

void CBlender_depth_of_field::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "postprocess_stage_depth_of_field", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
