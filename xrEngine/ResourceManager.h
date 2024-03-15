// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ResourceManagerH
#define ResourceManagerH
#pragma once

#include "shader.h"
#include "tss_def.h"
#include "TextureDescrManager.h"
#include "shadermacros.h"
// refs
struct lua_State;

class dx10ConstantBuffer;

// defs
class ENGINE_API CResourceManager
{
  private:
	struct str_pred : public std::binary_function<char*, char*, bool>
	{
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{
			return xr_strcmp(x, y) < 0;
		}
	};
	struct texture_detail
	{
		const char* T;
		R_constant_setup* cs;
	};

  public:
	DEFINE_MAP_PRED(const char*, IBlender*, map_Blender, map_BlenderIt, str_pred);
	DEFINE_MAP_PRED(const char*, CTexture*, map_Texture, map_TextureIt, str_pred);
	DEFINE_MAP_PRED(const char*, CMatrix*, map_Matrix, map_MatrixIt, str_pred);
	DEFINE_MAP_PRED(const char*, CConstant*, map_Constant, map_ConstantIt, str_pred);
	DEFINE_MAP_PRED(const char*, CRT*, map_RT, map_RTIt, str_pred);
	DEFINE_MAP_PRED(const char*, SVS*, map_VS, map_VSIt, str_pred);
	DEFINE_MAP_PRED(const char*, SPS*, map_PS, map_PSIt, str_pred);
	DEFINE_MAP_PRED(const char*, texture_detail, map_TD, map_TDIt, str_pred);

  private:
	// data
	map_Blender m_blenders;
	map_Texture m_textures;
	map_Matrix m_matrices;
	map_Constant m_constants;
	map_RT m_rtargets;
	//	DX10 cut map_RTC												m_rtargets_c;
	map_VS m_vs;
	map_PS m_ps;
	map_TD m_td;

	xr_vector<SState*> v_states;
	xr_vector<SDeclaration*> v_declarations;
	xr_vector<SGeometry*> v_geoms;
	xr_vector<R_constant_table*> v_constant_tables;

	xr_vector<dx10ConstantBuffer*> v_constant_buffer;
	xr_vector<SInputSignature*> v_input_signature;

	// lists
	xr_vector<STextureList*> lst_textures;
	xr_vector<SMatrixList*> lst_matrices;
	xr_vector<SConstantList*> lst_constants;

	// main shader-array
	xr_vector<SPass*> v_passes;
	xr_vector<ShaderElement*> v_elements;
	xr_vector<Shader*> v_shaders;

	xr_vector<ref_texture> m_necessary;
	// misc
  public:
	CTextureDescrMngr m_textures_description;
	//.	CInifile*											m_textures_description;
	xr_vector<std::pair<shared_str, R_constant_setup*>> v_constant_setup;
	lua_State* LSVM;
	BOOL bDeferredLoad;

  private:
	void LS_Load();
	void LS_Unload();

  public:
	// Miscelaneous
	void _ParseList(sh_list& dest, LPCSTR names);
	IBlender* _GetBlender(LPCSTR Name);
	IBlender* _FindBlender(LPCSTR Name);
	void _GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
	void _DumpMemoryUsage();
	//.	BOOL							_GetDetailTexture	(LPCSTR Name, LPCSTR& T, R_constant_setup* &M);

	map_Blender& _GetBlenders()
	{
		return m_blenders;
	}

	// Debug
	void DBG_VerifyGeoms();
	void DBG_VerifyTextures();

	// Editor cooperation
	void ED_UpdateBlender(LPCSTR Name, IBlender* data);
	void ED_UpdateMatrix(LPCSTR Name, CMatrix* data);
	void ED_UpdateConstant(LPCSTR Name, CConstant* data);
#ifdef _EDITOR
	void ED_UpdateTextures(AStringVec* names);
#endif

	// Low level resource creation
	CTexture* _CreateTexture(LPCSTR Name);
	void _DeleteTexture(const CTexture* T);

	CMatrix* _CreateMatrix(LPCSTR Name);
	void _DeleteMatrix(const CMatrix* M);

	CConstant* _CreateConstant(LPCSTR Name);
	void _DeleteConstant(const CConstant* C);

	R_constant_table* _CreateConstantTable(R_constant_table& C);
	void _DeleteConstantTable(const R_constant_table* C);

	dx10ConstantBuffer* _CreateConstantBuffer(ID3D11ShaderReflectionConstantBuffer* pTable);
	void _DeleteConstantBuffer(const dx10ConstantBuffer* pBuffer);

