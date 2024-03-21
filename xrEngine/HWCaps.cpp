#include "stdafx.h"
#pragma hdrstop

#include "hwcaps.h"
#include "hw.h"

#ifndef _EDITOR
#include "NVAPI/nvapi.h"
#include "ATI/atimgpud.h"

#pragma comment(lib, "nvapi")
#pragma comment(lib, "atimgpud_mtdll_x86")
#endif

namespace
{
#ifndef _EDITOR
u32 GetNVGpuNum()
{
	NvLogicalGpuHandle logicalGPUs[NVAPI_MAX_LOGICAL_GPUS];
	NvU32 logicalGPUCount;
	NvPhysicalGpuHandle physicalGPUs[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32 physicalGPUCount;

	//	int result = NVAPI_OK;

	int iGpuNum = 0;

	NvAPI_Status status;
	status = NvAPI_Initialize();

	if (status != NVAPI_OK)
	{
		Msg("* NVAPI is missing.");
		return iGpuNum;
	}

	// enumerate logical gpus
	status = NvAPI_EnumLogicalGPUs(logicalGPUs, &logicalGPUCount);
	if (status != NVAPI_OK)
	{
		return iGpuNum;
	}
	
	// enumerate physical gpus
	status = NvAPI_EnumPhysicalGPUs(physicalGPUs, &physicalGPUCount);
	if (status != NVAPI_OK)
	{
		return iGpuNum;
	}

	Msg("* NVidia MGPU: Logical(%d), Physical(%d)", logicalGPUCount, physicalGPUCount);

	//	Assume that we are running on logical GPU with most physical GPUs connected.
	for (u32 i = 0; i < logicalGPUCount; ++i)
	{
		status = NvAPI_GetPhysicalGPUsFromLogicalGPU(logicalGPUs[i], physicalGPUs, &physicalGPUCount);
		if (status == NVAPI_OK)
			iGpuNum = _max(iGpuNum, physicalGPUCount);
	}

	if (iGpuNum > 1)
	{
		Msg("* NVidia MGPU: %d-Way SLI detected.", iGpuNum);
	}

	return iGpuNum;
}

u32 GetATIGpuNum()
{
	int iGpuNum = AtiMultiGPUAdapters();

	if (iGpuNum > 1)
	{
		Msg("* ATI MGPU: %d-Way CrossFire detected.", iGpuNum);
	}

	return iGpuNum;
}

u32 GetGpuNum()
{
	u32 GPUNum = GetNVGpuNum();

	GPUNum = _max(GPUNum, GetATIGpuNum());

	GPUNum = _min(GPUNum, CHWCaps::MAX_GPUS);

	//	It's vital to have at least one GPU, else
	//	code will fail.
	VERIFY(GPUNum > 0);

	Msg("* Starting rendering as %d-GPU", GPUNum);

	return GPUNum;
}
#else
u32 GetGpuNum()
{
	return 1;
}
#endif
} // namespace

void CHWCaps::Update()
{
	/* D3DCAPS9 caps;
#pragma message(Reminder("Not implemented!"))
	// HW.pDevice->GetDeviceCaps(&caps);
	
	// ***************** GEOMETRY
	geometry_major = u16((u32(caps.VertexShaderVersion) & (0xf << 8ul)) >> 8);
	geometry_minor = u16((u32(caps.VertexShaderVersion) & 0xf));
	geometry.bSoftware = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0;
	geometry.bPointSprites = FALSE;
	geometry.bNPatches = (caps.DevCaps & D3DDEVCAPS_NPATCHES) != 0;
	DWORD cnt = (caps.MaxVertexShaderConst);
	clamp<DWORD>(cnt, 0, 256);
	geometry.dwRegisters = cnt;
	geometry.dwInstructions = 256;
	geometry.dwClipPlanes = _min(caps.MaxUserClipPlanes, 15);
	geometry.bVTF = (geometry_major >= 3) && HW.support(D3DFMT_R32F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_VERTEXTEXTURE);

	// ***************** PIXEL processing
	raster_major = u16(u32(u32(caps.PixelShaderVersion) & u32(0xf << 8ul)) >> 8);
	raster_minor = u16(u32(u32(caps.PixelShaderVersion) & u32(0xf)));
	raster.dwStages = caps.MaxSimultaneousTextures;
	raster.bNonPow2 = ((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0) ||
					  ((caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0);
	raster.bCubemap = (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) != 0;
	raster.dwMRT_count = (caps.NumSimultaneousRTs);
	raster.b_MRT_mixdepth = (caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) != 0;
	raster.dwInstructions = (caps.PS20Caps.NumInstructionSlots);

	// ***************** Info
	Msg("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)", caps.VertexShaderVersion, geometry_major, geometry_minor,
		CAP_VERSION(geometry_major, geometry_minor), caps.PixelShaderVersion, raster_major, raster_minor,
		CAP_VERSION(raster_major, raster_minor));

	// *******1********** Vertex cache
	IDirect3DQuery9* q_vc;
	D3DDEVINFO_VCACHE vc;
#pragma message(Reminder("Not implemented!"))
	 HRESULT _hr = HW.pDevice->CreateQuery(D3DQUERYTYPE_VCACHE, &q_vc);
	if (FAILED(_hr))
	{
		vc.OptMethod = 0;
		vc.CacheSize = 16;
		geometry.dwVertexCache = 16;
	}
	 else
	{
		q_vc->Issue(D3DISSUE_END);
		q_vc->GetData(&vc, sizeof(vc), D3DGETDATA_FLUSH);
		_RELEASE(q_vc);
		if (1 == vc.OptMethod)
			geometry.dwVertexCache = vc.CacheSize;
		else
			geometry.dwVertexCache = 16;
	}
	Msg("* GPU vertex cache: %s, %d", (1 == vc.OptMethod) ? "recognized" : "unrecognized", u32(geometry.dwVertexCache));

	// *******1********** Compatibility : vertex shader
	if (0 == raster_major)
		geometry_major = 0; // Disable VS if no PS
#ifdef _EDITOR
	geometry_major = 0;
#endif

	//
	bTableFog = FALSE; // BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

	// Detect if stencil available
	bStencil = FALSE;
	IDirect3DSurface9* surfZS = 0;
	D3DSURFACE_DESC surfDESC;
#pragma message(Reminder("Not implemented!"))
	// CHK_DX(HW.pDevice->GetDepthStencilSurface(&surfZS));
	R_ASSERT(surfZS);
	CHK_DX(surfZS->GetDesc(&surfDESC));
	_RELEASE(surfZS);

	switch (surfDESC.Format)
	{
	case D3DFMT_D15S1:
		bStencil = TRUE;
		break;
	case D3DFMT_D24S8:
		bStencil = TRUE;
		break;
	case D3DFMT_D24X4S4:
		bStencil = TRUE;
		break;
	}

	// Scissoring
	if (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)
		bScissor = TRUE;
	else
		bScissor = FALSE;

	// Stencil relative caps
	u32 dwStencilCaps = caps.StencilCaps;
	if ((!(dwStencilCaps & D3DSTENCILCAPS_INCR) && !(dwStencilCaps & D3DSTENCILCAPS_INCRSAT)) ||
		(!(dwStencilCaps & D3DSTENCILCAPS_DECR) && !(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)))
	{
		soDec = soInc = D3DSTENCILOP_KEEP;
		dwMaxStencilValue = 0;
	}
	else
	{
		// Prefer sat ops that cap at 0/max, but can use other ones as long as enough stencil bits
		soInc = (dwStencilCaps & D3DSTENCILCAPS_INCRSAT) ? D3DSTENCILOP_INCRSAT : D3DSTENCILOP_INCR;
		soDec = (dwStencilCaps & D3DSTENCILCAPS_DECRSAT) ? D3DSTENCILOP_DECRSAT : D3DSTENCILOP_DECR;
		dwMaxStencilValue = (1 << 8) - 1;
	}

#pragma message(Reminder("Not implemented!"))
	//HW.pD3D->CheckDeviceMultiSampleType(HW.DevAdapter, HW.DevT, HW.Caps.fTarget, FALSE, D3DMULTISAMPLE_NONMASKABLE,	&max_coverage);
	//max_coverage = max_coverage - 1; // get real max coverage

	// DEV INFO
	iGPUNum = GetGpuNum();*/

	struct sm
	{
		u16 major, minor;
	} version = {2, 0};

	switch (HW.FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_10_0:
		version.major = 4;
		version.minor = 0;
		break;
	case D3D_FEATURE_LEVEL_10_1:
		version.major = 4;
		version.minor = 1;
		break;
	case D3D_FEATURE_LEVEL_11_0:
		version.major = 5;
		version.minor = 0;
		break;
	}

	// ***************** GEOMETRY
	geometry_major = version.major;
	geometry_minor = version.minor;
	geometry.bSoftware = FALSE;
	geometry.bPointSprites = FALSE;
	geometry.bNPatches = version.major >= 5;
	geometry.dwRegisters = D3D10_REQ_CONSTANT_BUFFER_ELEMENT_COUNT;
	geometry.dwInstructions = 256;
	geometry.dwClipPlanes = 6;
	geometry.bVTF = TRUE;

	// ***************** PIXEL processing
	raster_major = version.major;
	raster_minor = version.minor;
	raster.dwStages = 16;
	raster.bNonPow2 = TRUE;
	raster.bCubemap = TRUE;
	raster.dwMRT_count = 8;
	raster.b_MRT_mixdepth = TRUE;
	raster.dwInstructions = D3D10_REQ_CONSTANT_BUFFER_ELEMENT_COUNT;

	// ***************** Info
	Msg("* GPU shading: vs(%x/%d.%d/%d), ps(%x/%d.%d/%d)", 0, geometry_major, geometry_minor,
		CAP_VERSION(geometry_major, geometry_minor), 0, raster_major, raster_minor,
		CAP_VERSION(raster_major, raster_minor));

	// *******1********** Vertex cache
	//	TODO: DX10: Find a way to detect cache size
	geometry.dwVertexCache = 128;
	Msg("* GPU vertex cache: %s, %d", "unrecognized", u32(geometry.dwVertexCache));

	// *******1********** Compatibility : vertex shader
	if (0 == raster_major)
		geometry_major = 0; // Disable VS if no PS

	//
	bTableFog = FALSE; // BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

	// Detect if stencil available
	bStencil = TRUE;

	// Scissoring
	bScissor = TRUE;

	// Stencil relative caps
	soInc = D3DSTENCILOP_INCRSAT;
	soDec = D3DSTENCILOP_DECRSAT;
	dwMaxStencilValue = (1 << 8) - 1;

	fTarget = (D3DFORMAT)DXGI_FORMAT_R8G8B8A8_UNORM;
	fDepth = (D3DFORMAT)DXGI_FORMAT_D24_UNORM_S8_UINT;

	// csaa caps
	/* ZeroMemory(msaa_coverage, sizeof(msaa_coverage));
	for (u32 i = 1, j = 0; i <= 8; i = i * 2, ++j)
	{
		HW.pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, i, &msaa_coverage[j]);
		Msg("MSAA %ux coverage: %u", i, msaa_coverage[j]);
	}*/

	// DEV INFO
	iGPUNum = GetGpuNum();
}

