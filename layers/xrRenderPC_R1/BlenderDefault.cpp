// BlenderDefault.cpp: implementation of the CBlender_default class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "BlenderDefault.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_default::CBlender_default()
{
	description.CLS = B_DEFAULT;
}

CBlender_default::~CBlender_default()
{
}
void CBlender_default::Save(IWriter& fs)
{
	IBlender::Save(fs);
}
void CBlender_default::Load(IReader& fs, u16 version)
{
	IBlender::Load(fs, version);
}
void CBlender_default::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);
	
		if (C.L_textures.size() < 3)
			Debug.fatal(DEBUG_INFO, "Not enought textures for shader, base tex: %s", *C.L_textures[0]);
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:
			// Level view
			if (C.bDetail_Diffuse)
			{
				C.r_Pass("lmap_dt", "lmap_dt", TRUE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_Sampler("s_lmap", C.L_textures[1]);
				C.r_Sampler("s_detail", C.detail_texture);
				C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
				C.r_End();
			}
			else
			{
				C.r_Pass("lmap", "lmap", TRUE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_Sampler("s_lmap", C.L_textures[1]);
				C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
				C.r_End();
			}
			break;
		case SE_R1_NORMAL_LQ:
			C.r_Pass("lmap", "lmap", TRUE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler("s_lmap", C.L_textures[1]);
			C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
			C.r_End();
			break;
		case SE_R1_LPOINT:
			C.r_Pass("lmap_point", "add_point", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, TRUE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler_clf("s_lmap_l", TEX_POINT_ATT);
			C.r_Sampler_clf("s_att", TEX_POINT_ATT);
			C.r_End();
			break;
		case SE_R1_LSPOT:
			C.r_Pass("lmap_spot", "add_spot", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, TRUE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler_clf("s_lmap_l", "internal\\internal_light_att", true);
			C.r_Sampler_clf("s_att", TEX_SPOT_ATT);
			C.r_End();
			break;
		//case SE_R1_LMODELS:
		//	// Lighting only, not use alpha-channel
		//	C.r_Pass("lmap_l", "lmap_l", FALSE);
		//	C.r_Sampler("s_base", C.L_textures[0]);
		//	C.r_Sampler("s_lmap", C.L_textures[1]);
		//	C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
		//	C.r_End();
		//	break;
		}
}
