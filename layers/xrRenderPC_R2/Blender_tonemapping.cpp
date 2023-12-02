////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2023
//	Author		: Deathman
//  Nocturning studio for NS Project X
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop
////////////////////////////////////////////////////////////////////////////
#include "blender_tonemapping.h"
#include "r2_types.h"
////////////////////////////////////////////////////////////////////////////
CBlender_tonemapping::CBlender_tonemapping()
{
	description.CLS = 0;
}

CBlender_tonemapping::~CBlender_tonemapping()
{
}

void CBlender_tonemapping::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "postprocess_stage_tonemapping", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_GBuffer_Albedo);
		jitter(C);
		C.r_End();
		break;
	}
}
////////////////////////////////////////////////////////////////////////////
