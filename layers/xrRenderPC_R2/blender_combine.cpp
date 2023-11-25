#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_combine::CBlender_combine()
{
	description.CLS = 0;
}
CBlender_combine::~CBlender_combine()
{
}

void CBlender_combine::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0: // combine
		C.r_Pass("combine_1", "combine_1", FALSE, FALSE, FALSE, TRUE, D3DBLEND_INVSRCALPHA,
				 D3DBLEND_SRCALPHA); //. MRT-blend?
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_gbuffer_albedo", r2_RT_GBuffer_Albedo);
		C.r_Sampler_rtf("s_diffuse_accumulator", r2_RT_Diffuse_Accumulator);
		C.r_Sampler_rtf("s_specular_accumulator", r2_RT_Specular_Accumulator);
		C.r_Sampler_rtf("s_autoexposure", r2_RT_luminance_cur);
		C.r_Sampler_rtf("s_material", r2_material);
		C.r_Sampler_clw("s_ao", r2_RT_ao);
		C.r_Sampler_clf("s_ao_base", r2_RT_ao_base);
		C.r_Sampler_clf("s_ao_blurred1", r2_RT_ao_blurred1);
		C.r_Sampler_clf("s_ao_blurred2", r2_RT_ao_blurred2);
		C.r_Sampler_clf("env_s0", r2_T_envs0);
		C.r_Sampler_clf("env_s1", r2_T_envs1);
		C.r_Sampler_clf("sky_s0", r2_T_sky0);
		C.r_Sampler_clf("sky_s1", r2_T_sky1);
		C.r_Sampler_rtf("s_vollight", r2_RT_generic2);

		C.r_Sampler_tex("s_debug_mask", "ed\\debug_mask");
		C.r_Sampler_tex("s_debug_diagonal_mask", "ed\\debug_diagonal_mask");
		C.r_Sampler_tex("s_debug_triple_mask", "ed\\debug_triple_mask");
		C.r_Sampler_tex("s_debug_quadro_mask", "ed\\debug_quadro_mask");

		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "combine_2", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_image", r2_RT_generic0);
		C.r_Sampler_clf("s_bloom", r2_RT_bloom1);
		C.r_Sampler_clf("s_distort", r2_RT_generic1);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "combine_2_distorted", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_gbuffer_position", r2_RT_GBuffer_Position);
		C.r_Sampler_rtf("s_gbuffer_normal", r2_RT_GBuffer_Normal);
		C.r_Sampler_rtf("s_image", r2_RT_generic0);
		C.r_Sampler_clf("s_bloom", r2_RT_bloom1);
		C.r_Sampler_clf("s_distort", r2_RT_generic1);
		C.r_End();
		break;
	}
}
