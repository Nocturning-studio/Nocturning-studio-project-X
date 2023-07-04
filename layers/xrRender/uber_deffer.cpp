#include "stdafx.h"
#include "uber_deffer.h"

string_path DummyPath = { 0 };

void fix_texture_name(LPSTR fn);

extern u32 ps_bump_mode;

void	uber_deffer(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	// Uber-parse
	string256 AlbedoTexName, BumpTexName, BumpDecompressionTexName, DetailTexName;
	strcpy(AlbedoTexName, *C.L_textures[0]);
	fix_texture_name(AlbedoTexName);
	ref_texture	Albedo;	Albedo.create(AlbedoTexName);
	bool HasBump = Albedo.bump_exist();

	// detect lmap
	bool HasLightMap = true;
	if (C.L_textures.size() < 3)	HasLightMap = false;
	else {
		pcstr tex = C.L_textures[2].c_str();
		if (tex[0] == 'l' && tex[1] == 'm' && tex[2] == 'a' && tex[3] == 'p')	HasLightMap = true;
		else															HasLightMap = false;
	}

	string256		PixelShaderName, VertexShaderName;
	strconcat(sizeof(VertexShaderName), VertexShaderName, "gbuffer_stage_", _vspec, HasLightMap ? "_lightmapped" : "");
	strconcat(sizeof(PixelShaderName), PixelShaderName, "gbuffer_stage_", _pspec, HasLightMap ? "_lightmapped" : "");
	strcpy_s(DetailTexName, sizeof(DetailTexName), _detail_replace ? _detail_replace : (C.detail_texture ? C.detail_texture : "ed\\ed_dummy_middlegray"));

	if (ps_bump_mode == 1)
		strcat(PixelShaderName, "_normal_mapping");
	else if (ps_bump_mode == 2)
		strcat(PixelShaderName, "_parallax_mapping");
	else if ((ps_bump_mode == 3) && (r2_advanced_pp))
		strcat(PixelShaderName, "_steep_parallax_mapping");
	else if ((ps_bump_mode == 3) && (!r2_advanced_pp))
		strcat(PixelShaderName, "_parallax_mapping");

	if (HasBump)
	{
		strcpy(BumpTexName, Albedo.bump_get().c_str());
		strconcat(sizeof(BumpDecompressionTexName), BumpDecompressionTexName, BumpTexName, "#");
	}
	else
	{
		strcpy(BumpTexName, "ed\\ed_dummy_bump");
		strcpy(BumpDecompressionTexName, "ed\\ed_dummy_bump#");
	}

	// Create shader pass name
	C.r_Pass(VertexShaderName, PixelShaderName, FALSE);

	// Samplers (Base texture)
	extern u32 ps_debug_textures;
	if (ps_debug_textures == 1)
		C.r_Sampler_tex("s_base", "ed\\debug_uv_checker");
	else if (ps_debug_textures == 2)
		C.r_Sampler_tex("s_base", "ed\\debug_white");
	else
		C.r_Sampler("s_base", AlbedoTexName, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	string256 BasePathAO;  strcpy(BasePathAO, AlbedoTexName);
	LPCSTR AoPath = strconcat(sizeof(BasePathAO), BasePathAO, BasePathAO, "_ao");
	if (FS.exist(DummyPath, "$game_textures$", AoPath, ".dds"))
		C.r_Sampler("s_baked_ao", AoPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_baked_ao", "ed\\ed_dummy_white");

	string256 BasePathEmissive;  strcpy(BasePathEmissive, AlbedoTexName);
	LPCSTR EmissivePath = strconcat(sizeof(BasePathEmissive), BasePathEmissive, BasePathEmissive, "_emissive");
	if (FS.exist(DummyPath, "$game_textures$", EmissivePath, ".dds"))
		C.r_Sampler("s_emissive", EmissivePath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_emissive", "ed\\ed_dummy_black");

	string256 BasePathMetallic;  strcpy(BasePathMetallic, AlbedoTexName);
	LPCSTR MetallicPath = strconcat(sizeof(BasePathMetallic), BasePathMetallic, BasePathMetallic, "_metallic");
	if (FS.exist(DummyPath, "$game_textures$", MetallicPath, ".dds"))
		C.r_Sampler("s_metallic", MetallicPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_metallic", "ed\\ed_dummy_black");

	string256 BasePathRoughness;  strcpy(BasePathRoughness, AlbedoTexName);
	LPCSTR RoughnessPath = strconcat(sizeof(BasePathRoughness), BasePathRoughness, BasePathRoughness, "_roughness");
	if (FS.exist(DummyPath, "$game_textures$", RoughnessPath, ".dds"))
		C.r_Sampler("s_roughness", RoughnessPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_roughness", "ed\\ed_dummy_black");

	string256 BasePathHeight;  strcpy(BasePathHeight, AlbedoTexName);
	LPCSTR HeightPath = strconcat(sizeof(BasePathHeight), BasePathHeight, BasePathHeight, "_height");
	if (FS.exist(DummyPath, "$game_textures$", HeightPath, ".dds"))
		C.r_Sampler("s_height_map", HeightPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_height_map", "ed\\ed_dummy_white");

	string256 BasePathGloss;  strcpy(BasePathGloss, AlbedoTexName);
	LPCSTR GlossPath = strconcat(sizeof(BasePathGloss), BasePathGloss, BasePathGloss, "_gloss");
	if (FS.exist(DummyPath, "$game_textures$", GlossPath, ".dds"))
		C.r_Sampler("s_gloss", GlossPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_gloss", "ed\\ed_dummy_black");

	string256 BasePathNormal;  strcpy(BasePathNormal, AlbedoTexName);
	LPCSTR NormalPath = strconcat(sizeof(BasePathNormal), BasePathNormal, BasePathNormal, "_normal");
	if (FS.exist(DummyPath, "$game_textures$", NormalPath, ".dds"))
		C.r_Sampler("s_normal_map", NormalPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_normal_map", "ed\\ed_dummy_normal");

	string256 BasePathOpacity;  strcpy(BasePathOpacity, AlbedoTexName);
	LPCSTR OpacityPath = strconcat(sizeof(BasePathOpacity), BasePathOpacity, BasePathOpacity, "_opacity");
	if (FS.exist(DummyPath, "$game_textures$", OpacityPath, ".dds"))
		C.r_Sampler("s_opacity", OpacityPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_opacity", "ed\\ed_dummy_white");

	LPCSTR BumpCorrectionPath = BumpDecompressionTexName;
	if (FS.exist(DummyPath, "$game_textures$", BumpCorrectionPath, ".dds"))
		C.r_Sampler("s_bumpX", BumpDecompressionTexName, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_bumpX", "ed\\ed_dummy_bump#", false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	LPCSTR BumpPath = BumpTexName;
	if (FS.exist(DummyPath, "$game_textures$", BumpPath, ".dds"))
		C.r_Sampler("s_bump", BumpTexName, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_bump", "ed\\ed_dummy_bump", false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	// Samplers detail texture
	C.r_Sampler("s_detail", DetailTexName, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	string256 BasePathDetailBumpX;  strcpy(BasePathDetailBumpX, DetailTexName);
	LPCSTR DetailBumpCorrectionPath = strconcat(sizeof(BasePathDetailBumpX), BasePathDetailBumpX, BasePathDetailBumpX, "_bump#");
	if (FS.exist(DummyPath, "$game_textures$", BumpCorrectionPath, ".dds"))
		C.r_Sampler("s_detail_bumpX", DetailBumpCorrectionPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_detail_bumpX", "ed\\ed_dummy_bump#", false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	string256 BasePathDetailBump;  strcpy(BasePathDetailBump, DetailTexName);
	LPCSTR DetailBumpPath = strconcat(sizeof(BasePathDetailBump), BasePathDetailBump, BasePathDetailBump, "_bump");
	if (FS.exist(DummyPath, "$game_textures$", BumpPath, ".dds"))
		C.r_Sampler("s_detail_bump", DetailBumpPath, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	else
		C.r_Sampler("s_detail_bump", "ed\\ed_dummy_bump", false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_blue_noise", "ed\\ed_blue_noise");

	if (HasLightMap)C.r_Sampler("s_hemi", C.L_textures[2], false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);

	if (!DO_NOT_FINISH)		C.r_End();
}

/*
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
		*./
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
		C.r_Sampler_tex("s_baked_ao", "vfx\\vfx_no_ao");

	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

	char* DetailAoPath = strconcat(sizeof(dt), dt, dt, "_ao");
	if (FS.exist(BakedAOPath, "$game_textures$", DetailAoPath, ".dds"))
		C.r_Sampler_tex("s_detail_baked_ao", DetailAoPath);
	else
		C.r_Sampler_tex("s_detail_baked_ao", "vfx\\vfx_no_ao");

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
*/