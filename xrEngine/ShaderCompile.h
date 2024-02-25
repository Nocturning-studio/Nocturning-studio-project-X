#pragma once

#include "ShaderMacros.h"

//----------------------------------------------------------------
class CShaderIncluder : public ID3DXInclude
{
public:
	HRESULT __stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
	{
		string_path pname;
		strconcat(sizeof(pname), pname, ::Render->getShaderPath(), pFileName);
		IReader* R = FS.r_open("$game_shaders$", pname);
		if (0 == R) {
			// possibly in shared directory or somewhere else - open directly
			R = FS.r_open("$game_shaders$", pFileName);
			if (0 == R) return E_FAIL;
		}
		u32 size = R->length();
		u8* data = xr_alloc<u8>(size + 1);
		CopyMemory(data, R->pointer(), size);
		data[size] = 0;
		FS.r_close(R);
		*ppData = data;
		*pBytes = size;
		return	D3D_OK;
	}

	HRESULT __stdcall	Close(LPCVOID	pData)
	{
		xr_free(pData);
		return	D3D_OK;
	}
};

//----------------------------------------------------------------
template<typename T>
T* CResourceManager::RegisterShader(const char* _name)
{
	T* sh = xr_new<T>();
	sh->dwFlags |= xr_resource_flagged::RF_REGISTERED;
	ShaderTypeTraits<T>::Map_S& sh_map = GetShaderMap<ShaderTypeTraits<T>::Map_S>();
	sh_map.insert(mk_pair(sh->set_name(_name), sh));
	return sh;
}

template<typename T>
T* CResourceManager::FindShader(const char* _name)
{
	ShaderTypeTraits<T>::Map_S& sh_map = GetShaderMap<ShaderTypeTraits<T>::Map_S>();
	ShaderTypeTraits<T>::Map_S::iterator I = sh_map.find(_name);

	if (I != sh_map.end())
		return I->second;

	// if the shader is not supported or not exist
	if (!ShaderTypeTraits<T>::IsSupported() ||
		(xr_strlen(_name) >= 4 &&
		_name[0] == 'n' && _name[1] == 'u' && 
		_name[2] == 'l' && _name[3] == 'l'))
	{
		T* sh = RegisterShader<T>("null");
		sh->sh = NULL;
		return sh;
	}

	return NULL;
}

void print_macros(CShaderMacros& macros, LPCSTR name, LPCSTR ext)
{
	if (macros.get_name().length() < 1)
		return;

	Msg("*   cache: %s.%s", name, ext);
	Msg("*   macro count: %d", macros.get_name().length());

	for (u32 i = 0; i < macros.get_impl().size(); ++i)
	{
		CShaderMacros::MacroImpl m = macros.get_impl()[i];
		Msg("*   macro: (%s : %s : %c)", m.Name, m.Definition, m.Status);
	}
}

template<typename T>
T* CResourceManager::CreateShader(const char* _name)
{
	// get shader macros
	CShaderMacros macros = ::Render->FetchShaderMacros();

	// make the unique shader name
	string_path name;
	sprintf_s(name, sizeof name, "%s%s", _name, macros.get_name().c_str());

	// if the shader is already exist, return it
	T* sh = FindShader<T>(name);
	if (sh) return sh;

	// create a new shader
	sh = RegisterShader<T>(name);

	// open file
	const char* ext = ShaderTypeTraits<T>::GetShaderExt();
	string_path	file_source;
	sprintf_s(file_source, sizeof file_source, "%s%s.%s", ::Render->getShaderPath(), _name, ext);
	FS.update_path(file_source, "$game_shaders$", file_source);
	IReader* file = FS.r_open(file_source);
	R_ASSERT2(file, file_source);

	// select target
	string32 c_target, c_entry;
	sprintf_s(c_entry, sizeof c_entry, "main");
	sprintf_s(c_target, sizeof c_target, "%s_%u_%u", ext, HW.Caps.raster_major, HW.Caps.raster_minor);

	Msg("* Compiling shader: target=%s, source=%s.%s", c_target, _name, ext);

	// compile and create
	HRESULT _hr = CompileShader(_name, ext, (LPCSTR)file->pointer(), file->length(), c_target, c_entry, macros, (T*&)sh);
	R_ASSERT(!FAILED(_hr));

	FS.r_close(file);

	return sh;
}