	SInputSignature* _CreateInputSignature(ID3DBlob* pBlob);
	void _DeleteInputSignature(const SInputSignature* pSignature);

	CRT* _CreateRT(LPCSTR name, u32 w, u32 h, DXGI_FORMAT f, VIEW_TYPE view, u32 samples);

	void _DeleteRT(const CRT* RT);

	SPass* _CreatePass(const SPass& proto);
	void _DeletePass(const SPass* P);

	// Shader compiling / optimizing
	SState* _CreateState(SimulatorStates& Code);
	void _DeleteState(const SState* SB);

	SDeclaration* _CreateDecl(D3DVERTEXELEMENT9* dcl);
	void _DeleteDecl(const SDeclaration* dcl);

	STextureList* _CreateTextureList(STextureList& L);
	void _DeleteTextureList(const STextureList* L);

	SMatrixList* _CreateMatrixList(SMatrixList& L);
	void _DeleteMatrixList(const SMatrixList* L);

	SConstantList* _CreateConstantList(SConstantList& L);
	void _DeleteConstantList(const SConstantList* L);

	ShaderElement* _CreateElement(ShaderElement& L);
	void _DeleteElement(const ShaderElement* L);

	Shader* _cpp_Create(LPCSTR s_shader, LPCSTR s_textures = 0, LPCSTR s_constants = 0, LPCSTR s_matrices = 0);
	Shader* _cpp_Create(IBlender* B, LPCSTR s_shader = 0, LPCSTR s_textures = 0, LPCSTR s_constants = 0,
						LPCSTR s_matrices = 0);
	Shader* _lua_Create(LPCSTR s_shader, LPCSTR s_textures);
	BOOL _lua_HasShader(LPCSTR s_shader);

	CResourceManager() : bDeferredLoad(TRUE)
	{
	}
	~CResourceManager();

	void OnDeviceCreate(IReader* F);
	void OnDeviceCreate(LPCSTR name);
	void OnDeviceDestroy(BOOL bKeepTextures);

	void reset_begin();
	void reset_end();

	// Creation/Destroying
	Shader* Create(LPCSTR s_shader = 0, LPCSTR s_textures = 0, LPCSTR s_constants = 0, LPCSTR s_matrices = 0);
	Shader* Create(IBlender* B, LPCSTR s_shader = 0, LPCSTR s_textures = 0, LPCSTR s_constants = 0,
				   LPCSTR s_matrices = 0);
	void Delete(const Shader* S);
	void RegisterConstantSetup(LPCSTR name, R_constant_setup* s)
	{
		v_constant_setup.push_back(mk_pair(shared_str(name), s));
	}

	SGeometry* CreateGeom(D3DVERTEXELEMENT9* decl, ID3D11Buffer* vb, ID3D11Buffer* ib);
	SGeometry* CreateGeom(u32 FVF, ID3D11Buffer* vb, ID3D11Buffer* ib);
	void DeleteGeom(const SGeometry* VS);
	void DeferredLoad(BOOL E)
	{
		bDeferredLoad = E;
	}
	void DeferredUpload();
	void DeferredUnload();
	void Evict();
	void StoreNecessaryTextures();
	void DestroyNecessaryTextures();
	void Dump(bool bBrief);

	template <typename T> T& GetShaderMap();
	template <typename T> T* FindShader(const char* _name);
	template <typename T> T* RegisterShader(const char* _name);
	template <typename T> HRESULT CompileShader(LPCSTR name, LPCSTR ext, LPCSTR src, 
		UINT size, LPCSTR target, LPCSTR entry, CShaderMacros& macros, T*& result);
	template <typename T> T* CreateShader(const char* _name, CShaderMacros& macros);
	template <typename T> void DestroyShader(const T* sh);
	template <typename T> HRESULT ReflectShader(DWORD const* src, UINT size, T*& result);
	template <typename T> void CreateSignature(DWORD const* src, UINT size, T*& result);
};

template SPS* CResourceManager::CreateShader<SPS>(LPCSTR _name, CShaderMacros& macros);
template SVS* CResourceManager::CreateShader<SVS>(LPCSTR _name, CShaderMacros& macros);

template void CResourceManager::DestroyShader<SPS>(const SPS* sh);
template void CResourceManager::DestroyShader<SVS>(const SVS* sh);

#endif // ResourceManagerH
