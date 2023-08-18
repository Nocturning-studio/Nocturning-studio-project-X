#include "stdafx.h"
#pragma hdrstop

#include "blender_photo_grid.h"
#include "r2_types.h"

CBlender_photo_grid::CBlender_photo_grid()
{
	description.CLS = 0;
}

CBlender_photo_grid::~CBlender_photo_grid()
{
}

void CBlender_photo_grid::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	switch (C.iElement)
	{
	case 0:
		C.r_Pass("null", "photo_grid", FALSE, FALSE, FALSE);
		C.r_Sampler_rtf("s_image", r2_RT_albedo);
		C.r_Sampler_tex("s_photo_grid", "vfx\\vfx_photo_grid");
		C.r_End();
		break;
	}
}
