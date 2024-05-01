#include "stdafx.h"

void CRenderTarget::phase_occq()
{
	//u_setrt(Device.dwWidth, Device.dwHeight, HW.pBaseRT, NULL, NULL, rt_ZB->pRT);
	SetRT(Device.dwWidth, Device.dwHeight, HW.pBaseRT, 0, 0, 0, rt_ZB->pZRT, false, false);
	RCache.set_CullMode(CULL_CCW);
	RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
	RCache.set_ColorWriteEnable(FALSE);
	RCache.set_Shader(s_occq);
}
