#ifndef r_backendH
#define r_backendH
#pragma once

//#define RBackend_PGO

#ifdef RBackend_PGO
#define PGO(a) a
#else
#define PGO(a)
#endif

#include "sh_atomic.h"

#include "r_DStreams.h"
#include "r_constants_cache.h"
#include "r_backend_xform.h"
#include "r_backend_hemi.h"
#include "r_backend_tree.h"

#include "dx11\r_backend_lod.h"

#include "fvf.h"

const u32 CULL_CCW = D3DCULL_CCW;
const u32 CULL_CW = D3DCULL_CW;
const u32 CULL_NONE = D3DCULL_NONE;

enum MaxTextures
{
	//	Actually these values are 128
	mtMaxPixelShaderTextures = 16,
	mtMaxVertexShaderTextures = 4,
	//mtMaxGeometryShaderTextures = 16,
	//mtMaxHullShaderTextures = 16,
	//mtMaxDomainShaderTextures = 16,
	//mtMaxComputeShaderTextures = 16,
};
enum
{
	MaxCBuffers = 14
};
///		detailed statistic
struct R_statistics_element
{
	u32 verts, dips;
	ICF void add(u32 _verts)
	{
		verts += _verts;
		dips++;
	}
};
struct R_statistics
{
	R_statistics_element s_static;
	R_statistics_element s_flora;
	R_statistics_element s_flora_lods;
	R_statistics_element s_details;
	R_statistics_element s_ui;
	R_statistics_element s_dynamic;
	R_statistics_element s_dynamic_sw;
	R_statistics_element s_dynamic_inst;
	R_statistics_element s_dynamic_1B;
	R_statistics_element s_dynamic_2B;
	R_statistics_element s_dynamic_3B;
	R_statistics_element s_dynamic_4B;
};

class ENGINE_API CBackend
{
  public:
	// Dynamic geometry streams
	_VertexStream Vertex;
	_IndexStream Index;
	ID3D11Buffer* QuadIB;
	ID3D11Buffer* old_QuadIB;
	ID3D11Buffer* TriangleFanIB;
	ID3D11Buffer* old_TriangleFanIB;
	ID3D11Buffer* CuboidIB;
	R_xforms xforms;
	R_hemi hemi;
	R_tree tree;
	R_LOD LOD;

	ref_cbuffer m_aVertexConstants[MaxCBuffers];
	ref_cbuffer m_aPixelConstants[MaxCBuffers];
	D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
	ID3D11InputLayout* m_pInputLayout;

  private:
	// Render-targets
	ID3D11RenderTargetView* pRT[4];
	ID3D11DepthStencilView* pZB;

	// Vertices/Indices/etc
	SDeclaration* decl;

	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	u32 vb_stride;

	// Pixel/Vertex constants
	ALIGN(16) R_constants constants;
	R_constant_table* ctable;

	// Shaders/State
	dx10State* state;
	ID3D11PixelShader* ps;
	ID3D11VertexShader* vs;

#ifdef DEBUG
	LPCSTR ps_name;
	LPCSTR vs_name;
#endif
	u32 stencil_enable;
	u32 stencil_func;
	u32 stencil_ref;
	u32 stencil_mask;
	u32 stencil_writemask;
	u32 stencil_fail;
	u32 stencil_pass;
	u32 stencil_zfail;
	u32 colorwrite_mask;
	u32 cull_mode;
	u32 z_enable;
	u32 z_func;
	u32 alpha_ref;

	// Lists
	STextureList* T;
	SMatrixList* M;
	SConstantList* C;

	// Lists-expanded
	CTexture* textures_ps[mtMaxPixelShaderTextures]; // stages
	CTexture* textures_vs[mtMaxVertexShaderTextures]; // 4 vs

#ifdef _EDITOR
	CMatrix* matrices[8]; // matrices are supported only for FFP
#endif

	void Invalidate();

  public:
	struct _stats
	{
		u32 polys;
		u32 verts;
		u32 calls;
		u32 vs;
		u32 ps;
#ifdef DEBUG
		u32 decl;
		u32 vb;
		u32 ib;
		u32 states;	   // Number of times the shader-state changes
		u32 textures;  // Number of times the shader-tex changes
		u32 matrices;  // Number of times the shader-xform changes
		u32 constants; // Number of times the shader-consts changes
#endif
		u32 xforms;
		u32 target_rt;
		u32 target_zb;

		R_statistics r;
	} stat;

  public:
	IC CTexture* get_ActiveTexture(u32 stage)
	{
		if (stage < CTexture::rstVertex)
			return textures_ps[stage];
		//else if (stage < CTexture::rstGeometry)
		else if (stage < CTexture::rstInvalid)
			return textures_vs[stage - CTexture::rstVertex];
		else
		{
			VERIFY(!"Invalid texture stage");
			return 0;
		}
	}

