///////////////////////////////////////////////////////////////////////////////////
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_ambient_occlusion.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_ambient_occlusion::CBlender_ambient_occlusion()
{
	description.CLS = 0;
}

CBlender_ambient_occlusion::~CBlender_ambient_occlusion()
{
}

void CBlender_ambient_occlusion::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "ambient_occlusion_stage_pass_ssao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "ambient_occlusion_stage_pass_hbao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "ambient_occlusion_stage_pass_hbao_plus", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 3:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_bilinear_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_ao", r2_RT_ao);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
