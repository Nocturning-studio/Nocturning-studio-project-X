///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_chromatic_abberation.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_chromatic_abberation::CBlender_chromatic_abberation()
{
	description.CLS = 0;
}

CBlender_chromatic_abberation::~CBlender_chromatic_abberation()
{
}

void CBlender_chromatic_abberation::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "postprocess_stage_chromatic_abberation", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
