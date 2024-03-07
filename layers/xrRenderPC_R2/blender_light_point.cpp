#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point.h"

CBlender_accum_point::CBlender_accum_point()
{
	description.CLS = 0;
}
CBlender_accum_point::~CBlender_accum_point()
{
}

void CBlender_accum_point::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case SE_L_FILL: // fill projective
		C.r_Pass("null", "copy", false, FALSE, FALSE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_End();
		break;
	case SE_L_UNSHADOWED: // unshadowed
		C.r_Pass("accumulating_light_stage_volume", "accumulating_light_stage_point", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		gbuffer(C);
		C.r_Sampler_clw("s_material", r2_material);
		C.r_Sampler_clf("s_lmap", *C.L_textures[0]);
		C.r_Sampler_clf("s_brdf_lut", "vfx\\vfx_brdf_lut");
		C.r_End();
		break;
	case SE_L_NORMAL: // normal
		C.sh_macro("USE_SHADOW_MAPPING", "1");
		C.r_Pass("accumulating_light_stage_volume", "accumulating_light_stage_point", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		gbuffer(C);
		C.r_Sampler_clw("s_material", r2_material);
		C.r_Sampler("s_lmap", C.L_textures[0]);
		C.r_Sampler_clf("s_smap", r2_RT_smap_depth);
		C.r_Sampler_clf("s_brdf_lut", "vfx\\vfx_brdf_lut");
		jitter(C);
		C.r_End();
		break;
	case SE_L_FULLSIZE: // normal-fullsize
		C.sh_macro("USE_SHADOW_MAPPING", "1");
		C.r_Pass("accumulating_light_stage_volume", "accumulating_light_stage_point", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		gbuffer(C);
		C.r_Sampler_clw("s_material", r2_material);
		C.r_Sampler("s_lmap", C.L_textures[0]);
		C.r_Sampler_clf("s_smap", r2_RT_smap_depth);
		C.r_Sampler_clf("s_brdf_lut", "vfx\\vfx_brdf_lut");
		jitter(C);
		C.r_End();
		break;
	case SE_L_TRANSLUENT: // shadowed + transluency
		C.sh_macro("USE_SHADOW_MAPPING", "1");
		C.sh_macro("USE_LIGHT_MAPPING", "1");
		C.r_Pass("accumulating_light_stage_volume", "accumulating_light_stage_point", false, FALSE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		gbuffer(C);
		C.r_Sampler_clw("s_material", r2_material);
		C.r_Sampler_clf("s_lmap", r2_RT_smap_surf); // diff here
		C.r_Sampler_clf("s_smap", r2_RT_smap_depth);
		C.r_Sampler_clf("s_brdf_lut", "vfx\\vfx_brdf_lut");
		jitter(C);
		C.r_End();
		break;
	}
}
