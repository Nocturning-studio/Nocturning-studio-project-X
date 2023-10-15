////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Created: 14.10.2023
// Author: Deathman
// Nocturning studio for NS Project X
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "shader_name_generator.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
#define USE_DEBUG_SHADER_NAME_GETERATION
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern ENGINE_API BOOL r2_sun_static;
extern ENGINE_API BOOL r2_advanced_pp;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern u32 ps_r2_bump_mode;
extern u32 ps_r2_debug_textures;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fix_texture_name(LPSTR fn);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generate_shader_name(CBlender_Compile& C,
						  bool bIsHightQualityGeometry,
						  LPCSTR VertexShaderName,
						  LPCSTR PixelShaderName,
						  BOOL bUseAlpha)
{
	// Output shader names
	string_path NewPixelShaderName;
	string_path NewVertexShaderName;

	// Base part of material
	string_path AlbedoTexture;
	string_path BumpTexture;
	string_path BumpCorrectionTexture;

	// Detail part of material
	string_path DetailAlbedoTexture;
	string_path DetailBumpTexture;
	string_path DetailBumpCorrectionTexture;

	// Other textures
	string_path LightMapTexture;
	string_path BakedAOTexture;

	string_path Dummy = { 0 };

	// Get  for base texture for material with debug
	switch (ps_r2_debug_textures)
	{
	case 0:
		strcpy_s(AlbedoTexture, sizeof(AlbedoTexture), *C.L_textures[0]);
		break;
	case 1:
		strcpy_s(AlbedoTexture, sizeof(AlbedoTexture), "ed\\debug_uv_checker");
		break;
	case 2:
		strcpy_s(AlbedoTexture, sizeof(AlbedoTexture), "ed\\debug_white");
		break;
	}

	// Add extension to texture  and chek for null 
	fix_texture_name(AlbedoTexture);

	// Check bump existing
	ref_texture	refAlbedoTexture;
	refAlbedoTexture.create(AlbedoTexture);
	bool bUseBump = refAlbedoTexture.bump_exist();

	// Get bump map texture
	if (bUseBump)
		strcpy_s(BumpTexture, sizeof(BumpTexture), refAlbedoTexture.bump_get().c_str());
	else
		strcpy_s(BumpTexture, sizeof(BumpTexture), "ed\\ed_dummy_bump");

	// Get bump decompression map
	strcpy_s(BumpCorrectionTexture, sizeof(BumpCorrectionTexture), BumpTexture);
	strconcat(sizeof(BumpCorrectionTexture), BumpCorrectionTexture, BumpCorrectionTexture, "#");

	// Check detail existing and allowing
	bool bDetailTextureExist = C.detail_texture;
	bool bDetailTextureAllowed = C.bDetail;
	bool bUseDetail = bDetailTextureExist && bDetailTextureAllowed;

	// Get  for base texture for material
	strcpy_s(AlbedoTexture, sizeof(AlbedoTexture), *C.L_textures[0]);

	// Get detail texture 
	if (bUseDetail)
		strcpy_s(DetailAlbedoTexture, sizeof(DetailAlbedoTexture), C.detail_texture);
	else
		strcpy_s(DetailAlbedoTexture, sizeof(DetailAlbedoTexture), "ed\\ed_dummy_detail");

	// Get bump for detail texture
	strcpy_s(DetailBumpTexture, sizeof(DetailBumpTexture), DetailAlbedoTexture);

	// Checking for existace
	if (FS.exist(Dummy, "$game_textures$", DetailBumpTexture, ".dds"))
		strconcat(sizeof(DetailBumpTexture), DetailBumpTexture, DetailBumpTexture, "_bump");
	else
		strcpy_s(DetailBumpTexture, sizeof(DetailBumpTexture), "ed\\ed_dummy_bump");

	// Get bump decompression map for detail texture
	strcpy_s(DetailBumpCorrectionTexture, sizeof(DetailBumpCorrectionTexture), DetailBumpTexture);
	strconcat(sizeof(DetailBumpCorrectionTexture), DetailBumpCorrectionTexture, DetailBumpCorrectionTexture, "#");

	// Check lightmap existing
	bool bUseLightMap;
	if (C.L_textures.size() < 3)
	{
		bUseLightMap = false;
	}
	else
	{
		pcstr LightMapTextureName = C.L_textures[2].c_str();
		if (LightMapTextureName[0] == 'l' && LightMapTextureName[1] == 'm' && LightMapTextureName[2] == 'a' && LightMapTextureName[3] == 'p')
		{
			bUseLightMap = true;
		}
		else
		{
			bUseLightMap = false;
		}
	}

	// Get LightMap texture
	if (bUseLightMap)
		strcpy_s(LightMapTexture, sizeof(LightMapTexture), *C.L_textures[2]);

	// Get BakedAO texture
	strcpy_s(BakedAOTexture, sizeof(BakedAOTexture), AlbedoTexture);
	strconcat(sizeof(BakedAOTexture), BakedAOTexture, BakedAOTexture, "_ao");
	if (!FS.exist(Dummy, "$game_textures$", BakedAOTexture, ".dds"))
		strcpy_s(BakedAOTexture, sizeof(BakedAOTexture), "vfx\\vfx_no_ao");

	// Starting generate shader name
	strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, "deffer_", PixelShaderName);
	strconcat(sizeof(NewVertexShaderName), NewVertexShaderName, "deffer_", VertexShaderName);

	// Create lightmapped shader if need
	if (bUseLightMap)
		strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_lmh");

	// Create shader with alpha testing if need
	if (bUseAlpha)
		strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_aref");

	// Create shader with normal mapping or displacement if need
	if (bIsHightQualityGeometry)
	{
		if ((ps_r2_bump_mode == 1 || r2_sun_static))
			strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_normal");
		else if ((ps_r2_bump_mode == 2 || (!r2_sun_static && !r2_advanced_pp)) || (r2_advanced_pp && !C.bSteepParallax))
			strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_parallax");
		else if (((ps_r2_bump_mode == 3) && (r2_advanced_pp)) && C.bSteepParallax)
			strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_steep_parallax");
	}

	// Create shader with deatil texture if need
	if (bUseDetail)
		strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_d");

	//RImplementation.addShaderOption("TEST_DEFINE", "1");

	// Create shader pass
	C.r_Pass(NewVertexShaderName, NewPixelShaderName, FALSE);

	C.r_Sampler("s_base", AlbedoTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_baked_ao", BakedAOTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_bumpX", BumpCorrectionTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_bump", BumpTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detail", DetailAlbedoTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detailBump", DetailBumpTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detailBumpX", DetailBumpCorrectionTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	if (bUseLightMap)
		C.r_Sampler("s_hemi", LightMapTexture, false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);

	C.r_Sampler("s_blue_noise", "noise\\blue_noise_texture", false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	C.r_Sampler("s_perlin_noise", "noise\\perlin_noise_texture", false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	RImplementation.clearAllShaderOptions();

	C.r_End();
}

/*
{
	//RImplementation.addShaderOption("TEST_DEFINE", "1");

	// Uber-parse
	string256		fname, fnameA, fnameB;
	strcpy(fname, *C.L_textures[0]);
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
	strconcat(sizeof(vs), vs, "deffer_", _vspec, "");
	strconcat(sizeof(ps), ps, "deffer_", _pspec, lmap ? "_lmh" : "");

	strcpy_s(dt, sizeof(dt), C.detail_texture ? C.detail_texture : "ed\\ed_dummy_detail");

	if (_aref) { strcat(ps, "_aref"); }

	// HQ
//	if (!hq)		
// {
//		strcat			(vs,"-lq");
//		strcat			(ps,"-lq");
//	}

	if (bump)
		strcpy(fnameA, _t.bump_get().c_str());
	else
		strcpy(fnameA, "ed\\ed_dummy_bump");
	strconcat(sizeof(fnameB), fnameB, fnameA, "#");

	extern u32 ps_r2_bump_mode;
	if (((ps_r2_bump_mode == 1) || (r2_sun_static)) && hq)
		strcat(ps, "_normal");
	else if (((ps_r2_bump_mode == 2) || (!r2_sun_static && !r2_advanced_pp)) && hq)
		strcat(ps, "_parallax");
	else if (((ps_r2_bump_mode == 3) && (r2_advanced_pp)) && hq && C.bSteepParallax)
		strcat(ps, "_steep_parallax");
	else if (((ps_r2_bump_mode == 3) && (r2_advanced_pp)) && hq && !C.bSteepParallax)
		strcat(ps, "_parallax");


	if (C.bDetail && C.bDetail_Diffuse)
		strcat(ps, "_d");

	strcpy_s(dtA, dt);
	strconcat(sizeof(dtA), dtA, dtA, "_bump");
	strconcat(sizeof(dtB), dtB, dtA, "#");

	// Uber-construct
	C.r_Pass(vs, ps, FALSE);

	extern u32 ps_r2_debug_textures;
	if (ps_r2_debug_textures == 1)
		C.r_Sampler_tex("s_base", "ed\\debug_uv_checker");
	else if (ps_r2_debug_textures == 2)
		C.r_Sampler_tex("s_base", "ed\\debug_white");
	else
		C.r_Sampler("s_base", C.L_textures[0], false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	char* Ao = strconcat(sizeof(fname), fname, fname, "_ao");
	string_path BakedAO = { 0 };
	if (FS.exist(BakedAO, "$game_textures$", Ao, ".dds"))
		C.r_Sampler_tex("s_baked_ao", Ao);
	else
		C.r_Sampler_tex("s_baked_ao", "vfx\\vfx_no_ao");

	C.r_Sampler("s_bumpX", fnameB, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler("s_bump", fnameA, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detail", dt, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detailBump", dtA, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	C.r_Sampler("s_detailBumpX", dtB, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	if (lmap)C.r_Sampler("s_hemi", C.L_textures[2], false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);

	C.r_Sampler_tex("s_blue_noise", "noise\\blue_noise_texture");
	C.r_Sampler_tex("s_perlin_noise", "noise\\perlin_noise_texture");

	RImplementation.clearAllShaderOptions();

	if (!DO_NOT_FINISH)
		C.r_End();
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////