#include "stdafx.h"
#include "r2.h"
#include "..\xrEngine\fbasicvisual.h"
#include "..\xrEngine\xr_object.h"
#include "..\xrEngine\CustomHUD.h"
#include "..\xrEngine\igame_persistent.h"
#include "..\xrEngine\environment.h"
#include "..\xrEngine\SkeletonCustom.h"
#include "..\xrRender\LightTrack.h"
#include <boost/crc.hpp>
#include "../xrEngine\r_constants.h"
//////////////////////////////////////////////////////////////////////////
CRender RImplementation;
//////////////////////////////////////////////////////////////////////////
class CGlow : public IRender_Glow
{
  public:
	bool bActive;

  public:
	CGlow() : bActive(false)
	{
	}
	virtual void set_active(bool b)
	{
		bActive = b;
	}
	virtual bool get_active()
	{
		return bActive;
	}
	virtual void set_position(const Fvector& P)
	{
	}
	virtual void set_direction(const Fvector& D)
	{
	}
	virtual void set_radius(float R)
	{
	}
	virtual void set_texture(LPCSTR name)
	{
	}
	virtual void set_color(const Fcolor& C)
	{
	}
	virtual void set_color(float r, float g, float b)
	{
	}
};

float r_dtex_range = 50.f;
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_dynamic(IRender_Visual* pVisual, float cdist_sq)
{
	int id = SE_R2_SHADOW;
	if (CRender::PHASE_NORMAL == RImplementation.phase)
	{
		id = ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
	}
	return pVisual->shader->E[id]._get();
}
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_static(IRender_Visual* pVisual, float cdist_sq)
{
	int id = SE_R2_SHADOW;
	if (CRender::PHASE_NORMAL == RImplementation.phase)
	{
		id = ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
	}
	return pVisual->shader->E[id]._get();
}
//////////////////////////////////////////////////////////////////////////
static class cl_parallax : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		float h = ps_r2_df_parallax_h;
		RCache.set_c(C, h, -h / 2.f, 1.f / r_dtex_range, 1.f / r_dtex_range);
	}
} binder_parallax;
//////////////////////////////////////////////////////////////////////////
static class cl_sun_far : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		float fValue = ps_r2_sun_far;
		RCache.set_c(C, fValue, fValue, fValue, 0);
	}
} binder_sun_far;
//////////////////////////////////////////////////////////////////////////
extern ENGINE_API BOOL r2_sun_static;
extern ENGINE_API BOOL r2_advanced_pp;
//////////////////////////////////////////////////////////////////////////
// Just two static storage
void CRender::create()
{
	Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 0x12345678);

	m_skinning = -1;

	///////////////////////////////////////////////////
	// Smap resolution choosing
