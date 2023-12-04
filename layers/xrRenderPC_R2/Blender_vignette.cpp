///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_vignette.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_vignette::CBlender_vignette()
{
	description.CLS = 0;
}

CBlender_vignette::~CBlender_vignette()
{
}

void CBlender_vignette::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "postprocess_stage_vignette_pass_static", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		C.r_Sampler_clf("s_vignette", "vfx\\vfx_vignette");
		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "postprocess_stage_vignette_pass_dynamic", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		C.r_Sampler_clf("s_vignette", "vfx\\vfx_vignette");
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
