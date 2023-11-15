///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_antialiasing.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
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
		C.r_Pass("null", "antialiasing_stage_dlaa", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "antialiasing_stage_dlaa_edge_detect", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "antialiasing_stage_fxaa", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
