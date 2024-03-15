// HW.cpp: implementation of the CHW class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable : 4995)
#include <d3dx9.h>
#pragma warning(default : 4995)
#include "HW.h"
#include "xr_IOconsole.h"

#include "dx11\StateCache.h"
#include "dx11\SamplerStateCache.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#ifndef _EDITOR
void fill_vid_mode_list(CHW* _hw);
void free_vid_mode_list();
#else
void fill_vid_mode_list(CHW* _hw){};
void free_vid_mode_list(){};
#endif

void free_vid_mode_list();

ENGINE_API CHW HW;

#ifdef DEBUG
IDirect3DStateBlock9* dwDebugSB = 0;
#endif

void CHW::Reset(HWND hwnd)
{
#ifdef DEBUG
	_RELEASE(dwDebugSB);
#endif
	_RELEASE(pBaseZB);
	_RELEASE(pBaseRT);

#ifndef _EDITOR
#ifndef DEDICATED_SERVER
	//BOOL bWindowed = strstr(Core.Params, "-windowed") ? TRUE : !psDeviceFlags.is(rsFullscreen);
#else
	BOOL bWindowed = TRUE;
#endif

	//selectResolution(DevPP.BackBufferWidth, DevPP.BackBufferHeight, bWindowed);
	DXGI_MODE_DESC& desc = m_ChainDesc.BufferDesc;
	selectResolution(desc.Width, desc.Height, TRUE);

	// Windoze
	/* DevPP.SwapEffect = bWindowed ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
	DevPP.Windowed = bWindowed;
	DevPP.PresentationInterval = selectPresentInterval();
	if (!bWindowed)
		DevPP.FullScreen_RefreshRateInHz = selectRefresh(DevPP.BackBufferWidth, DevPP.BackBufferHeight, Caps.fTarget);
	else
		DevPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;*/

	desc.RefreshRate.Numerator = 60;
	desc.RefreshRate.Denominator = 1;

	CHK_DX(m_pSwapChain->ResizeTarget(&desc));
#endif

	/* while (TRUE)
	{
		HRESULT _hr = HW.pDevice->Reset(&DevPP);
		if (SUCCEEDED(_hr))
			break;
		Msg("! ERROR: [%dx%d]: %s", DevPP.BackBufferWidth, DevPP.BackBufferHeight, Debug.error2string(_hr));
		Sleep(100);
	}*/
	//R_CHK(pDevice->GetRenderTarget(0, &pBaseRT));
	//R_CHK(pDevice->GetDepthStencilSurface(&pBaseZB));
	_SHOW_REF("refCount:pBaseZB", pBaseZB);
	_SHOW_REF("refCount:pBaseRT", pBaseRT);

	_RELEASE(pBaseZB);
	_RELEASE(pBaseRT);

	DXGI_SWAP_CHAIN_DESC& cd = m_ChainDesc;
	cd.Windowed = TRUE;

	CHK_DX(m_pSwapChain->ResizeBuffers(cd.BufferCount, desc.Width, desc.Height, desc.Format,
									   DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	DXGI_SWAP_CHAIN_DESC& sd = m_ChainDesc;
	HRESULT R;

	ID3D11Texture2D* pBuffer;
	R = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBuffer);
	R_CHK(R);

	R = pDevice11->CreateRenderTargetView(pBuffer, NULL, &pBaseRT);
	pBuffer->Release();
	R_CHK(R);

	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = sd.BufferDesc.Width;
	descDepth.Height = sd.BufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	R = pDevice11->CreateTexture2D(&descDepth,	  // Texture desc
								 NULL,			  // Initial data
								 &pDepthStencil); // [out] Texture
	R_CHK(R);

	//	Create Depth/stencil view
	R = pDevice11->CreateDepthStencilView(pDepthStencil, NULL, &pBaseZB);
	R_CHK(R);

	pDepthStencil->Release();
#ifdef DEBUG
	R_CHK(pDevice->CreateStateBlock(D3DSBT_ALL, &dwDebugSB));
#endif
#ifndef _EDITOR
	updateWindowProps(hwnd);
#endif
}

xr_token* vid_mode_token = NULL;

void CHW::CreateD3D()
{
	/*
#ifndef DEDICATED_SERVER
	LPCSTR _name = "d3d9.dll";
#else
	LPCSTR _name = "xrd3d9-null.dll";
#endif

	hD3D9 = LoadLibrary(_name);
	if (!hD3D9)
		make_string("Can't find 'd3d9.dll'\nPlease install latest version of DirectX before running this program");

	typedef IDirect3D9* WINAPI _Direct3DCreate9(UINT SDKVersion);
	_Direct3DCreate9* createD3D = (_Direct3DCreate9*)GetProcAddress(hD3D9, "Direct3DCreate9");
	R_ASSERT2(createD3D, "There was a problem with Direct3DCreate9");
	this->pD3D = createD3D(D3D_SDK_VERSION);
	if (!this->pD3D)
		make_string("Please install DirectX 9.0c");
		*/
}

void CHW::DestroyD3D()
{
	//_RELEASE(this->pD3D);
	//FreeLibrary(hD3D9);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
D3DFORMAT CHW::selectDepthStencil(D3DFORMAT fTarget)
{
	return D3DFMT_D24S8;

	static D3DFORMAT fDS_Try1[6] = {D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32,
									   D3DFMT_D24X8, D3DFMT_D16,	 D3DFMT_D15S1};

	D3DFORMAT* fDS_Try = fDS_Try1;
	int fDS_Cnt = 6;

	/* for (int it = 0; it < fDS_Cnt; it++)
	{
		if (SUCCEEDED(pD3D->CheckDeviceFormat(DevAdapter, DevT, fTarget, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
											  fDS_Try[it])))
		{
			if (SUCCEEDED(pD3D->CheckDepthStencilMatch(DevAdapter, DevT, fTarget, fTarget, fDS_Try[it])))
			{
				return fDS_Try[it];
			}
		}
	}*/
	return D3DFMT_UNKNOWN;
}

void CHW::DestroyDevice()
{
	//	Destroy state managers
	StateManager.Reset();
	RSManager.ClearStateArray();
	DSSManager.ClearStateArray();
	BSManager.ClearStateArray();
	SSManager.ClearStateArray();

	_SHOW_REF("refCount:pBaseZB", pBaseZB);
	_RELEASE(pBaseZB);

	_SHOW_REF("refCount:pBaseRT", pBaseRT);
	_RELEASE(pBaseRT);
#ifdef DEBUG
	_SHOW_REF("refCount:dwDebugSB", dwDebugSB);
	_RELEASE(dwDebugSB);
#endif
#ifdef _EDITOR
	_RELEASE(HW.pDevice);
#else 
	//if (!m_ChainDesc.Windowed) m_pSwapChain->SetFullscreenState(FALSE, NULL);
	_SHOW_REF("refCount:m_pSwapChain", m_pSwapChain);
	_RELEASE(m_pSwapChain);

	_SHOW_REF("ContextREF:", HW.pContext);
	_RELEASE(HW.pContext);

	_SHOW_REF("DeviceREF:", HW.pDevice11);
	_RELEASE(HW.pDevice11);

	_SHOW_REF("AdapterREF:", HW.pAdapter);
	_RELEASE(HW.pAdapter);
#endif
	DestroyD3D();

#ifndef _EDITOR
	free_vid_mode_list();
#endif
}
void CHW::selectResolution(u32& dwWidth, u32& dwHeight, BOOL bWindowed)
{
	fill_vid_mode_list(this);
#ifdef DEDICATED_SERVER
	dwWidth = 640;
	dwHeight = 480;
#else
	//if (bWindowed)
	{
		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
	}
	//else // check
	/* {
#ifndef _EDITOR
		string64 buff;
		sprintf_s(buff, sizeof(buff), "%dx%d", psCurrentVidMode[0], psCurrentVidMode[1]);

		if (_ParseItem(buff, vid_mode_token) == u32(-1)) // not found
		{												 // select safe
			sprintf_s(buff, sizeof(buff), "vid_mode %s", vid_mode_token[0].name);
			Console->Execute(buff);
		}

		dwWidth = psCurrentVidMode[0];
		dwHeight = psCurrentVidMode[1];
#endif
	}*/
#endif
}

void CHW::CreateDevice(HWND m_hWnd)
{
	CreateD3D();

	// General - select adapter and device
#ifdef DEDICATED_SERVER
	BOOL bWindowed = TRUE;
#else
	//BOOL bWindowed = !psDeviceFlags.is(rsFullscreen);
#endif

	//DevAdapter = D3DADAPTER_DEFAULT;
	//DevT = Caps.bForceGPU_REF ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;

	//. #ifdef DEBUG
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	/* for (UINT Adapter = 0; Adapter < pD3D->GetAdapterCount(); Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res = pD3D->GetAdapterIdentifier(Adapter, 0, &Identifier);
		if (SUCCEEDED(Res) && (xr_strcmp(Identifier.Description, "NVIDIA NVPerfHUD") == 0))
		{
			DevAdapter = Adapter;
			DevT = D3DDEVTYPE_REF;
			break;
		}
	}*/
	//. #endif

	// DX11 DXGI
	IDXGIFactory1* pFactory11;
	R_CHK(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory11)));
	pAdapter = 0;

	if (!pAdapter)
		pFactory11->EnumAdapters1(0, &pAdapter);

	// Display the name of video board
	 D3DADAPTER_IDENTIFIER9 adapterID;
	//R_CHK(pD3D->GetAdapterIdentifier(DevAdapter, 0, &adapterID));
	//Msg("* GPU [vendor:%X]-[device:%X]: %s", adapterID.VendorId, adapterID.DeviceId, adapterID.Description);

	u16 drv_Product = HIWORD(adapterID.DriverVersion.HighPart);
	u16 drv_Version = LOWORD(adapterID.DriverVersion.HighPart);
	u16 drv_SubVersion = HIWORD(adapterID.DriverVersion.LowPart);
	u16 drv_Build = LOWORD(adapterID.DriverVersion.LowPart);
	//Msg("* GPU driver: %d.%d.%d.%d", u32(drv_Product), u32(drv_Version), u32(drv_SubVersion), u32(drv_Build));

	//Caps.id_vendor = adapterID.VendorId;
	//Caps.id_device = adapterID.DeviceId;

	DXGI_ADAPTER_DESC1 Desc11;
	R_CHK(pAdapter->GetDesc1(&Desc11));
	Msg("* GPU [vendor:%X]-[device:%X]: %S", Desc11.VendorId, Desc11.DeviceId, Desc11.Description);

	Caps.id_vendor = Desc11.VendorId;
	Caps.id_device = Desc11.DeviceId;
	// Retreive windowed mode
	 D3DDISPLAYMODE mWindowed;
	//R_CHK(pD3D->GetAdapterDisplayMode(DevAdapter, &mWindowed));

	// Select back-buffer & depth-stencil format
	D3DFORMAT& fTarget = Caps.fTarget;
	D3DFORMAT& fDepth = Caps.fDepth;
	if (TRUE)
	{
		fTarget = D3DFMT_X8B8G8R8;
		//R_CHK(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, TRUE));
		fDepth = selectDepthStencil(fTarget);
	}
	 else
	{
		 /* switch (psCurrentBPP)
		{
		case 32:
			fTarget = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, FALSE)))
				break;
			fTarget = D3DFMT_A8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, FALSE)))
				break;
			fTarget = D3DFMT_R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		case 16:
		default:
			fTarget = D3DFMT_R5G6B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, FALSE)))
				break;
			fTarget = D3DFMT_X1R5G5B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, FALSE)))
				break;
			fTarget = D3DFMT_X4R4G4B4;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter, DevT, fTarget, fTarget, FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		}*/
		fDepth = selectDepthStencil(fTarget);
		fTarget = D3DFMT_X8B8G8R8;
	}
	
	if ((D3DFMT_UNKNOWN == fTarget) || (D3DFMT_UNKNOWN == fTarget))
	{
		Msg("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		FlushLog();
		MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!",
				   MB_OK | MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
	}

	// Set up the presentation parameters
	//D3DPRESENT_PARAMETERS& P = DevPP;
	//ZeroMemory(&P, sizeof(P));

#ifndef _EDITOR
	//selectResolution(P.BackBufferWidth, P.BackBufferHeight, bWindowed);
#endif
	DXGI_SWAP_CHAIN_DESC& P = m_ChainDesc;
	ZeroMemory(&P, sizeof(P));
	selectResolution(P.BufferDesc.Width, P.BufferDesc.Height, TRUE);

	// Back buffer
	//.	P.BackBufferWidth		= dwWidth;
	//. P.BackBufferHeight		= dwHeight;
	//P.BackBufferFormat = fTarget;
	//P.BackBufferCount = 1;
	P.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	P.BufferCount = 1;

	// Multisample
	//P.MultiSampleType = D3DMULTISAMPLE_NONE;
	//P.MultiSampleQuality = 0;
	P.SampleDesc.Count = 1;
	P.SampleDesc.Quality = 0;

	// Windoze
	//P.SwapEffect = bWindowed ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
	//P.hDeviceWindow = m_hWnd;
	//P.Windowed = bWindowed;
	P.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	P.OutputWindow = m_hWnd;
	P.Windowed = TRUE;

	// Depth/stencil
	//P.EnableAutoDepthStencil = TRUE;
	//P.AutoDepthStencilFormat = fDepth;
	P.Flags = 0; //. D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	//// Refresh rate
	//P.PresentationInterval = selectPresentInterval();
	//if (!bWindowed)
	//	P.FullScreen_RefreshRateInHz = selectRefresh(P.BackBufferWidth, P.BackBufferHeight, fTarget);
	//else
	//	P.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	P.BufferDesc.RefreshRate.Numerator = 60;
	P.BufferDesc.RefreshRate.Denominator = 1;

	P.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Create the device
	u32 GPU = selectGPU();
	/* HRESULT R = HW.pD3D->CreateDevice(DevAdapter, DevT, m_hWnd,
									  GPU | D3DCREATE_MULTITHREADED, //. ? locks at present
									  &P, &pDevice);*/

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	
	HRESULT R;
	D3D_FEATURE_LEVEL levels[] = {
		//D3D_FEATURE_LEVEL_11_0,
		//D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	D3D_FEATURE_LEVEL curr_level = D3D_FEATURE_LEVEL_10_0;
	u32 cnt = sizeof(levels) / sizeof(D3D_FEATURE_LEVEL);

	R = D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, createDeviceFlags, levels, cnt, D3D11_SDK_VERSION,
						  &pDevice11, &curr_level, &pContext);
   
	/* if (FAILED(R))
	{
		R = HW.pD3D->CreateDevice(DevAdapter, DevT, m_hWnd,
								  GPU | D3DCREATE_MULTITHREADED, //. ? locks at present
								  &P, &pDevice);
	}*/
	if (FAILED(R))
	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		Msg("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		FlushLog();
		MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!",
				   MB_OK | MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
	};
	R_CHK(R);

	_SHOW_REF("* CREATE: DeviceREF:", HW.pDevice11);
	switch (GPU)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
		Log("* Vertex Processor: SOFTWARE");
		break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:
		Log("* Vertex Processor: MIXED");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:
		Log("* Vertex Processor: HARDWARE");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE:
		Log("* Vertex Processor: PURE HARDWARE");
		break;
	}

	R_CHK(R);

	R = pFactory11->CreateSwapChain(pDevice11, &P, &m_pSwapChain);
	pFactory11->Release();

	// Capture misc data
#ifdef DEBUG
	R_CHK(pDevice->CreateStateBlock(D3DSBT_ALL, &dwDebugSB));
#endif
	//R_CHK(pDevice->GetRenderTarget(0, &pBaseRT));
	//R_CHK(pDevice->GetDepthStencilSurface(&pBaseZB));
	//u32 memory = pDevice->GetAvailableTextureMem();
	u32 memory = Desc11.DedicatedVideoMemory;
	Msg("* Texture memory: %d M", memory / (1024 * 1024));
	//Msg("* DirectX Graphics Infrastructure level: %2.1f", float(D3DXGetDriverLevel(pDevice)) / 100.f);
	Msg("* DirectX Graphics Infrastructure level: DirectX11");
#ifndef _EDITOR
	updateWindowProps(m_hWnd);
	fill_vid_mode_list(this);
#endif
}

