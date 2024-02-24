// xrCPU_Pipe.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#pragma hdrstop

#pragma comment(lib, "xrEngine")

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

extern xrSkin1W xrSkin1W_x86;
extern xrSkin1W xrSkin1W_3DNow;
// extern xrSkin1W		xrSkin1W_SSE;
extern xrSkin2W xrSkin2W_x86;
extern xrSkin2W xrSkin2W_SSE;
extern xrSkin2W xrSkin2W_3DNow;
// extern xrBoneLerp		xrBoneLerp_x86;
// extern xrBoneLerp		xrBoneLerp_3DNow;
extern xrM44_Mul xrM44_Mul_x86;
extern xrM44_Mul xrM44_Mul_3DNow;
extern xrM44_Mul xrM44_Mul_SSE;
extern xrTransfer xrTransfer_x86;
extern xrMemCopy_8b xrMemCopy_MMXSSE3DNow;
extern xrMemCopy_8b xrMemCopy_x86;
extern xrMemFill_32b xrMemFill32_MMX;

extern "C"
{
#ifdef ALL_TO_ONE
	void __cdecl xrBind_PSGP(xrDispatchTable* T, DWORD dwFeatures)
#else
	__declspec(dllexport) void __cdecl xrBind_PSGP(xrDispatchTable* T, DWORD dwFeatures)
#endif
	{
		// generic
		T->skin1W = xrSkin1W_x86;
		T->skin2W = xrSkin2W_x86;
		// T->blerp	= xrBoneLerp_x86;
		T->m44_mul = xrM44_Mul_x86;
		T->transfer = xrTransfer_x86;
		T->memCopy = xrMemCopy_x86;
		T->memFill = NULL;
		T->memFill32 = xrMemFill32_MMX;

		// SSE
		if (CPU::ID.hasFeature(CpuFeature::Sse))
		{
			T->memCopy = xrMemCopy_MMXSSE3DNow;
			//T->skin2W	= xrSkin2W_SSE;
		}

		// 3dnow!
		if (CPU::ID.hasFeature(CpuFeature::_3dNow))
		{
			// T->skin1W	= xrSkin1W_3DNow;
			//  T->blerp	= xrBoneLerp_3DNow;
			T->memCopy = xrMemCopy_MMXSSE3DNow;
			// T->skin2W	= xrSkin2W_3DNow;
		}
	}
};
