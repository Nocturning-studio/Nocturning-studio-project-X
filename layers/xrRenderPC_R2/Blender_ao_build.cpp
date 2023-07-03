#include "stdafx.h"
#pragma hdrstop

#include "blender_ao_build.h"
#include "r2_types.h"

CBlender_ao_build::CBlender_ao_build()
{
	description.CLS = 0;
}

CBlender_ao_build::~CBlender_ao_build()
{
}

void CBlender_ao_build::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0: //Read secondary RT and blur AO
		C.r_Pass("null", "ao_stage_position_downsample", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_position", r2_RT_P);
		C.r_End();
		break;
	case 1: // combine
		C.r_Pass("null", "ao_stage_build", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_clf("s_position_blurred", r2_RT_blurred_position);
		C.r_Sampler_rtf("s_normal", r2_RT_N);
		C.r_Sampler_tex("s_blue_noise", "noise\\blue_noise_texture");
		jitter(C);
		C.r_End();
		break;
	case 2: //Read previous RT and blur AO
		C.r_Pass("null", "ao_stage_filter_pass_1", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao1);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_clf("s_position_blurred", r2_RT_blurred_position);
		C.r_End();
		break;
	case 3: //Read secondary RT and blur AO
		C.r_Pass("null", "ao_stage_filter_pass_2", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao_blurred", r2_RT_ao2);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_clf("s_position_blurred", r2_RT_blurred_position);
		C.r_End();
		break;
	case 4: //Read secondary RT and blur AO
		C.r_Pass("null", "ao_stage_filter_pass_3", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao_blurred1", r2_RT_ao3);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_clf("s_position_blurred", r2_RT_blurred_position);
		C.r_End();
		break;
	}
}