#pragma todo(Deathman to Deathman : Ќеобходимо разделить разрешение shadow map на разрешение дл€ sun и spot или point источников света)
	if (o.sunstatic)
	{
		o.smapsize = 1024;
	}
	else
	{
		switch (ps_r2_sun_quality)
		{
		case 1:
			o.smapsize = 1024;
			o.sun_depth_far_bias = -0.001f;
			o.sun_depth_near_bias = -0.0001f;
			break;
		case 2:
			o.smapsize = 1536;
			o.sun_depth_far_bias = -0.001f;
			o.sun_depth_near_bias = -0.0001f;
			break;
		case 3:
			o.smapsize = 1536;
			o.sun_depth_far_bias = -0.001f;
			o.sun_depth_near_bias = -0.0001f;
			break;
		case 4:
			o.smapsize = 2048;
			o.sun_depth_far_bias = -0.001f;
			o.sun_depth_near_bias = -0.0001f;
			break;
		case 5:
			o.smapsize = 2560;
			o.sun_depth_far_bias = -0.0003f;
			o.sun_depth_near_bias = -0.00004f;
			break;
		}
		o.sun_depth_far_scale = 1.0f;
		o.sun_depth_near_scale = 1.0f;
	}
	///////////////////////////////////////////////////

	o.mrt = (HW.Caps.raster.dwMRT_count >= 3);
	o.mrtmixdepth = (HW.Caps.raster.b_MRT_mixdepth);

	// Check for NULL render target support
	D3DFORMAT nullrt = (D3DFORMAT)MAKEFOURCC('N', 'U', 'L', 'L');
	o.nullrt = HW.support(nullrt, D3DRTYPE_SURFACE, D3DUSAGE_RENDERTARGET);

	if (o.nullrt)
	{
		Msg("* NULLRT supported");
		if (0)
		{
			u32 device_id = HW.Caps.id_device;
			bool disable_nullrt = false;
			switch (device_id)
			{
			case 0x190:
			case 0x191:
			case 0x192:
			case 0x193:
			case 0x194:
			case 0x197:
			case 0x19D:
			case 0x19E: {
				disable_nullrt = true; // G80
				break;
			}
			case 0x400:
			case 0x401:
			case 0x402:
			case 0x403:
			case 0x404:
			case 0x405:
			case 0x40E:
			case 0x40F: {
				disable_nullrt = true; // G84
				break;
			}
			case 0x420:
			case 0x421:
			case 0x422:
			case 0x423:
			case 0x424:
			case 0x42D:
			case 0x42E:
			case 0x42F: {
				disable_nullrt = true; // G86
				break;
			}
			}
			if (disable_nullrt)
				o.nullrt = false;
		};
		if (o.nullrt)
			Msg("* ...and used");
	};

	// SMAP / DST
	o.HW_smap = HW.support(D3DFMT_D24X8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL);
	o.fp16_filter = HW.support(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER);
	o.fp16_blend = HW.support(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);

	R_ASSERT2(o.mrt && o.mrtmixdepth && o.HW_smap && o.fp16_filter && o.fp16_blend &&
				  (HW.Caps.raster.dwInstructions >= 256),
			  "Hardware doesn't meet minimum feature-level");

	// nvstencil on NV40 and up
	o.nvstencil = FALSE;
	if ((HW.Caps.id_vendor == 0x10DE) && (HW.Caps.id_device >= 0x40))
		o.nvstencil = TRUE;
	if (strstr(Core.Params, "-nonvs"))
		o.nvstencil = FALSE;

	// nv-dbt
	o.nvdbt = HW.support((D3DFORMAT)MAKEFOURCC('N', 'V', 'D', 'B'), D3DRTYPE_SURFACE, 0);
	if (o.nvdbt)
		Msg("* NV-DBT supported and used");

	// options
	o.bug = (strstr(Core.Params, "-bug")) ? TRUE : FALSE;
	o.sunfilter = (strstr(Core.Params, "-sunfilter")) ? TRUE : FALSE;
	// o.sunstatic			= (strstr(Core.Params,"-sunstatic"))?	TRUE	:FALSE	;
	o.sunstatic = r2_sun_static;
	o.advancedpp = r2_advanced_pp;
	o.sjitter = (strstr(Core.Params, "-sjitter")) ? TRUE : FALSE;
	o.depth16 = (strstr(Core.Params, "-depth16")) ? TRUE : FALSE;
	o.noshadows = (strstr(Core.Params, "-noshadows")) ? TRUE : FALSE;
	o.Tshadows = (strstr(Core.Params, "-tsh")) ? TRUE : FALSE;
	o.mblur = (strstr(Core.Params, "-mblur")) ? TRUE : FALSE;
	o.distortion_enabled = (strstr(Core.Params, "-nodistort")) ? FALSE : TRUE;
	o.distortion = o.distortion_enabled;
	o.disasm = (strstr(Core.Params, "-disasm")) ? TRUE : FALSE;
	o.forceskinw = (strstr(Core.Params, "-skinw")) ? TRUE : FALSE;

	// constants
	::Device.Resources->RegisterConstantSetup("parallax_heigt", &binder_parallax);
	::Device.Resources->RegisterConstantSetup("sun_far", &binder_sun_far);

	c_lmaterial = "L_material";
	c_sbase = "s_base";

	Target = xr_new<CRenderTarget>(); // Main target

	Models = xr_new<CModelPool>();
	PSLibrary.OnCreate();
	HWOCC.occq_create(occq_size);

	// rmNormal					();
	marker = 0;
	ZeroMemory(q_sync_point, sizeof(q_sync_point));
	for (u32 i = 0; i < HW.Caps.iGPUNum; ++i)
		R_CHK(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &q_sync_point[i]));

	xrRender_apply_tf();
	::PortalTraverser.initialize();
}

void CRender::destroy()
{
	::PortalTraverser.destroy();
	for (u32 i = 0; i < HW.Caps.iGPUNum; ++i)
		_RELEASE(q_sync_point[i]);
	HWOCC.occq_destroy();
	xr_delete(Models);
	xr_delete(Target);
	PSLibrary.OnDestroy();
	Device.seqFrame.Remove(this);
	r_dsgraph_destroy(); // FIX BY IXRAY (THANKS BY DEATHMAN)
}

void CRender::reset_begin()
{
	// Update incremental shadowmap-visibility solver
	// BUG-ID: 10646
	{
		u32 it = 0;
		for (it = 0; it < Lights_LastFrame.size(); it++)
		{
			if (0 == Lights_LastFrame[it])
				continue;
			try
			{
				Lights_LastFrame[it]->svis.resetoccq();
			}
			catch (...)
			{
				Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
			}
		}
		Lights_LastFrame.clear();
	}

	xr_delete(Target);
	HWOCC.occq_destroy();
	for (u32 i = 0; i < HW.Caps.iGPUNum; ++i)
		_RELEASE(q_sync_point[i]);
}

void CRender::reset_end()
{
	for (u32 i = 0; i < HW.Caps.iGPUNum; ++i)
		R_CHK(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &q_sync_point[i]));
	HWOCC.occq_create(occq_size);

	Target = xr_new<CRenderTarget>();

	xrRender_apply_tf();

	// Set this flag true to skip the first render frame,
	// that some data is not ready in the first frame (for example device camera position)
	m_bFirstFrameAfterReset = true;
}

void CRender::OnFrame()
{
	Models->DeleteQueue();
	if (ps_render_flags.test(RFLAG_EXP_MT_CALC))
	{
		// MT-details (@front)
		Device.seqParallel.insert(Device.seqParallel.begin(),
								  fastdelegate::FastDelegate0<>(Details, &CDetailManager::MT_CALC));

		// MT-HOM (@front)
		Device.seqParallel.insert(Device.seqParallel.begin(), fastdelegate::FastDelegate0<>(&HOM, &CHOM::MT_RENDER));
	}
}

BOOL CRender::is_sun()
{
	if (o.sunstatic)
		return FALSE;
	Fcolor sun_color = ((light*)Lights.sun_adapted._get())->color;
	return (ps_r2_lighting_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r, sun_color.g, sun_color.b) > EPS));
}

