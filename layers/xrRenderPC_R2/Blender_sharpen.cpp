///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
///////////////////////////////////////////////////////////////////////////////////
#include "blender_sharpen.h"
#include "r2_types.h"
///////////////////////////////////////////////////////////////////////////////////
CBlender_sharpen::CBlender_sharpen()
{
	description.CLS = 0;
}

CBlender_sharpen::~CBlender_sharpen()
{
}

void CBlender_sharpen::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "postprocess_stage_sharpen", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