u32 CHW::selectPresentInterval()
{
	/* D3DCAPS9 caps;
	pD3D->GetDeviceCaps(DevAdapter, DevT, &caps);

	if (!psDeviceFlags.test(rsVSync))
	{
		if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
			return D3DPRESENT_INTERVAL_IMMEDIATE;
		if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE)
			return D3DPRESENT_INTERVAL_ONE;
	}*/
	return D3DPRESENT_INTERVAL_DEFAULT;
}

u32 CHW::selectGPU()
{
	return D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;

	/*
	if (Caps.bForceGPU_SW)
		return D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DCAPS9 caps;
	pD3D->GetDeviceCaps(DevAdapter, DevT, &caps);

	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		if (Caps.bForceGPU_NonPure)
			return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
		{
			if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE)
				return D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
			else
				return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		// return D3DCREATE_MIXED_VERTEXPROCESSING;
	}
	else
		return D3DCREATE_SOFTWARE_VERTEXPROCESSING;*/
}

u32 CHW::selectRefresh(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt)
{
	return D3DPRESENT_RATE_DEFAULT;

	/* if (psDeviceFlags.is(rsRefresh60hz))
		return D3DPRESENT_RATE_DEFAULT;
	 else
	{
		u32 selected = D3DPRESENT_RATE_DEFAULT;
		u32 count = pD3D->GetAdapterModeCount(DevAdapter, fmt);
		for (u32 I = 0; I < count; I++)
		{
			D3DDISPLAYMODE Mode;
			pD3D->EnumAdapterModes(DevAdapter, fmt, I, &Mode);
			if (Mode.Width == dwWidth && Mode.Height == dwHeight)
			{
				if (Mode.RefreshRate > selected)
					selected = Mode.RefreshRate;
			}
		}
		return selected;
	}*/
}

