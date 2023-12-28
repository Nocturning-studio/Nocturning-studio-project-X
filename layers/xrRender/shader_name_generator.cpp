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
void generate_shader_name(CBlender_Compile& C, bool bIsHightQualityGeometry, LPCSTR VertexShaderName,
						  LPCSTR PixelShaderName, BOOL bUseAlpha)
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
	string_path HemisphereLightMapTexture;
	string_path LightMapTexture;
	string_path BakedAOTexture;

	string_path Dummy = {0};

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
	ref_texture refAlbedoTexture;
	refAlbedoTexture.create(AlbedoTexture);
	bool bUseBump = refAlbedoTexture.bump_exist();

	// Get bump map texture
	if (bUseBump)
	{
		strcpy_s(BumpTexture, sizeof(BumpTexture), refAlbedoTexture.bump_get().c_str());
	}
	else
	{
		strcpy_s(BumpTexture, sizeof(BumpTexture), AlbedoTexture);
		strconcat(sizeof(BumpTexture), BumpTexture, BumpTexture, "_bump");

		if (!FS.exist(Dummy, "$game_textures$", BumpTexture, ".dds"))
			strcpy_s(BumpTexture, sizeof(BumpTexture), "ed\\ed_dummy_bump");
	}

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

	// Checking for existance
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
		pcstr HemisphereLightMapTextureName = C.L_textures[2].c_str();
		pcstr LightMapTextureName = C.L_textures[1].c_str();
		if ((HemisphereLightMapTextureName[0] == 'l' && HemisphereLightMapTextureName[1] == 'm' &&
			 HemisphereLightMapTextureName[2] == 'a' && HemisphereLightMapTextureName[3] == 'p') &&
			(LightMapTextureName[0] == 'l' && LightMapTextureName[1] == 'm' &&
			 LightMapTextureName[2] == 'a' && LightMapTextureName[3] == 'p'))
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
	{
		strcpy_s(HemisphereLightMapTexture, sizeof(HemisphereLightMapTexture), *C.L_textures[2]);
		strcpy_s(LightMapTexture, sizeof(LightMapTexture), *C.L_textures[1]);
	}

	// Get BakedAO texture
	strcpy_s(BakedAOTexture, sizeof(BakedAOTexture), AlbedoTexture);
	strconcat(sizeof(BakedAOTexture), BakedAOTexture, BakedAOTexture, "_ao");
	if (!FS.exist(Dummy, "$game_textures$", BakedAOTexture, ".dds"))
		strcpy_s(BakedAOTexture, sizeof(BakedAOTexture), "ed\\debug_white");

	// Starting generate shader name
	strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, "gbuffer_stage_", PixelShaderName);
	strconcat(sizeof(NewVertexShaderName), NewVertexShaderName, "gbuffer_stage_", VertexShaderName);

	// Create lightmapped shader if need
	if (bUseLightMap)
		strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_lightmapped");

	// Create shader with alpha testing if need
	if (bUseAlpha)
		strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_alphatest");

	// Create shader with normal mapping or displacement if need
	if (bIsHightQualityGeometry)
	{
		if (ps_r2_bump_mode == 1 || r2_sun_static || !bUseBump)
			strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_normal");
		else if ((ps_r2_bump_mode == 2 || (!r2_sun_static && !r2_advanced_pp)) || (r2_advanced_pp && !C.bSteepParallax))
			strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_parallax");
		else if (((ps_r2_bump_mode == 3) && (r2_advanced_pp)) && C.bSteepParallax)
			strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_steep_parallax");
	}

	// Create shader with deatil texture if need
	if (bUseDetail)
		strconcat(sizeof(NewPixelShaderName), NewPixelShaderName, NewPixelShaderName, "_detailed");

	// RImplementation.addShaderOption("TEST_DEFINE", "1");

	// Create shader pass
	C.r_Pass(NewVertexShaderName, NewPixelShaderName, FALSE);

	C.r_Sampler("s_base", AlbedoTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_POINT,
				D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_baked_ao", BakedAOTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,
				D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_bumpX", BumpCorrectionTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,
				D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_bump", BumpTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,
				D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detail", DetailAlbedoTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,
				D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detailBump", DetailBumpTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR,
				D3DTEXF_ANISOTROPIC);

	C.r_Sampler("s_detailBumpX", DetailBumpCorrectionTexture, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC,
				D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);

	if (bUseLightMap)
	{
		C.r_Sampler("s_hemi", HemisphereLightMapTexture, false, D3DTADDRESS_CLAMP, D3DTEXF_GAUSSIANQUAD, D3DTEXF_NONE,
					D3DTEXF_LINEAR);
		C.r_Sampler("s_lmap", LightMapTexture, false, D3DTADDRESS_CLAMP, D3DTEXF_GAUSSIANQUAD, D3DTEXF_NONE,
					D3DTEXF_LINEAR);
	}

	jitter(C);

	RImplementation.clearAllShaderOptions();

	C.r_End();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
