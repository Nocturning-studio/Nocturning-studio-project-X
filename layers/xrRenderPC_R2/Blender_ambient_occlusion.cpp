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
	case SE_AO_SSAO:
		// raw ao
		C.r_Pass("null", "ambient_occlusion_stage_pass_ssao", FALSE, FALSE, FALSE);
		gbuffer(C);
		C.r_End();

		// filer
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_bilinear_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao);
		C.r_End();
		break;
	case SE_AO_SSAO_PLUS:
		// raw ao
		C.r_Pass("null", "ambient_occlusion_stage_pass_ssao_plus", FALSE, FALSE, FALSE);
		gbuffer(C);
		jitter(C);
		C.r_End();

		// horizontal filer
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_horizontal_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao);
		gbuffer(C);
		C.r_End();

		// vertical filer
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_vertical_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao);
		gbuffer(C);
		C.r_End();
		break;
	case SE_AO_HBAO_PLUS:
		// raw ao
		C.r_Pass("null", "ambient_occlusion_stage_pass_hbao_plus", FALSE, FALSE, FALSE);
		gbuffer(C);
		C.r_End();

		// filer
		C.r_Pass("null", "ambient_occlusion_blurring_stage_pass_bilinear_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao);
		C.r_End();
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////
