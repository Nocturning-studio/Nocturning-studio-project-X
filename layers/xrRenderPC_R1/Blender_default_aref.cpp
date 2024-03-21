// Blender_default_aref.cpp: implementation of the CBlender_default_aref class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "Blender_default_aref.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_default_aref::CBlender_default_aref()
{
	description.CLS = B_DEFAULT_AREF;
	description.version = 1;
	oAREF.value = 32;
	oAREF.min = 0;
	oAREF.max = 255;
	oBlend.value = FALSE;
}

CBlender_default_aref::~CBlender_default_aref()
{
}

void CBlender_default_aref::Save(IWriter& fs)
{
	IBlender::Save(fs);
	xrPWRITE_PROP(fs, "Alpha ref", xrPID_INTEGER, oAREF);
	xrPWRITE_PROP(fs, "Alpha-blend", xrPID_BOOL, oBlend);
}

void CBlender_default_aref::Load(IReader& fs, u16 version)
{
	IBlender::Load(fs, version);

	switch (version)
	{
	case 0:
		xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
		oBlend.value = FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
		xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
		break;
	}
}

void CBlender_default_aref::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	bool use_aref = (!oBlend.value);
	C.macros.add(use_aref, "USE_AREF", "1");

	if (C.L_textures.size() < 2)
		Debug.fatal(DEBUG_INFO, "Not enought textures for shader, base tex: %s", *C.L_textures[0]);

	switch (C.iElement)
	{
	case SE_R1_NORMAL_HQ: {
		LPCSTR sname = "lmap";
		if (C.bDetail_Diffuse)
			sname = "lmap_dt";
		if (oBlend.value)
			C.r_Pass(sname, sname, TRUE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		else
			C.r_Pass(sname, sname, TRUE, TRUE, TRUE, TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler("s_lmap", C.L_textures[1]);
		C.r_Sampler("s_detail", C.detail_texture);
		C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
		C.r_End();
	}
	break;
	case SE_R1_NORMAL_LQ: {
		LPCSTR sname = "lmap";
		if (oBlend.value)
			C.r_Pass(sname, sname, TRUE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		else
			C.r_Pass(sname, sname, TRUE, TRUE, TRUE, TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler("s_lmap", C.L_textures[1]);
		C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
		C.r_End();
	}
	break;
	case SE_R1_LPOINT:
		if (!oBlend.value)
		{
			C.r_Pass("lmap_point", "add_point", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler_clf("s_lmap_l", TEX_POINT_ATT);
			C.r_Sampler_clf("s_att", TEX_POINT_ATT);
			C.r_End();
		}
		break;
	case SE_R1_LSPOT:
		if (!oBlend.value)
		{
			C.r_Pass("lmap_spot", "add_spot", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler_clf("s_lmap_l", "internal\\internal_light_att", true);
			C.r_Sampler_clf("s_att", TEX_SPOT_ATT);
			C.r_End();
		}
		break;
	//case SE_R1_LMODELS:
	//	C.r_Pass("lmap_l", "lmap_l", FALSE);
	//	C.r_Sampler("s_base", C.L_textures[0]);
	//	C.r_Sampler("s_lmap", C.L_textures[1]);
	//	C.r_Sampler_clf("s_hemi", *C.L_textures[2]);
	//	C.r_End();
	//	break;
	}

}
