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
		C.r_Pass("null", "ambient_occlusion_stage_pass_hdao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "ambient_occlusion_stage_pass_hbao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 3:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_filter_step_1", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_base);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 4:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_filter_step_2", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_blurred1);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 5:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_finalize", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_blurred2);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
