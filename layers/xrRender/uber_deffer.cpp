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
	strcpy			(fname,*C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
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
	strconcat		(sizeof(vs),vs,"deffer_", _vspec, lmap?"_lmh":""	);
	strconcat		(sizeof(ps),ps,"deffer_", _pspec, lmap?"_lmh":""	);
	strcpy_s		(dt,sizeof(dt),_detail_replace?_detail_replace:( C.detail_texture?C.detail_texture:"" ) );

	if	(_aref)		{ strcat(ps,"_aref");	}

	if	(!bump)		{
		fnameA[0] = fnameB[0] = 0;
		strcat			(vs,"_flat");
		strcat			(ps,"_flat");
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump)) {
			strcat(vs, "_d");
			if (C.bDetail_Bump)
				strcat(ps, "_d_db");	//	bump & detail & hq
			else
				strcat(ps, "_d");
		}
	} else {
		strcpy			(fnameA,_t.bump_get().c_str());
		strconcat		(sizeof(fnameB),fnameB,fnameA,"#");
		/*
		// KD: forming bump name if detail bump needed
		if (C.bDetail_Bump)
		{
			strcpy_s		(dtA,dt);
			strconcat		(sizeof(dtA),dtA,dtA,"_bump");
			strconcat		(sizeof(dtB),dtB,dtA,"#");
		} else {
			dtA[0] = dtB[0] = 0;
		}
		*/
		extern u32 ps_bump_mode;
		if ((ps_bump_mode == 1) || (r2_sun_static))
		{
			strcat(vs, "_bump");
			strcat(ps, "_bump");
		}
		else if ((ps_bump_mode == 2) || (!r2_sun_static && !r2_advanced_pp))
		{
			strcat(vs, "_parallax");
			strcat(ps, "_parallax");
		}
		if ((ps_bump_mode == 3) && (r2_advanced_pp))
		{
			strcat(vs, "_steep_parallax");
			strcat(ps, "_steep_parallax");
		}
		
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump) )	{
			strcat		(vs,"_d"	);
			if (C.bDetail_Bump) {
				extern u32 ps_tdetail_bump_mode;
				if ((ps_tdetail_bump_mode == 1) || (r2_sun_static))
				strcat(ps, "_db");	//	bump & detail & hq (bump mapping)
				else if ((ps_tdetail_bump_mode == 2) || (!r2_sun_static && !r2_advanced_pp))
				strcat(ps, "_dp");	//	bump & detail & hq (parallax mapping)
				else if ((ps_tdetail_bump_mode == 3) && (r2_advanced_pp))
				strcat(ps, "_ds");	//	bump & detail & hq (steep parallax mapping)
			}
			else
				strcat(ps,"_d"		);
		}
		
	}

	if (C.bDetail_Bump)
	{
		strcpy_s(dtA, dt);
		strconcat(sizeof(dtA), dtA, dtA, "_bump");
		strconcat(sizeof(dtB), dtB, dtA, "#");
	}
	else {
		dtA[0] = dtB[0] = 0;
	}

	// HQ
	if (bump && hq)		{
		strcat			(vs,"-hq");
		strcat			(ps,"-hq");
	}

	extern u32 ps_debug_textures;

	// Uber-construct
	C.r_Pass		(vs,ps,	FALSE);

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
		C.r_Sampler_tex("s_baked_ao", "vfx\\vfx_dummy_white");

#ifdef ADVANCED_BUILD
	char* EmissivePath = strconcat(sizeof(fname), fname, fname, "_emissive");
	string_path EmissiveTexPath = { 0 };
	if (FS.exist(EmissiveTexPath, "$game_textures$", EmissivePath, ".dds"))
		C.r_Sampler_tex("s_emissive", EmissivePath);
	else
		C.r_Sampler_tex("s_emissive", "vfx\\vfx_dummy_black");
