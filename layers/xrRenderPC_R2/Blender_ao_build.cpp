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
	case 0:
		C.r_Pass("null", "ao_phase_ssao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_normal", r2_RT_N);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_tex("s_blue_noise", "noise\\blue_noise_texture");
		jitter(C);
		C.r_End();
		break;
	case 1:
		C.r_Pass("null", "ao_phase_hdao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_normal", r2_RT_N);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_tex("s_blue_noise", "noise\\blue_noise_texture");
		jitter(C);
		C.r_End();
		break;
	case 2:
		C.r_Pass("null", "ao_phase_hbao", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_normal", r2_RT_N);
		C.r_Sampler_rtf("s_position", r2_RT_P);
		C.r_Sampler_tex("s_blue_noise", "noise\\blue_noise_texture");
		jitter(C);
		C.r_End();
		break;
	case 3:
		C.r_Pass("null", "ao_phase_diagonal_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_base);
		C.r_End();
		break;
	case 4:
		C.r_Pass("null", "ao_phase_strided_filter", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_blurred1);
		C.r_End();
		break;
	case 5:
		C.r_Pass("null", "ao_phase_finalize", FALSE, FALSE, FALSE);
		C.r_Sampler_clf("s_ao", r2_RT_ao_blurred2);
		C.r_End();
		break;
	}
}
