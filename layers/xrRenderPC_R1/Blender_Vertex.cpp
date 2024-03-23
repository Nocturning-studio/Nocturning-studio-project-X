// Blender_Vertex.cpp: implementation of the CBlender_Vertex class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "Blender_Vertex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Vertex::CBlender_Vertex()
{
	description.CLS = B_VERT;
}

CBlender_Vertex::~CBlender_Vertex()
{
}

void CBlender_Vertex::Save(IWriter& fs)
{
	IBlender::Save(fs);
}

void CBlender_Vertex::Load(IReader& fs, u16 version)
{
	IBlender::Load(fs, version);
}

void CBlender_Vertex::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:
			// Level view
			if (C.bDetail_Diffuse)
			{
				C.r_Pass("vert_dt", "vert_dt", TRUE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_Sampler("s_detail", C.detail_texture);
				C.r_End();
			}
			else
			{
				C.r_Pass("vert", "vert", TRUE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_End();
			}
			break;
		case SE_R1_NORMAL_LQ:
			// Level view
			C.r_Pass("vert", "vert", TRUE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_End();
			break;
		case SE_R1_LPOINT:
			C.r_Pass("vert_point", "add_point", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, TRUE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler_clf("s_lmap_l", TEX_POINT_ATT);
			C.r_Sampler_clf("s_att", TEX_POINT_ATT);
			C.r_End();
			break;
		case SE_R1_LSPOT:
			C.r_Pass("vert_spot", "add_spot", FALSE, TRUE, FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, TRUE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_Sampler_clf("s_lmap_l", "internal\\internal_light_att", true);
			C.r_Sampler_clf("s_att", TEX_SPOT_ATT);
			C.r_End();
			break;
		case SE_R1_LMODELS:
			// Lighting only
			C.r_Pass("vert_l", "vert_l", FALSE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_End();
			break;
		}
}