BOOL CHW::support(D3DFORMAT fmt, DWORD type, DWORD usage)
{
	return TRUE;
	/* HRESULT hr = pD3D->CheckDeviceFormat(DevAdapter, DevT, Caps.fTarget, usage, (D3DRESOURCETYPE)type, fmt);
	if (FAILED(hr))
		return FALSE;
	else
		return TRUE;*/
}

void CHW::updateWindowProps(HWND m_hWnd)
{
#ifndef DEDICATED_SERVER
	//BOOL bWindowed = !psDeviceFlags.is(rsFullscreen);
#else
	//BOOL bWindowed = TRUE;
#endif
	BOOL bWindowed = TRUE;


	u32 dwWindowStyle = 0;
	// Set window properties depending on what mode were in.
	if (bWindowed || strstr(Core.Params, "-windowed"))
	{
		SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyle = (WS_POPUP));
		// When moving from fullscreen to windowed mode, it is important to
		// adjust the window size after recreating the device rather than
		// beforehand to ensure that you get the window size you want.  For
		// example, when switching from 640x480 fullscreen to windowed with
		// a 1000x600 window on a 1024x768 desktop, it is impossible to set
		// the window size to 1000x600 until after the display mode has
		// changed to 1024x768, because windows cannot be larger than the
		// desktop.

		RECT m_rcWindowBounds;
		RECT DesktopRect;

		GetClientRect(GetDesktopWindow(), &DesktopRect);

		/* SetRect(&m_rcWindowBounds, (DesktopRect.right - DevPP.BackBufferWidth) / 2,
				(DesktopRect.bottom - DevPP.BackBufferHeight) / 2, (DesktopRect.right + DevPP.BackBufferWidth) / 2,
				(DesktopRect.bottom + DevPP.BackBufferHeight) / 2);*/
		SetRect(&m_rcWindowBounds, (DesktopRect.right - m_ChainDesc.BufferDesc.Width) / 2,
				(DesktopRect.bottom - m_ChainDesc.BufferDesc.Height) / 2,
				(DesktopRect.right + m_ChainDesc.BufferDesc.Width) / 2,
				(DesktopRect.bottom + m_ChainDesc.BufferDesc.Height) / 2);

		AdjustWindowRect(&m_rcWindowBounds, dwWindowStyle, FALSE);

		SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
					 (m_rcWindowBounds.right - m_rcWindowBounds.left), (m_rcWindowBounds.bottom - m_rcWindowBounds.top),
					 SWP_SHOWWINDOW | SWP_NOCOPYBITS);
	}
	else
	{
		SetWindowLong(m_hWnd, GWL_STYLE, dwWindowStyle = (WS_POPUP | WS_VISIBLE));
	}