// Implementation
IRender_ObjectSpecific* CRender::ros_create(IRenderable* parent)
{
	return xr_new<CROS_impl>();
}
void CRender::ros_destroy(IRender_ObjectSpecific*& p)
{
	xr_delete(p);
}
IRender_Visual* CRender::model_Create(LPCSTR name, IReader* data)
{
	return Models->Create(name, data);
}
IRender_Visual* CRender::model_CreateChild(LPCSTR name, IReader* data)
{
	return Models->CreateChild(name, data);
}
IRender_Visual* CRender::model_Duplicate(IRender_Visual* V)
{
	return Models->Instance_Duplicate(V);
}
void CRender::model_Delete(IRender_Visual*& V, BOOL bDiscard)
{
	Models->Delete(V, bDiscard);
}
IRender_DetailModel* CRender::model_CreateDM(IReader* F)
{
	CDetail* D = xr_new<CDetail>();
	D->Load(F);
	return D;
}
void CRender::model_Delete(IRender_DetailModel*& F)
{
	if (F)
	{
		CDetail* D = (CDetail*)F;
		D->Unload();
		xr_delete(D);
		F = NULL;
	}
}
IRender_Visual* CRender::model_CreatePE(LPCSTR name)
{
	PS::CPEDef* SE = PSLibrary.FindPED(name);
	R_ASSERT3(SE, "Particle effect doesn't exist", name);
	return Models->CreatePE(SE);
}
IRender_Visual* CRender::model_CreateParticles(LPCSTR name)
{
	PS::CPEDef* SE = PSLibrary.FindPED(name);
	if (SE)
		return Models->CreatePE(SE);
	else
	{
		PS::CPGDef* SG = PSLibrary.FindPGD(name);
		R_ASSERT3(SG, "Particle effect or group doesn't exist", name);
		return Models->CreatePG(SG);
	}
}
void CRender::models_Prefetch()
{
	Models->Prefetch();
}
void CRender::models_Clear(BOOL b_complete)
{
	Models->ClearPool(b_complete);
}

ref_shader CRender::getShader(int id)
{
	VERIFY(id < int(Shaders.size()));
	return Shaders[id];
}
IRender_Portal* CRender::getPortal(int id)
{
	VERIFY(id < int(Portals.size()));
	return Portals[id];
}
IRender_Sector* CRender::getSector(int id)
{
	VERIFY(id < int(Sectors.size()));
	return Sectors[id];
}
IRender_Sector* CRender::getSectorActive()
{
	return pLastSector;
}
IRender_Visual* CRender::getVisual(int id)
{
	VERIFY(id < int(Visuals.size()));
	return Visuals[id];
}
D3DVERTEXELEMENT9* CRender::getVB_Format(int id, BOOL _alt)
{
	if (_alt)
	{
		VERIFY(id < int(xDC.size()));
		return xDC[id].begin();
	}
	else
	{
		VERIFY(id < int(nDC.size()));
		return nDC[id].begin();
	}
}
IDirect3DVertexBuffer9* CRender::getVB(int id, BOOL _alt)
{
	if (_alt)
	{
		VERIFY(id < int(xVB.size()));
		return xVB[id];
	}
	else
	{
		VERIFY(id < int(nVB.size()));
		return nVB[id];
	}
}
IDirect3DIndexBuffer9* CRender::getIB(int id, BOOL _alt)
{
	if (_alt)
	{
		VERIFY(id < int(xIB.size()));
		return xIB[id];
	}
	else
	{
		VERIFY(id < int(nIB.size()));
		return nIB[id];
	}
}
FSlideWindowItem* CRender::getSWI(int id)
{
	VERIFY(id < int(SWIs.size()));
	return &SWIs[id];
}
IRender_Target* CRender::getTarget()
{
	return Target;
}

IRender_Light* CRender::light_create()
{
	return Lights.Create();
}
IRender_Glow* CRender::glow_create()
{
	return xr_new<CGlow>();
}

void CRender::flush()
{
	r_dsgraph_render_graph(0);
}

BOOL CRender::occ_visible(vis_data& P)
{
	return HOM.visible(P);
}
BOOL CRender::occ_visible(sPoly& P)
{
	return HOM.visible(P);
}
BOOL CRender::occ_visible(Fbox& P)
{
	return HOM.visible(P);
}

void CRender::add_Visual(IRender_Visual* V)
{
	add_leafs_Dynamic(V);
}
void CRender::add_Geometry(IRender_Visual* V)
{
	add_Static(V, View->getMask());
}
void CRender::add_StaticWallmark(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* verts)
{
	if (T->suppress_wm)
		return;
	VERIFY2(_valid(P) && _valid(s) && T && verts && (s > EPS_L), "Invalid static wallmark params");
	Wallmarks->AddStaticWallmark(T, verts, P, &*S, s);
}

void CRender::clear_static_wallmarks()
{
	Wallmarks->clear();
}

