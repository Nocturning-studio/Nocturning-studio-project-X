#include "stdafx.h"
#pragma hdrstop

#include "Blender_Model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model::CBlender_Model()
{
	description.CLS = B_MODEL;
	description.version = 1;
	oAREF.value = 32;
	oAREF.min = 0;
	oAREF.max = 255;
	oBlend.value = FALSE;
}

CBlender_Model::~CBlender_Model()
{
}

void CBlender_Model::Save(IWriter& fs)
{
	IBlender::Save(fs);
	xrPWRITE_PROP(fs, "Use alpha-channel", xrPID_BOOL, oBlend);
	xrPWRITE_PROP(fs, "Alpha ref", xrPID_INTEGER, oAREF);
}

void CBlender_Model::Load(IReader& fs, u16 version)
{
	IBlender::Load(fs, version);

	switch (version)
	{
	case 0:
		oAREF.value = 32;
		oAREF.min = 0;
		oAREF.max = 255;
		oBlend.value = FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
		xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
		break;
	}
}

void CBlender_Model::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case SE_R1_NORMAL_HQ:
		C.r_Pass("model_def_hq", "model_def_hq", TRUE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler_clf("s_lmap", "$user$projector", true);
		C.r_End();
		break;
	case SE_R1_NORMAL_LQ:
		C.r_Pass("model_def_lq", "model_def_lq", TRUE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_End();
		break;
	case SE_R1_LPOINT:
		C.r_Pass("model_def_point", "add_point", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler_clf("s_lmap", TEX_POINT_ATT);
		C.r_Sampler_clf("s_att", TEX_POINT_ATT);
		C.r_End();
		break;
	case SE_R1_LSPOT:
		C.r_Pass("model_def_spot", "add_spot", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler_clf("s_lmap", "internal\\internal_light_att", true);
		C.r_Sampler_clf("s_att", TEX_SPOT_ATT);
		C.r_End();
		break;
	//case SE_R1_LMODELS:
	//	C.r_Pass("model_def_shadow", "model_shadow", FALSE, FALSE, FALSE, TRUE, D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
	//	C.r_End();
	//	break;
	}
}
