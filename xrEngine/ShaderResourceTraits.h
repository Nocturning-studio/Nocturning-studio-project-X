#pragma once

#include "ResourceManager.h"

template <typename T> struct ShaderTypeTraits;
/*
template <> struct ShaderTypeTraits<SVS>
{
	typedef CResourceManager::map_VS Map_S;
	typedef IDirect3DVertexShader9 ID3DShader;

	static inline u32 GetShaderDest()
	{
		return RC_dest_vertex;
	}
	static inline LPCSTR GetShaderExt()
	{
		return "vs";
	}
	static inline BOOL IsSupported()
	{
		return TRUE;
	}

	static inline ID3DShader* D3DCreateShader(DWORD const* buffer, size_t size)
	{
		ID3DShader* s = 0;
#pragma message(Reminder("Not implemented!"))
		// R_CHK(HW.pDevice->CreateVertexShader(buffer, &s));
		return s;
	}
};

template <> struct ShaderTypeTraits<SPS>
{
	typedef CResourceManager::map_PS Map_S;
	typedef IDirect3DPixelShader9 ID3DShader;

	static inline u32 GetShaderDest()
	{
		return RC_dest_pixel;
	}
	static inline LPCSTR GetShaderExt()
	{
		return "ps";
	}
	static inline BOOL IsSupported()
	{
		return TRUE;
	}

	static inline ID3DShader* D3DCreateShader(DWORD const* buffer, size_t size)
	{
		ID3DShader* s = 0;
#pragma message(Reminder("Not implemented!"))
		// R_CHK(HW.pDevice->CreatePixelShader(buffer, &s));
		return s;
	}
};
*/

template <> inline CResourceManager::map_VS& CResourceManager::GetShaderMap()
{
	return m_vs;
}
template <> inline CResourceManager::map_PS& CResourceManager::GetShaderMap()
{
	return m_ps;
}


template <> struct ShaderTypeTraits<SVS>
{
	typedef CResourceManager::map_VS Map_S;
	typedef ID3D11VertexShader ID3DShader;

	static inline u32 GetShaderDest()
	{
		return RC_dest_vertex;
	}
	static inline LPCSTR GetShaderExt()
	{
		return "vs";
	}
	static inline LPCSTR GetShaderTarget()
	{
		return "vs_4_0_level_9_3";
	}
	static inline BOOL IsSupported()
	{
		// hack
		return TRUE;
	}

	static inline ID3DShader* D3DCreateShader(DWORD const* buffer, size_t size)
	{
		ID3DShader* s = 0;
		R_CHK(HW.pDevice11->CreateVertexShader(buffer, size, NULL, &s));
		return s;
	}
};

template <> struct ShaderTypeTraits<SPS>
{
	typedef CResourceManager::map_PS Map_S;
	typedef ID3D11PixelShader ID3DShader;

	static inline u32 GetShaderDest()
	{
		return RC_dest_pixel;
	}
	static inline LPCSTR GetShaderExt()
	{
		return "ps";
	}
	static inline LPCSTR GetShaderTarget()
	{
		return "ps_4_0_level_9_3";
	}
	static inline BOOL IsSupported()
	{
		// hack
		return TRUE;
	}

	static inline ID3DShader* D3DCreateShader(DWORD const* buffer, size_t size)
	{
		ID3DShader* s = 0;
		R_CHK(HW.pDevice11->CreatePixelShader(buffer, size, NULL, &s));
		return s;
	}
};