#endif

	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	if (C.bDetail_Bump) {
		C.r_Sampler		("s_detailBump",		dtA,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_detailBumpX",		dtB,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
#ifdef ADVANCED_BUILD
		C.r_Sampler		("s_detail_spec",		strconcat(sizeof(dt), dt, dt, "_spec"),		false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
#endif
	}
	if (lmap)C.r_Sampler("s_hemi",	C.L_textures[2],	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);

#ifdef ADVANCED_BUILD
	if (bump)
	C.r_Sampler("s_spec",		strconcat(sizeof(fname), fname, fname, "_spec"),		false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC); // <- Very big thanks for LVutner (albedo_spec.dds)
#endif

	RImplementation.clearAllShaderOptions();

	if (!DO_NOT_FINISH)		C.r_End	();
}

#pragma todo("Deathman to ALL: UGLIOUS TEMPORAL SOLUTION for swiching terrain relief mode. WE ARE MUST REWRITE IN FUTURE")
void	uber_deffer_implicit(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	//RImplementation.addShaderOption("TEST_DEFINE", "1");

	// Uber-parse
	string256		fname, fnameA, fnameB;
	strcpy(fname, *C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create(fname);

	// detect lmap
	bool			lmap = true;
	if (C.L_textures.size() < 3)
	{
		lmap = false;
	}
	else 
	{
		pcstr		tex = C.L_textures[2].c_str();
		if (tex[0] == 'l' && tex[1] == 'm' && tex[2] == 'a' && tex[3] == 'p')	
			lmap = true;
		else 
			lmap = false;
	}

	string256		ps, vs, dt;
	strconcat(sizeof(vs), vs, "deffer_", _vspec, lmap ? "_lmh" : "");
	strconcat(sizeof(ps), ps, "deffer_", _pspec, lmap ? "_lmh" : "");
	strcpy_s(dt, sizeof(dt), _detail_replace ? _detail_replace : (C.detail_texture ? C.detail_texture : ""));

	fnameA[0] = fnameB[0] = 0;
	strcat(vs, "_flat");
	strcat(ps, "_flat");

	extern u32 ps_terrain_bump_mode;

	if (C.bDetail_Diffuse) {
		strcat(vs, "_d");
		if ((ps_terrain_bump_mode == 1) || (r2_sun_static))
			strcat(ps, "_d_db");	//	bump mapping & detail
		else if ((ps_terrain_bump_mode == 2) || (!r2_sun_static && !r2_advanced_pp))
			strcat(ps, "_d_dp");	//	parallax mapping & detail
		else if ((ps_terrain_bump_mode == 3) && (r2_advanced_pp))
			strcat(ps, "_d_ds");	//	steep parallax & detail
		}

	extern u32 ps_debug_textures;

	// Uber-construct
	C.r_Pass(vs, ps, FALSE);

	if (ps_debug_textures == 2)
		C.r_Sampler_tex("s_base", "ed\\debug_white");
	else
		C.r_Sampler("s_base", C.L_textures[0], false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detail", dt, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	if (lmap)C.r_Sampler("s_hemi", C.L_textures[2], false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);

	RImplementation.clearAllShaderOptions();

	if (!DO_NOT_FINISH)		C.r_End();
}

void	uber_deffer_model(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	//RImplementation.addShaderOption("TEST_DEFINE", "1");

	// Uber-parse
	string256		fname, fnameA, fnameB;
	strcpy(fname, *C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create(fname);
	bool			bump = _t.bump_exist();

	// detect lmap
	bool			lmap = true;
	if (C.L_textures.size() < 3)	lmap = false;
	else {
		pcstr		tex = C.L_textures[2].c_str();
		if (tex[0] == 'l' && tex[1] == 'm' && tex[2] == 'a' && tex[3] == 'p')	lmap = true;
		else															lmap = false;
	}


	string256		ps, vs, dt, dtA, dtB;
	strconcat(sizeof(vs), vs, "deffer_", _vspec, lmap ? "_lmh" : "");
	strconcat(sizeof(ps), ps, "deffer_", _pspec, lmap ? "_lmh" : "");
	strcpy_s(dt, sizeof(dt), _detail_replace ? _detail_replace : (C.detail_texture ? C.detail_texture : ""));

	if (_aref) { strcat(ps, "_aref"); }

	if (!bump) {
		fnameA[0] = fnameB[0] = 0;
		strcat(vs, "_flat");
		strcat(ps, "_flat");
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump)) {
			strcat(vs, "_d");
			if (C.bDetail_Bump)
				strcat(ps, "_d_db");	//	bump & detail & hq
			else
				strcat(ps, "_d");
		}
	}
	else {
		strcpy(fnameA, _t.bump_get().c_str());
		strconcat(sizeof(fnameB), fnameB, fnameA, "#");

		extern u32 ps_bump_mode;
		if ((ps_bump_mode == 1) || (r2_sun_static))
		{
			strcat(vs, "_bump");
			strcat(ps, "_bump");
		}
		else if ((ps_bump_mode >= 2) || (!r2_sun_static && !r2_advanced_pp))
		{
			strcat(vs, "_parallax");
			strcat(ps, "_parallax");
		}

		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump)) {
			strcat(vs, "_d");
			if (C.bDetail_Bump) {
				extern u32 ps_tdetail_bump_mode;
				if ((ps_tdetail_bump_mode == 1) || (r2_sun_static))
					strcat(ps, "_db");	//	bump & detail & hq (bump mapping)
				else if ((ps_tdetail_bump_mode >= 2) || (!r2_sun_static && !r2_advanced_pp))
					strcat(ps, "_dp");	//	bump & detail & hq (parallax mapping)
			}
			else
				strcat(ps, "_d");
		}

	}

	if (C.bDetail_Bump)
	{
		strcpy_s(dtA, dt);
		strconcat(sizeof(dtA), dtA, dtA, "_bump");
		strconcat(sizeof(dtB), dtB, dtA, "#");
	}
	else {
		dtA[0] = dtB[0] = 0;
	}

	// HQ
	if (bump && hq) {
		strcat(vs, "-hq");
		strcat(ps, "-hq");
	}

	extern u32 ps_debug_textures;

	// Uber-construct
	C.r_Pass(vs, ps, FALSE);

	if (ps_debug_textures == 1)
		C.r_Sampler_tex("s_base", "ed\\debug_uv_checker");
	else if (ps_debug_textures == 2)
		C.r_Sampler_tex("s_base", "ed\\debug_white");
	else
		C.r_Sampler("s_base", C.L_textures[0], false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);


	char* AoPath = strconcat(sizeof(fname), fname, fname, "_ao");
	string_path BakedAOPath = { 0 };
	if (FS.exist(BakedAOPath, "$game_textures$", AoPath, ".dds"))
		C.r_Sampler_tex("s_baked_ao", AoPath);
	else
		C.r_Sampler_tex("s_baked_ao", "vfx\\vfx_no_ao");

	C.r_Sampler("s_bumpX", fnameB, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler("s_bump", fnameA, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detail", dt, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	if (C.bDetail_Bump) {
		C.r_Sampler("s_detailBump", dtA, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
		C.r_Sampler("s_detailBumpX", dtB, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
#ifdef ADVANCED_BUILD
		C.r_Sampler("s_detail_spec", strconcat(sizeof(dt), dt, dt, "_spec"), false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
#endif
	}
	if (lmap)C.r_Sampler("s_hemi", C.L_textures[2], false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);

#ifdef ADVANCED_BUILD
	if (bump)
		C.r_Sampler("s_spec", strconcat(sizeof(fname), fname, fname, "_spec"), false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC); // <- Very big thanks for LVutner (albedo_spec.dds)
#endif

	RImplementation.clearAllShaderOptions();

	if (!DO_NOT_FINISH)		C.r_End();
}