	IC void get_ConstantDirect(shared_str& n, u32 DataSize, void** pVData, void** pGData, void** pPData);

	// API
	IC void set_xform(u32 ID, const Fmatrix& M);
	IC void set_xform_world(const Fmatrix& M);
	IC void set_xform_view(const Fmatrix& M);
	IC void set_xform_project(const Fmatrix& M);
	IC const Fmatrix& get_xform_world();
	IC const Fmatrix& get_xform_view();
	IC const Fmatrix& get_xform_project();

	IC void set_RT(ID3D11RenderTargetView* RT, u32 ID = 0);
	IC void set_ZB(ID3D11DepthStencilView* ZB);
	IC ID3D11RenderTargetView* get_RT(u32 ID = 0);
	IC ID3D11DepthStencilView* get_ZB();

	IC void set_Constants(R_constant_table* C);
	IC void set_Constants(ref_ctable& C)
	{
		set_Constants(&*C);
	}

	void set_Textures(STextureList* T);
	IC void set_Textures(ref_texture_list& T)
	{
		set_Textures(&*T);
	}

#ifdef _EDITOR
	IC void set_Matrices(SMatrixList* M);
	IC void set_Matrices(ref_matrix_list& M)
	{
		set_Matrices(&*M);
	}
#endif

	IC void set_Element(ShaderElement* S, u32 pass = 0);
	IC void set_Element(ref_selement& S, u32 pass = 0)
	{
		set_Element(&*S, pass);
	}

	IC void set_Shader(Shader* S, u32 pass = 0);
	IC void set_Shader(ref_shader& S, u32 pass = 0)
	{
		set_Shader(&*S, pass);
	}

	ICF void set_States(dx10State* _state);
	ICF void set_States(ref_state& _state)
	{
		set_States(_state->state);
	}

	ICF void set_Format(SDeclaration* _decl);

	ICF void set_PS(ID3D11PixelShader* _ps, LPCSTR _n = 0);
	ICF void set_PS(ref_ps& _ps)
	{
		set_PS(_ps->sh, _ps->cName.c_str());
	}


	ICF bool is_TessEnabled();

	ICF void set_VS(ref_vs& _vs);
	ICF void set_VS(SVS* _vs);

  protected: //	In DX10 we need input shader signature which is stored in ref_vs
		 //
	ICF void set_VS(ID3D11VertexShader* _vs, LPCSTR _n = 0);
  public:

	ICF void set_Vertices(ID3D11Buffer* _vb, u32 _vb_stride);
	ICF void set_Indices(ID3D11Buffer* _ib);
	ICF void set_Geometry(SGeometry* _geom);
	ICF void set_Geometry(ref_geom& _geom)
	{
		set_Geometry(&*_geom);
	}
	IC void set_Stencil(u32 _enable, u32 _func = D3DCMP_ALWAYS, u32 _ref = 0x00, u32 _mask = 0x00,
						u32 _writemask = 0x00, u32 _fail = D3DSTENCILOP_KEEP, u32 _pass = D3DSTENCILOP_KEEP,
						u32 _zfail = D3DSTENCILOP_KEEP);
	IC void set_Z(u32 _enable);
	IC void set_ZFunc(u32 _func);
	IC void set_AlphaRef(u32 _value);
	IC void set_ColorWriteEnable(u32 _mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
											 D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	IC void set_CullMode(u32 _mode);
	IC u32 get_CullMode()
	{
		return cull_mode;
	}
	void set_ClipPlanes(u32 _enable, Fplane* _planes = NULL, u32 count = 0);
	void set_ClipPlanes(u32 _enable, Fmatrix* _xform = NULL, u32 fmask = 0xff);
	IC void set_Scissor(Irect* rect = NULL);

	// constants
	ICF ref_constant get_c(LPCSTR n)
	{
		if (ctable)
			return ctable->get(n);
		else
			return 0;
	}
	ICF ref_constant get_c(shared_str& n)
	{
		if (ctable)
			return ctable->get(n);
		else
			return 0;
	}

	// constants - direct (fast)
	ICF void set_c(R_constant* C, const Fmatrix& A)
	{
		if (C)
			constants.set(C, A);
	}
	ICF void set_c(R_constant* C, const Fvector4& A)
	{
		if (C)
			constants.set(C, A);
	}
	ICF void set_c(R_constant* C, float x, float y, float z, float w)
	{
		if (C)
			constants.set(C, x, y, z, w);
	}
	ICF void set_ca(R_constant* C, u32 e, const Fmatrix& A)
	{
		if (C)
			constants.seta(C, e, A);
	}
	ICF void set_ca(R_constant* C, u32 e, const Fvector4& A)
	{
		if (C)
			constants.seta(C, e, A);
	}
	ICF void set_ca(R_constant* C, u32 e, float x, float y, float z, float w)
	{
		if (C)
			constants.seta(C, e, x, y, z, w);
	}
	ICF void set_c(R_constant* C, float A)
	{
		if (C)
			constants.set(C, A);
	}
	ICF void set_c(R_constant* C, int A)
	{
		if (C)
			constants.set(C, A);
	}

	// constants - LPCSTR (slow)
	ICF void set_c(LPCSTR n, const Fmatrix& A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}
	ICF void set_c(LPCSTR n, const Fvector4& A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}
	ICF void set_c(LPCSTR n, float x, float y, float z, float w)
	{
		if (ctable)
			set_c(&*ctable->get(n), x, y, z, w);
	}
	ICF void set_ca(LPCSTR n, u32 e, const Fmatrix& A)
	{
		if (ctable)
			set_ca(&*ctable->get(n), e, A);
	}
	ICF void set_ca(LPCSTR n, u32 e, const Fvector4& A)
	{
		if (ctable)
			set_ca(&*ctable->get(n), e, A);
	}
	ICF void set_ca(LPCSTR n, u32 e, float x, float y, float z, float w)
	{
		if (ctable)
			set_ca(&*ctable->get(n), e, x, y, z, w);
	}
	ICF void set_c(LPCSTR n, float A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}
	ICF void set_c(LPCSTR n, int A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}

	// constants - shared_str (average)
	ICF void set_c(shared_str& n, const Fmatrix& A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}
	ICF void set_c(shared_str& n, const Fvector4& A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}
	ICF void set_c(shared_str& n, float x, float y, float z, float w)
	{
		if (ctable)
			set_c(&*ctable->get(n), x, y, z, w);
	}
	ICF void set_ca(shared_str& n, u32 e, const Fmatrix& A)
	{
		if (ctable)
			set_ca(&*ctable->get(n), e, A);
	}
	ICF void set_ca(shared_str& n, u32 e, const Fvector4& A)
	{
		if (ctable)
			set_ca(&*ctable->get(n), e, A);
	}
	ICF void set_ca(shared_str& n, u32 e, float x, float y, float z, float w)
	{
		if (ctable)
			set_ca(&*ctable->get(n), e, x, y, z, w);
	}
	ICF void set_c(shared_str& n, float A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}
	ICF void set_c(shared_str& n, int A)
	{
		if (ctable)
			set_c(&*ctable->get(n), A);
	}

	ICF void Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);
	ICF void Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC);

	ICF void Compute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);

	// Device create / destroy / frame signaling
	void RestoreQuadIBData(); // Igor: is used to test bug with rain, particles corruption
	void CreateQuadIB();
	void CreateTriangleFanIB();
	void OnFrameBegin();
	void OnFrameEnd();
	void OnDeviceCreate();
	void OnDeviceDestroy();

	// Debug render
	void dbg_DP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 vBase, u32 pc);
	void dbg_DIP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);

	//	TODO: DX10: Implement this.
	IC void dbg_SetRS(D3DRENDERSTATETYPE p1, u32 p2)
	{
		VERIFY(!"Not implemented");
	}
	IC void dbg_SetSS(u32 sampler, D3DSAMPLERSTATETYPE type, u32 value)
	{
		VERIFY(!"Not implemented");
	}

#ifdef DEBUG
	void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, u16* pIdx, int pcnt);
	void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);
	IC void dbg_DrawAABB(Fvector& T, float sx, float sy, float sz, u32 C)
	{
		Fvector half_dim;
		half_dim.set(sx, sy, sz);
		Fmatrix TM;
		TM.translate(T);
		dbg_DrawOBB(TM, half_dim, C);
	}
	void dbg_DrawOBB(Fmatrix& T, Fvector& half_dim, u32 C);
	IC void dbg_DrawTRI(Fmatrix& T, Fvector* p, u32 C)
	{
		dbg_DrawTRI(T, p[0], p[1], p[2], C);
	}
	void dbg_DrawTRI(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C);
	void dbg_DrawLINE(Fmatrix& T, Fvector& p1, Fvector& p2, u32 C);
	void dbg_DrawEllipse(Fmatrix& T, u32 C);
#endif

	CBackend()
	{
		Invalidate();
	};

	//	DirectX 10 internal functionality
	// void CreateConstantBuffers();
	// void DestroyConstantBuffers();
	void ApplyVertexLayout();
	void ApplyRTandZB();
	void ApplyPrimitieTopology(D3D_PRIMITIVE_TOPOLOGY Topology);
	bool CBuffersNeedUpdate(ref_cbuffer buf1[MaxCBuffers], ref_cbuffer buf2[MaxCBuffers], u32& uiMin, u32& uiMax);

  private:
	ID3DBlob* m_pInputSignature;

	bool m_bChangedRTorZB;
};

extern ENGINE_API CBackend RCache;

#ifndef _EDITOR
#include "D3DUtils.h"
#endif

#endif