void CRender::add_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm)
{
	Wallmarks->AddSkeletonWallmark(wm);
}
void CRender::add_SkeletonWallmark(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start,
								   const Fvector& dir, float size)
{
	Wallmarks->AddSkeletonWallmark(xf, obj, sh, start, dir, size);
}
void CRender::add_Occluder(Fbox2& bb_screenspace)
{
	HOM.occlude(bb_screenspace);
}
void CRender::set_Object(IRenderable* O)
{
	val_pObject = O;
}
void CRender::rmNear()
{
	IRender_Target* T = getTarget();
	D3DVIEWPORT9 VP = {0, 0, T->get_width(), T->get_height(), 0, 0.02f};
	CHK_DX(HW.pDevice->SetViewport(&VP));
}
void CRender::rmFar()
{
	IRender_Target* T = getTarget();
	D3DVIEWPORT9 VP = {0, 0, T->get_width(), T->get_height(), 0.99999f, 1.f};
	CHK_DX(HW.pDevice->SetViewport(&VP));
}
void CRender::rmNormal()
{
	IRender_Target* T = getTarget();
	D3DVIEWPORT9 VP = {0, 0, T->get_width(), T->get_height(), 0, 1.f};
	CHK_DX(HW.pDevice->SetViewport(&VP));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender() : m_bFirstFrameAfterReset(false)
{
	init_cacades();
}

CRender::~CRender()
{
}

#include "..\xrEngine\GameFont.h"
// #include "../xrRender/xrRender_console.cpp"
void CRender::Statistics(CGameFont* _F)
{
	CGameFont& F = *_F;
	F.OutNext(" **** LT:%2d,LV:%2d **** ", stats.l_total, stats.l_visible);
	stats.l_visible = 0;
	F.OutNext("    S(%2d)   | (%2d)NS   ", stats.l_shadowed, stats.l_unshadowed);
	F.OutNext("smap use[%2d], merge[%2d], finalclip[%2d]", stats.s_used, stats.s_merged - stats.s_used,
			  stats.s_finalclip);
	stats.s_used = 0;
	stats.s_merged = 0;
	stats.s_finalclip = 0;
	F.OutSkip();
	F.OutNext(" **** Occ-Q(%03.1f) **** ", 100.f * f32(stats.o_culled) / f32(stats.o_queries ? stats.o_queries : 1));
	F.OutNext(" total  : %2d", stats.o_queries);
	stats.o_queries = 0;
	F.OutNext(" culled : %2d", stats.o_culled);
	stats.o_culled = 0;
	F.OutSkip();
	u32 ict = stats.ic_total + stats.ic_culled;
	F.OutNext(" **** iCULL(%03.1f) **** ", 100.f * f32(stats.ic_culled) / f32(ict ? ict : 1));
	F.OutNext(" visible: %2d", stats.ic_total);
	stats.ic_total = 0;
	F.OutNext(" culled : %2d", stats.ic_culled);
	stats.ic_culled = 0;
#ifdef DEBUG
	HOM.stats();
#endif
}

/////////
#pragma comment(lib, "d3dx9.lib")
/*
extern "C"
{
LPCSTR WINAPI	D3DXGetPixelShaderProfile	(LPDIRECT3DDEVICE9  pDevice);
LPCSTR WINAPI	D3DXGetVertexShaderProfile	(LPDIRECT3DDEVICE9	pDevice);
};
*/
void CRender::addShaderOption(const char* name, const char* value)
{
	D3DXMACRO macro = {name, value};
	m_ShaderOptions.push_back(macro);
}

void CRender::clearAllShaderOptions()
{
	m_ShaderOptions.clear();
}

static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name,
							 void*& result, bool const disasm, LPCSTR const disasm_file_name)
{
	HRESULT _result = E_FAIL;
	if (pTarget[0] == 'p')
	{
		SPS* sps_result = (SPS*)result;
		_result = HW.pDevice->CreatePixelShader(buffer, &sps_result->ps);
		if (!SUCCEEDED(_result))
		{
			Log("! PS: ", file_name);
			Msg("! CreatePixelShader hr == 0x%08x", _result);
			return E_FAIL;
		}

		LPCVOID data = NULL;
		_result = D3DXFindShaderComment(buffer, MAKEFOURCC('C', 'T', 'A', 'B'), &data, NULL);
		if (SUCCEEDED(_result) && data)
		{
			LPD3DXSHADER_CONSTANTTABLE pConstants = LPD3DXSHADER_CONSTANTTABLE(data);
			sps_result->constants.parse(pConstants, 0x1);
		}
		else
		{
			Log("! PS: ", file_name);
			Msg("! D3DXFindShaderComment hr == 0x%08x", _result);
		}
	}
	else
	{
		SVS* svs_result = (SVS*)result;
		_result = HW.pDevice->CreateVertexShader(buffer, &svs_result->vs);
		if (!SUCCEEDED(_result))
		{
			Log("! VS: ", file_name);
			Msg("! CreatePixelShader hr == 0x%08x", _result);
			return E_FAIL;
		}

		LPCVOID data = NULL;
		_result = D3DXFindShaderComment(buffer, MAKEFOURCC('C', 'T', 'A', 'B'), &data, NULL);
		if (SUCCEEDED(_result) && data)
		{
			LPD3DXSHADER_CONSTANTTABLE pConstants = LPD3DXSHADER_CONSTANTTABLE(data);
			svs_result->constants.parse(pConstants, 0x2);
		}
		else
		{
			Log("! VS: ", file_name);
			Msg("! D3DXFindShaderComment hr == 0x%08x", _result);
		}
	}

	if (disasm)
	{
		ID3DXBuffer* _disasm = 0;
		D3DXDisassembleShader(LPDWORD(buffer), TRUE, 0, &_disasm);
		string_path dname;
		strconcat(sizeof(dname), dname, "disassemblied_shaders\\", disasm_file_name,
				  ('v' == pTarget[0]) ? "_vertex_shader.html" : "_pixel_shader.html");
		IWriter* W = FS.w_open("$app_data_root$", dname);
		W->w(_disasm->GetBufferPointer(), _disasm->GetBufferSize());
		FS.w_close(W);
		_RELEASE(_disasm);
	}

	return _result;
}

