#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_reflected.h"

CBlender_accum_reflected::CBlender_accum_reflected()
{
	description.CLS = 0;
}
CBlender_accum_reflected::~CBlender_accum_reflected()
{
}

void CBlender_accum_reflected::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	C.r_Pass("accum_volume", "accum_indirect", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
	C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
	C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
	C.r_Sampler_clw("s_material", r2_material);
	C.r_Sampler_rtf("s_diffuse_accumulator", r2_RT_Diffuse_Accumulator);
	C.r_Sampler_rtf("s_specular_accumulator", r2_RT_Specular_Accumulator);
	C.r_End();
}
