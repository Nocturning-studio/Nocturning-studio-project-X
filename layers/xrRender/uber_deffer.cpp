#include "stdafx.h"
#include "uber_deffer.h"

extern ENGINE_API BOOL r2_sun_static;
extern ENGINE_API BOOL r2_advanced_pp;

void fix_texture_name(LPSTR fn);

void	uber_deffer	(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	//RImplementation.addShaderOption("TEST_DEFINE", "1");

	// Uber-parse
	string256		fname, fnameA, fnameB;
	strcpy			(fname,*C.L_textures[0]);
	fix_texture_name(fname);
	ref_texture		_t;		_t.create			(fname);
	bool			bump	= _t.bump_exist		();

	// detect lmap
	bool			lmap	= true;
	if	(C.L_textures.size()<3)	lmap = false;
	else {
		pcstr		tex		= C.L_textures[2].c_str();
		if (tex[0]=='l' && tex[1]=='m' && tex[2]=='a' && tex[3]=='p')	lmap = true	;
		else															lmap = false;
	}

	string256		ps,vs,dt,dtA,dtB;
	strconcat		(sizeof(vs), vs, "deffer_", _vspec, "");
	strconcat		(sizeof(ps),ps,"deffer_", _pspec, lmap?"_lmh":""	);

	strcpy_s		(dt, sizeof(dt), C.detail_texture ? C.detail_texture : "ed\\ed_dummy_detail");

	if	(_aref)		{ strcat(ps,"_aref");	}

	// HQ
//	if (!hq)		
// {
//		strcat			(vs,"-lq");
//		strcat			(ps,"-lq");
//	}

	if (bump)
		strcpy			(fnameA,_t.bump_get().c_str());
	else
		strcpy			(fnameA, "ed\\ed_dummy_bump");
		strconcat		(sizeof(fnameB),fnameB,fnameA,"#");

	extern u32 ps_bump_mode;
	if (((ps_bump_mode == 1) || (r2_sun_static)) && hq)
		strcat(ps, "_normal");
	else if (((ps_bump_mode == 2) || (!r2_sun_static && !r2_advanced_pp)) && hq)
		strcat(ps, "_parallax");
	if (((ps_bump_mode == 3) && (r2_advanced_pp)) && hq)
		strcat(ps, "_steep_parallax");

	if (C.bDetail && C.bDetail_Diffuse)
		strcat(ps, "_d");

	strcpy_s(dtA, dt);
	strconcat(sizeof(dtA), dtA, dtA, "_bump");
	strconcat(sizeof(dtB), dtB, dtA, "#");

	// Uber-construct
	C.r_Pass		(vs,ps,	FALSE);

	extern u32 ps_debug_textures;
	if (ps_debug_textures == 1)
	C.r_Sampler_tex("s_base", "ed\\debug_uv_checker");
	else if (ps_debug_textures == 2)
	C.r_Sampler_tex("s_base", "ed\\debug_white");
	else
	C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	char* AoPath = strconcat(sizeof(fname), fname, fname, "_ao");
	string_path BakedAOPath = { 0 }; 
	if (FS.exist(BakedAOPath, "$game_textures$", AoPath, ".dds"))
		C.r_Sampler_tex("s_baked_ao", AoPath);
	else
		C.r_Sampler_tex("s_baked_ao", "vfx\\vfx_no_ao");

	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	C.r_Sampler		("s_detailBump",		dtA,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_detailBumpX",		dtB,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	if (lmap)C.r_Sampler("s_hemi",	C.L_textures[2],	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);

	C.r_Sampler_tex("s_blue_noise", "noise\\blue_noise_texture");
	C.r_Sampler_tex("s_perlin_noise", "noise\\perlin_noise_texture");

	RImplementation.clearAllShaderOptions();

	if (!DO_NOT_FINISH)		C.r_End	();
}
