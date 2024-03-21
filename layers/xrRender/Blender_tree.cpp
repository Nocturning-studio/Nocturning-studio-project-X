// Blender_Vertex_aref.cpp: implementation of the CBlender_Tree class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "Blender_tree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Tree::CBlender_Tree()
{
	description.CLS = B_TREE;
	description.version = 1;
	oBlend.value = FALSE;
	oNotAnTree.value = FALSE;
}

CBlender_Tree::~CBlender_Tree()
{
}

void CBlender_Tree::Save(IWriter& fs)
{
	IBlender::Save(fs);
	xrPWRITE_PROP(fs, "Alpha-blend", xrPID_BOOL, oBlend);
	xrPWRITE_PROP(fs, "Object LOD", xrPID_BOOL, oNotAnTree);
}

void CBlender_Tree::Load(IReader& fs, u16 version)
{
	IBlender::Load(fs, version);
	xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
	if (version >= 1)
	{
		xrPREAD_PROP(fs, xrPID_BOOL, oNotAnTree);
	}
}

#if RENDER == R_R1
//////////////////////////////////////////////////////////////////////////
// R1
//////////////////////////////////////////////////////////////////////////
void CBlender_Tree::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	bool use_aref = (!oNotAnTree.value);
	C.macros.add(use_aref, "USE_AREF", "1");

	switch (C.iElement)
	{
	case SE_R1_NORMAL_HQ:
		if (oNotAnTree.value)
		{
			// Level view
			LPCSTR tsv = "tree_s", tsp = "vert";
			if (C.bDetail_Diffuse)
			{
				tsv = "tree_s_dt";
				tsp = "vert_dt";
			}
			if (oBlend.value)
				C.r_Pass(tsv, tsp, TRUE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
			else
				C.r_Pass(tsv, tsp, TRUE, TRUE, TRUE, TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler("s_detail", C.detail_texture);
			C.r_End();
		}
		else
		{
			// Level view
			if (C.bDetail_Diffuse)
			{
				if (oBlend.value)
					C.r_Pass("tree_w_dt", "vert_dt", TRUE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
				else
					C.r_Pass("tree_w_dt", "vert_dt", TRUE, TRUE, TRUE, TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_Sampler("s_detail", C.detail_texture);
				C.r_End();
			}
			else
			{
				if (oBlend.value)
					C.r_Pass("tree_w", "vert", TRUE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
				else
					C.r_Pass("tree_w", "vert", TRUE, TRUE, TRUE, TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_Sampler("s_detail", C.detail_texture);
				C.r_End();
			}
		}
		break;
	case SE_R1_NORMAL_LQ:
		// Level view
		if (oBlend.value)
			C.r_Pass("tree_s", "vert", TRUE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		else
			C.r_Pass("tree_s", "vert", TRUE, TRUE, TRUE, TRUE, D3DBLEND_ONE, D3DBLEND_ZERO);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_End();
		break;
	case SE_R1_LPOINT:
		C.r_Pass((oNotAnTree.value) ? "tree_s_point" : "tree_w_point", "add_point", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler_clf("s_lmap_l", TEX_POINT_ATT);
		C.r_Sampler_clf("s_att", TEX_POINT_ATT);
		C.r_End();
		break;
	case SE_R1_LSPOT:
		C.r_Pass((oNotAnTree.value) ? "tree_s_spot" : "tree_w_spot", "add_spot", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_Sampler_clf("s_lmap_l", "internal\\internal_light_att", true);
		C.r_Sampler_clf("s_att", TEX_SPOT_ATT);
		C.r_End();
		break;
	//case SE_R1_LMODELS:
	//	//	Don't use lighting from flora - strange visual results
	//	C.r_Pass		("tree_wave","vert_l",FALSE);
	//	C.r_Sampler		("s_base",C.L_textures[0]);
	//	C.r_End			();
	//	break;
	}
}
#else
//////////////////////////////////////////////////////////////////////////
// R2
//////////////////////////////////////////////////////////////////////////
#include "shader_name_generator.h"
void CBlender_Tree::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	//*************** codepath is the same, only shaders differ
	LPCSTR tvs = "multiple_object_animated";
	LPCSTR tvs_s = "shadow_direct_multiple_object_animated";
	if (oNotAnTree.value)
	{
		tvs = "multiple_object";
		tvs_s = "shadow_direct_multiple_object";
	}

	switch (C.iElement)
	{
	case SE_R2_NORMAL_HQ: // deffer
		generate_shader_name(C, true, tvs, "static_mesh", oBlend.value);
		break;
	case SE_R2_NORMAL_LQ: // deffer
		generate_shader_name(C, false, tvs, "static_mesh", oBlend.value);
		break;
	case SE_R2_SHADOW: // smap-spot
		if (oBlend.value)
			C.r_Pass(tvs_s, "shadow_direct_static_mesh_alphatest", FALSE, TRUE, TRUE, TRUE, D3DBLEND_ZERO, D3DBLEND_ONE,
					 TRUE, 200);
		else
			C.r_Pass(tvs_s, "shadow_direct_static_mesh", FALSE);
		C.r_Sampler("s_base", C.L_textures[0]);
		C.r_End();
		break;
	}
}
#endif