//--------------------------------------------------------------------------------------------------------------
class includer : public ID3DXInclude
{
  public:
	HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData,
						   UINT* pBytes)
	{
		string_path pname;
		strconcat(sizeof(pname), pname, ::Render->getShaderPath(), pFileName);
		IReader* R = FS.r_open("$game_shaders$", pname);
		if (0 == R)
		{
			// possibly in shared directory or somewhere else - open directly
			R = FS.r_open("$game_shaders$", pFileName);
			if (0 == R)
				return E_FAIL;
		}

		// duplicate and zero-terminate
		size_t size = R->length();
		u8* data = xr_alloc<u8>(size + 1);
		CopyMemory(data, R->pointer(), size);
		data[size] = 0;
		FS.r_close(R);

		*ppData = data;
		*pBytes = static_cast<u32>(size);
		return D3D_OK;
	}
	HRESULT __stdcall Close(LPCVOID pData)
	{
		xr_free(pData);
		return D3D_OK;
	}
};

static inline bool match_shader(LPCSTR const debug_shader_id, LPCSTR const full_shader_id, LPCSTR const mask,
								size_t const mask_length);
static inline bool match_shader_id(LPCSTR const debug_shader_id, LPCSTR const full_shader_id,
								   FS_FileSet const& file_set, string_path& result);

