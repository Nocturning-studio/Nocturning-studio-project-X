#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_direct.h"

CBlender_accum_direct::CBlender_accum_direct()
{
	description.CLS = 0;
}
CBlender_accum_direct::~CBlender_accum_direct()
{
}

void CBlender_accum_direct::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case SE_SUN_NEAR: // near pass - enable Z-test to perform depth-clipping
	case SE_SUN_MIDDLE:
		C.r_Pass("null", "accum_sun_near", false, TRUE, FALSE, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
		C.PassSET_ZB(TRUE, FALSE, TRUE); // force inverted Z-Buffer
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_albedo", r2_RT_GBuffer_Albedo);
		C.r_Sampler_clw("s_material", r2_material);
		C.r_Sampler_rtf("s_accumulator", r2_RT_accum);
		C.r_Sampler("s_lmap", r2_sunmask);
		C.r_Sampler_clf("s_smap", r2_RT_smap_depth);
		jitter(C);
		C.r_End();
		break;
	case SE_SUN_FAR: // far pass, only stencil clipping performed
		C.r_Pass("null", "accum_sun_far", false, TRUE, FALSE, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_albedo", r2_RT_GBuffer_Albedo);
		C.r_Sampler_clw("s_material", r2_material);
		C.r_Sampler_rtf("s_accumulator", r2_RT_accum);
		C.r_Sampler("s_lmap", r2_sunmask);
		C.r_Sampler_clf("s_smap", r2_RT_smap_depth);
		jitter(C);
		C.r_End();
		break;
	}
}
