///////////////////////////////////////////////////////////////////////////////////
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_ambient_occlusion_blur.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_ambient_occlusion_blur::CBlender_ambient_occlusion_blur()
{
	description.CLS = 0;
}

CBlender_ambient_occlusion_blur::~CBlender_ambient_occlusion_blur()
{
}

void CBlender_ambient_occlusion_blur::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_vertical_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_base);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_horizontal_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_ao", r2_RT_ao_blurred1);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_vertical_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_blurred2);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 3:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_horizontal_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_ao", r2_RT_ao_blurred1);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	case 4:
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_upscale", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_blurred2);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