HRESULT CRender::shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName,
								LPCSTR pTarget, DWORD Flags, void*& result)
{
	D3DXMACRO defines[512]{};
	int def_it = 0;

	char c_smapsize[32];
	char c_gloss[32];

	char c_hard_optimization[32];

	char c_sun_shafts[32];

	char c_ao[32];
	char c_ao_quality[32];

	char c_aa[32];
	char c_aa_quality[32];

	char c_debug_frame_layers[32];

	char c_ps_shadow_filtering[32];

	char c_vignette[32];
	char c_chroma_abb[32];
	char c_sepia[32];
	char c_bloom[32];
	char c_hdr[32];

	char c_soft_water[32];
	char c_soft_particles[32];

	char c_fog_quality[32];

	char c_mblur[32];
	char c_mblur_quality[32];
	char c_dof[32];
	char c_dof_quality[32];

	char c_sharpen[32];

	char c_bump_quality[32];

	char sh_name[MAX_PATH] = "";

	for (u32 i = 0; i < m_ShaderOptions.size(); ++i)
	{
		defines[def_it++] = m_ShaderOptions[i];
	}

	size_t len = 0;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// R2a/R2/R2.5 defines
	//////////////////////////////////////////////////////////////////////////////////////////////
	{
		sprintf(c_smapsize, "%d", u32(o.smapsize));
		defines[def_it].Name = "SMAP_size";
		defines[def_it].Definition = c_smapsize;
		def_it++;
		strcat(sh_name, c_smapsize);
		len += 4;
	}
	sh_name[len] = '0' + char(o.smapsize);
	++len;

	if (HW.Caps.raster_major >= 3)
	{
		defines[def_it].Name = "USE_BRANCHING";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(HW.Caps.raster_major >= 3);
	++len;

	if (HW.Caps.geometry.bVTF)
	{
		defines[def_it].Name = "USE_VTF";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(HW.Caps.geometry.bVTF);
	++len;

	if (o.Tshadows)
	{
		defines[def_it].Name = "USE_TSHADOWS";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(o.Tshadows);
	++len;

	if (ps_r2_debug_frame_layers)
	{
		sprintf(c_debug_frame_layers, "%d", ps_r2_debug_frame_layers);
		defines[def_it].Name = "DEBUG_VIEW_MODE";
		defines[def_it].Definition = c_debug_frame_layers;
		def_it++;
		strcat(sh_name, c_debug_frame_layers);
		len += 4;
	}
	sh_name[len] = '0' + char(ps_r2_debug_frame_layers);
	++len;

	if (o.sunfilter)
	{
		defines[def_it].Name = "USE_SUNFILTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(o.sunfilter);
	++len;

	if (o.sunstatic)
	{
		defines[def_it].Name = "USE_R2_STATIC_SUN";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(o.sunstatic);
	++len;

	if (o.advancedpp)
	{
		defines[def_it].Name = "USE_R2_ADVANCED_POSTPROCESS";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(o.advancedpp);
	++len;

	int HardOptimization = ps_r2_ls_flags.test(R2FLAG_HARD_OPTIMIZATION);
	if (HardOptimization)
	{
		sprintf(c_hard_optimization, "%d", HardOptimization);
		defines[def_it].Name = "USE_R2_HARD_OPTIMIZATION";
		defines[def_it].Definition = c_hard_optimization;
		def_it++;
		strcat(sh_name, c_hard_optimization);
		len += 1;
	}
	sh_name[len] = '0' + char(HardOptimization);
	++len;

	int vignette = ps_vignette_mode;
	if (vignette)
	{
		sprintf(c_vignette, "%d", vignette);
		defines[def_it].Name = "VIGNETTE_MODE";
		defines[def_it].Definition = c_vignette;
		def_it++;
		strcat(sh_name, c_vignette);
		len += 1;
		sh_name[len] = '0' + char(vignette);
		++len;
	}

	int sepia = ps_r2_postprocess_flags.test(R2FLAG_SEPIA);
	if (sepia)
	{
		sprintf(c_sepia, "%d", sepia);
		defines[def_it].Name = "USE_SEPIA";
		defines[def_it].Definition = c_sepia;
		def_it++;
		strcat(sh_name, c_sepia);
		len += 1;
	}
	sh_name[len] = '0' + char(sepia);
	++len;

	int chroma_abb = ps_r2_postprocess_flags.test(R2FLAG_CHROMATIC_ABBERATION);
	if (chroma_abb)
	{
		sprintf(c_chroma_abb, "%d", chroma_abb);
		defines[def_it].Name = "USE_CHROMATIC_ABBERATION";
		defines[def_it].Definition = c_chroma_abb;
		def_it++;
		strcat(sh_name, c_chroma_abb);
		len += 1;
	}
	sh_name[len] = '0' + char(chroma_abb);
	++len;

	int bloom = ps_r2_postprocess_flags.test(R2FLAG_BLOOM);
	if (bloom)
	{
		sprintf(c_bloom, "%d", bloom);
		defines[def_it].Name = "USE_BLOOM";
		defines[def_it].Definition = c_bloom;
		def_it++;
		strcat(sh_name, c_bloom);
		len += 1;
	}
	sh_name[len] = '0' + char(bloom);
	++len;

	int HdrEnabled = ps_r2_postprocess_flags.test(R2FLAG_HDR);
	if (HdrEnabled)
	{
		sprintf(c_hdr, "%d", HdrEnabled);
		defines[def_it].Name = "USE_HDR";
		defines[def_it].Definition = c_hdr;
		def_it++;
		strcat(sh_name, c_hdr);
		len += 1;
	}
	sh_name[len] = '0' + char(HdrEnabled);
	++len;

	if (o.forceskinw)
	{
		defines[def_it].Name = "SKIN_COLOR";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(o.forceskinw);
	++len;

	// skinning
	if (m_skinning < 0)
	{
		defines[def_it].Name = "SKIN_NONE";
		defines[def_it].Definition = "1";
		def_it++;
		sh_name[len] = '1';
		++len;
	}
	else
	{
		sh_name[len] = '0';
		++len;
	}

	if (0 == m_skinning)
	{
		defines[def_it].Name = "SKIN_0";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(0 == m_skinning);
	++len;

	if (1 == m_skinning)
	{
		defines[def_it].Name = "SKIN_1";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(1 == m_skinning);
	++len;

	if (2 == m_skinning)
	{
		defines[def_it].Name = "SKIN_2";
		defines[def_it].Definition = "1";
		def_it++;
	}
	sh_name[len] = '0' + char(2 == m_skinning);
	++len;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// R2.5 only defines
	//////////////////////////////////////////////////////////////////////////////////////////////

	if (RImplementation.o.advancedpp)
	{
		if (ps_r2_postprocess_flags.test(R2FLAG_MBLUR))
		{
			sprintf(c_mblur, "%d", 1);
			defines[def_it].Name = "USE_MBLUR";
			defines[def_it].Definition = c_mblur;
			def_it++;
			strcat(sh_name, c_mblur);
			len += 4;
		}
		sh_name[len] = '0' + char(ps_r2_postprocess_flags.test(R2FLAG_MBLUR));
		++len;

		if (ps_r2_mblur_quality)
		{
			sprintf(c_mblur_quality, "%d", ps_r2_mblur_quality);
			defines[def_it].Name = "MBLUR_QUALITY";
			defines[def_it].Definition = c_mblur_quality;
			def_it++;
			strcat(sh_name, c_mblur_quality);
			len += 4;
		}
		sh_name[len] = '0' + (char)ps_r2_mblur_quality;
		++len;

		if (ps_r2_postprocess_flags.test(R2FLAG_DOF))
		{
			sprintf(c_dof, "%d", 1);
			defines[def_it].Name = "USE_DOF";
			defines[def_it].Definition = c_dof;
			def_it++;
			strcat(sh_name, c_dof);
			len += 4;
		}
		sh_name[len] = '0' + char(ps_r2_postprocess_flags.test(R2FLAG_DOF));
		++len;

		if (ps_r2_dof_quality)
		{
			sprintf(c_dof_quality, "%d", ps_r2_dof_quality);
			defines[def_it].Name = "DOF_QUALITY";
			defines[def_it].Definition = c_dof_quality;
			def_it++;
			strcat(sh_name, c_dof_quality);
			len += 4;
		}
		sh_name[len] = '0' + (char)ps_r2_dof_quality;
		++len;

		if (ps_r2_ao)
		{
			sprintf(c_ao, "%d", ps_r2_ao);
			defines[def_it].Name = "AO_TYPE";
			defines[def_it].Definition = c_ao;
			def_it++;
			strcat(sh_name, c_ao);
			len += 4;
		}
		sh_name[len] = '0' + char(ps_r2_ao);
		++len;

		if (ps_r2_ao_quality)
		{
			sprintf(c_ao_quality, "%d", ps_r2_ao_quality);
			defines[def_it].Name = "AO_QUALITY";
			defines[def_it].Definition = c_ao_quality;
			def_it++;
			strcat(sh_name, c_ao_quality);
			len += 4;
		}
		sh_name[len] = '0' + (char)ps_r2_ao_quality;
		++len;

		if (ps_r2_aa)
		{
			sprintf(c_aa, "%d", ps_r2_aa);
			defines[def_it].Name = "AA_TYPE";
			defines[def_it].Definition = c_aa;
			def_it++;
			strcat(sh_name, c_aa);
			len += 3;
		}
		sh_name[len] = '0' + (char)ps_r2_aa;
		++len;

		if (ps_r2_aa_quality)
		{
			sprintf(c_aa_quality, "%d", ps_r2_aa_quality);
			defines[def_it].Name = "AA_QUALITY";
			defines[def_it].Definition = c_aa_quality;
			def_it++;
			strcat(sh_name, c_aa_quality);
			len += 4;
		}
		sh_name[len] = '0' + (char)ps_r2_aa_quality;
		++len;

		int bUseSharpen = (ps_r2_postprocess_flags.test(R2FLAG_SHARPEN));
		if (bUseSharpen)
		{
			sprintf(c_sharpen, "%d", bUseSharpen);
			defines[def_it].Name = "USE_SHARPEN";
			defines[def_it].Definition = c_sharpen;
			def_it++;
			strcat(sh_name, c_sharpen);
			len += 4;
		}
		sh_name[len] = '0' + char(bUseSharpen);
		++len;

		int soft_water = ps_r2_postprocess_flags.test(R2FLAG_SOFT_WATER);
		if (ps_r2_postprocess_flags.test(R2FLAG_SOFT_WATER))
		{
			sprintf(c_soft_water, "%d", soft_water);
			defines[def_it].Name = "USE_SOFT_WATER";
			defines[def_it].Definition = c_soft_water;
			def_it++;
			strcat(sh_name, c_soft_water);
			len += 1;
		}
		sh_name[len] = '0' + char(soft_water);
		++len;

		int soft_particles = ps_r2_postprocess_flags.test(R2FLAG_SOFT_PARTICLES);
		if (ps_r2_postprocess_flags.test(R2FLAG_SOFT_PARTICLES))
		{
			sprintf(c_soft_particles, "%d", soft_particles);
			defines[def_it].Name = "USE_SOFT_PARTICLES";
			defines[def_it].Definition = c_soft_particles;
			def_it++;
			strcat(sh_name, c_soft_particles);
			len += 1;
		}
		sh_name[len] = '0' + char(soft_particles);
		++len;

		if (ps_r2_shadow_filtering)
		{
			sprintf(c_ps_shadow_filtering, "%d", ps_r2_shadow_filtering);
			defines[def_it].Name = "SHADOW_FILTER";
			defines[def_it].Definition = c_ps_shadow_filtering;
			def_it++;
			strcat(sh_name, c_ps_shadow_filtering);
			len += 4;
		}
		sh_name[len] = '0' + char(ps_r2_shadow_filtering);
		++len;

		if (ps_r2_fog_quality)
		{
			sprintf(c_fog_quality, "%d", ps_r2_fog_quality);
			defines[def_it].Name = "FOG_QUALITY";
			defines[def_it].Definition = c_fog_quality;
			def_it++;
			strcat(sh_name, c_fog_quality);
			len += 4;
		}
		sh_name[len] = '0' + (char)ps_r2_fog_quality;
		++len;

		if (ps_r2_sun_shafts)
		{
			sprintf(c_sun_shafts, "%d", ps_r2_sun_shafts);
			defines[def_it].Name = "SUN_SHAFTS_QUALITY";
			defines[def_it].Definition = c_sun_shafts;
			def_it++;
			sh_name[len] = '0' + char(ps_r2_sun_shafts);
			++len;
		}
		else
		{
			sh_name[len] = '0';
			++len;
		}
		//////////////////////////////////////////////////////////////////////////

		if (ps_r2_bump_quality)
		{
			sprintf(c_bump_quality, "%d", ps_r2_bump_quality);
			defines[def_it].Name = "BUMP_QUALITY";
			defines[def_it].Definition = c_bump_quality;
			def_it++;
			strcat(sh_name, c_bump_quality);
			len += 4;
		}
		sh_name[len] = '0' + (char)ps_r2_bump_quality;
		++len;
	}

	// finish
	defines[def_it].Name = 0;
	defines[def_it].Definition = 0;
	def_it++;

	// Precache

	HRESULT _result = E_FAIL;

	char extension[3];
	strncpy_s(extension, pTarget, 2);

	string_path folder_name, folder = {0};
	strcpy(folder, "precompiled_shaders\\");
	strcat(folder, getShaderPath());
	strcat(folder, "\\");
	strcat(folder, name);
	strcat(folder, ".");
	strcat(folder, extension);
	strcat(folder, "_");
	strcat(folder, sh_name);
	strcat(folder, ".xrprecompiledshader");

	FS.update_path(folder_name, "$game_shaders$", folder);
	strcat(folder_name, "\\");

	m_file_set.clear();
	FS.file_list(m_file_set, folder_name, FS_ListFiles | FS_RootOnly, "*");

	string_path temp_file_name, file_name;
	if (!match_shader_id(name, sh_name, m_file_set, temp_file_name))
	{
		//		Msg				( "no library shader found" );
		string_path file = {0};
		strcpy(file, "shaders_cache\\");
		strcat(file, getShaderPath());
		strcat(file, "\\");
		strcat(file, name);
		strcat(file, ".");
		strcat(file, extension);
		strcat(file, "_");
		strcat(file, sh_name);
		strcat(file, ".xrprecompiledshader");
		FS.update_path(file_name, "$app_data_root$", file);
	}
	else
	{
		strcpy(file_name, folder_name);
		strcat(file_name, temp_file_name);
	}

	string_path disasm_file_name = {0};
	strcat(disasm_file_name, getShaderPath());
	strcat(disasm_file_name, "\\");
	strcat(disasm_file_name, name);

	if (FS.exist(file_name))
	{
		// Msg("opening library or cache shader...");
		IReader* file = FS.r_open(file_name);
		if (file->length() > 4)
		{
			u32 crc = 0;
			crc = file->r_u32();

			boost::crc_32_type processor;
			processor.process_block(file->pointer(), ((char*)file->pointer()) + file->elapsed());
			u32 const real_crc = processor.checksum();

			if (real_crc == crc)
			{
				_result = create_shader(pTarget, (DWORD*)file->pointer(), static_cast<u32>(file->elapsed()), file_name,
										result, o.disasm, disasm_file_name);
			}
		}
		file->close();
	}

	if (FAILED(_result))
	{
		//
		if (0 == xr_strcmp(pFunctionName, "main"))
		{
			if ('v' == pTarget[0])
				pTarget = D3DXGetVertexShaderProfile(HW.pDevice); // vertex	"vs_2_a"; //
			else
				pTarget = D3DXGetPixelShaderProfile(HW.pDevice); // pixel	"ps_2_a"; //
		}

		includer Includer;
		LPD3DXBUFFER pShaderBuf = NULL;
		LPD3DXBUFFER pErrorBuf = NULL;
		LPD3DXCONSTANTTABLE pConstants = NULL;
		LPD3DXINCLUDE pInclude = (LPD3DXINCLUDE)&Includer;

		_result = D3DXCompileShader((LPCSTR)pSrcData, SrcDataLen, defines, pInclude, pFunctionName, pTarget,
									Flags | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL | D3DXSHADER_PACKMATRIX_ROWMAJOR |
										D3DXSHADER_PACKMATRIX_COLUMNMAJOR,
									&pShaderBuf, &pErrorBuf, &pConstants);

		if (SUCCEEDED(_result))
		{
			IWriter* file = FS.w_open(file_name);

			boost::crc_32_type processor;
			processor.process_block(pShaderBuf->GetBufferPointer(),
									((char*)pShaderBuf->GetBufferPointer()) + pShaderBuf->GetBufferSize());
			u32 const crc = processor.checksum();

			file->w_u32(crc);
			file->w(pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize());
			FS.w_close(file);

			_result = create_shader(pTarget, (DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
									file_name, result, o.disasm, disasm_file_name);

			if (pErrorBuf)
			{
				Log("! ", file_name);
				Log("! Warning: ", (LPCSTR)pErrorBuf->GetBufferPointer());
			}
		}
		else
		{
			Log("! ", file_name);
			R_ASSERT2(SUCCEEDED(_result),
					  make_string("! Can't compile shader: %s, %s", file_name, (LPCSTR)pErrorBuf->GetBufferPointer()));
			if (pErrorBuf)
				Log("! Error: ", (LPCSTR)pErrorBuf->GetBufferPointer());
			else
				Msg("Can't compile shader hr=0x%08x", _result);
		}
	}

	return _result;
}

static inline bool match_shader(LPCSTR const debug_shader_id, LPCSTR const full_shader_id, LPCSTR const mask,
								size_t const mask_length)
{
	size_t const full_shader_id_length = xr_strlen(full_shader_id);
	R_ASSERT2(full_shader_id_length == mask_length,
			  make_string("bad cache for shader %s, [%s], [%s]", debug_shader_id, mask, full_shader_id));
	char const* i = full_shader_id;
	char const* const e = full_shader_id + full_shader_id_length;
	char const* j = mask;
	for (; i != e; ++i, ++j)
	{
		if (*i == *j)
			continue;

		if (*j == '_')
			continue;

		return false;
	}

	return true;
}

static inline bool match_shader_id(LPCSTR const debug_shader_id, LPCSTR const full_shader_id,
								   FS_FileSet const& file_set, string_path& result)
{
#ifdef DEBUG
	LPCSTR temp = "";
	bool found = false;
	FS_FileSet::const_iterator i = file_set.begin();
	FS_FileSet::const_iterator const e = file_set.end();
	for (; i != e; ++i)
	{
		if (match_shader(debug_shader_id, full_shader_id, (*i).name.c_str(), (*i).name.size()))
		{
			VERIFY(!found);
			found = true;
			temp = (*i).name.c_str();
		}
	}

	strcpy(result, temp);
	return found;
#else  // #ifdef DEBUG
	FS_FileSet::const_iterator i = file_set.begin();
	FS_FileSet::const_iterator const e = file_set.end();
	for (; i != e; ++i)
	{
		if (match_shader(debug_shader_id, full_shader_id, (*i).name.c_str(), (*i).name.size()))
		{
			strcpy(result, (*i).name.c_str());
			return true;
		}
	}

	return false;
#endif // #ifdef DEBUG
}
