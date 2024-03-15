#ifndef SH_RT_H
#define SH_RT_H
#pragma once

enum VIEW_TYPE
{
	// SRV = 1 << 1, // ShaderResource
	// RTV = 1 << 2, // RenderTarget
	// DSV = 1 << 3, // DepthStencil
	// UAV = 1 << 4, // UnorderedAcces
	SRV_RTV,	 // ShaderResource & RenderTarget
	SRV_RTV_UAV, // ShaderResource & RenderTarget & UnorderedAcces
	SRV_RTV_DSV, // ShaderResource & RenderTarget & DepthStencil
	SRV_DSV,	 // ShaderResource & DepthStencil
};

//////////////////////////////////////////////////////////////////////////
class ENGINE_API CRT : public xr_resource_named
{
  public:
	CRT();
	~CRT();

	void create(LPCSTR name, u32 w, u32 h, DXGI_FORMAT f, VIEW_TYPE view, u32 samples);
	void destroy();
	void reset_begin();
	void reset_end();
	IC BOOL valid()
	{
		return !!pTexture;
	}

  public:
	ID3D11DepthStencilView* pZRT;
	ID3D11UnorderedAccessView* pUAView;
	DXGI_FORMAT format;
	VIEW_TYPE view;
	u32 samples;

	u32 dwWidth;
	u32 dwHeight;

	ID3D11Texture2D* pSurface;
	ID3D11RenderTargetView* pRT;
	ref_texture pTexture;

	u64 _order;
};
struct ENGINE_API resptrcode_crt : public resptr_base<CRT>
{
	// Depth Stencil view type required DXGI_FORMAT_D...
	void create(LPCSTR name, u32 w, u32 h, DXGI_FORMAT f, VIEW_TYPE view, u32 samples = 1);

	void destroy()
	{
		_set(NULL);
	}
};
typedef resptr_core<CRT, resptrcode_crt> ref_rt;


#endif // SH_RT_H