#include <boost/crc.hpp>

template<typename T>
HRESULT CResourceManager::CompileShader(
	LPCSTR			name,
	LPCSTR			ext,
	LPCSTR			src,
	UINT			size,
	LPCSTR			target,
	LPCSTR			entry,
	CShaderMacros&	macros,
	T*&				result)
{
	string_path cache_dest;
	sprintf_s(cache_dest, sizeof cache_dest, "shaders_cache\\%s%s.%s\\%s", ::Render->getShaderPath(), name, ext, macros.get_name().c_str());
	FS.update_path(cache_dest, "$app_data_root$", cache_dest);

	//print_macros(macros, cache_dest, ext);

	CShaderIncluder		Includer;
	ID3DXBuffer*		pShaderBuf = NULL;
	ID3DXBuffer*		pErrorBuf = NULL;
	ID3DXConstantTable* pConstants	= NULL;
	
	u32 flags = D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_OPTIMIZATION_LEVEL3;
	
	HRESULT _result = D3DXCompileShader(src, size, &macros.get_macros()[0], &Includer, entry, target, flags, &pShaderBuf, &pErrorBuf, &pConstants);
	
	if (SUCCEEDED(_result))
	{
		IWriter* file			= FS.w_open(cache_dest);

		boost::crc_32_type		processor;
		processor.process_block	( pShaderBuf->GetBufferPointer(), ((char*)pShaderBuf->GetBufferPointer()) + pShaderBuf->GetBufferSize() );
		u32 const crc			= processor.checksum( );

		file->w_u32				(crc);
		file->w					(pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize());
		FS.w_close				(file);

		_result					= ReflectShader((DWORD*)pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize(), result);

		if (FAILED(_result))
		{
			Msg("! D3DReflectShader %s.%s hr == 0x%08x", name, ext, _result);
			R_ASSERT(NULL);
		}

		bool disasm = strstr(Core.Params, "-disasm") ? true : false;

		if (disasm)
		{
			ID3DXBuffer* pDisasm = 0;
			D3DXDisassembleShader((DWORD*)pShaderBuf->GetBufferPointer(), TRUE, 0, &pDisasm);
			string_path disasm_dest;
			sprintf_s(disasm_dest, sizeof disasm_dest, "shaders_disasm\\%s%s.%s\\%s.html", 
				::Render->getShaderPath(), name, ext, macros.get_name().c_str());
			IWriter* W = FS.w_open("$app_data_root$", disasm_dest);
			W->w(pDisasm->GetBufferPointer(), pDisasm->GetBufferSize());
			FS.w_close(W);
			_RELEASE(pDisasm);
		}
	}
	else
	{
		string16 code;
		//xr_sprintf(code, sizeof code, "hr=0x%08x", _result);
		sprintf_s(code, sizeof code, "hr=0x%08x", _result);

		std::string message = std::string(pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "");

		std::string error = make_string("!Can't compile shader %s\nfile: %s.%s, target: %s\n", code, name, ext, target);
		error += message;

		Log(error.c_str());
		FlushLog();
		CHECK_OR_EXIT(!FAILED(_result), error);
	}

	return _result;
}

template<typename T>
HRESULT CResourceManager::ReflectShader(
	DWORD const*	src,
	UINT			size,
	T*&				result)
{
	result->sh = ShaderTypeTraits<T>::D3DCreateShader(src, size);

	const void* pReflection = 0;
	HRESULT _hr = D3DXFindShaderComment(src, MAKEFOURCC('C', 'T', 'A', 'B'), &pReflection, NULL);
	
	if (SUCCEEDED(_hr) && pReflection)
	{
		result->constants.parse((void*)pReflection, ShaderTypeTraits<T>::GetShaderDest());
		return _hr;
	}

	return E_FAIL;
}

template<typename T>
void CResourceManager::DestroyShader(const T* sh)
{
	ShaderTypeTraits<T>::Map_S& sh_map = GetShaderMap<ShaderTypeTraits<T>::Map_S>();

	if (0==(sh->dwFlags&xr_resource_flagged::RF_REGISTERED))
		return;

	LPSTR N = LPSTR(*sh->cName);
	typename ShaderTypeTraits<T>::Map_S::iterator I = sh_map.find(N);
		
	if (I!=sh_map.end())
	{
		sh_map.erase(I);
		return;
	}
}