#ifndef DEDICATED_SERVER
	ShowCursor(FALSE);
	SetForegroundWindow(m_hWnd);
#endif
}

struct _uniq_mode
{
	_uniq_mode(LPCSTR v) : _val(v)
	{
	}
	LPCSTR _val;
	bool operator()(LPCSTR _other)
	{
		return !stricmp(_val, _other);
	}
};

#ifndef _EDITOR
void free_vid_mode_list()
{
	for (int i = 0; vid_mode_token[i].name; i++)
	{
		xr_free(vid_mode_token[i].name);
	}
	xr_free(vid_mode_token);
	vid_mode_token = NULL;
}

void fill_vid_mode_list(CHW* _hw)
{
	static xr_token _vid_modes[] = {
		{"1280x720", 0},
		{"1920x1080", 0},
		{0, 0},
	};

	vid_mode_token = _vid_modes;

	/* if (vid_mode_token != NULL)
		return;
	xr_vector<LPCSTR> _tmp;
	u32 cnt = _hw->pD3D->GetAdapterModeCount(_hw->DevAdapter, _hw->Caps.fTarget);

	u32 i;
	for (i = 0; i < cnt; ++i)
	{
		D3DDISPLAYMODE Mode;
		string32 str;

		_hw->pD3D->EnumAdapterModes(_hw->DevAdapter, _hw->Caps.fTarget, i, &Mode);
		//		if (Mode.Width < 426)		continue;

		sprintf_s(str, sizeof(str), "%dx%d", Mode.Width, Mode.Height);

		if (_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
			continue;

		_tmp.push_back(NULL);
		_tmp.back() = xr_strdup(str);
	}

	u32 _cnt = _tmp.size() + 1;

	vid_mode_token = xr_alloc<xr_token>(_cnt);

	vid_mode_token[_cnt - 1].id = -1;
	vid_mode_token[_cnt - 1].name = NULL;

#ifdef DEBUG
	Msg("Available video modes[%d]:", _tmp.size());
#endif // DEBUG
	for (i = 0; i < _tmp.size(); ++i)
	{
		vid_mode_token[i].id = i;
		vid_mode_token[i].name = _tmp[i];
#ifdef DEBUG
		Msg("[%s]", _tmp[i]);
#endif // DEBUG
	}*/
}
#endif
