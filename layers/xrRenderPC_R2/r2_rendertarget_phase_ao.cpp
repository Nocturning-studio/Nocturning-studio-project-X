///////////////////////////////////////////////////////////////////////////////////
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "r2_rendertarget.h"
///////////////////////////////////////////////////////////////////////////////////
void CRenderTarget::phase_create_ao()
{
	// Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	if (ps_r2_ao_quality <= 2)
	{
		w *= 0.5f;
		h *= 0.5f;
	}
	else
	{
		w *= 0.75f;
		h *= 0.75f;
	}

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	// Set output RT
	u_setrt(rt_ao_base, nullptr, nullptr, HW.pBaseZB);

	CHK_DX(HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, C, 1.0f, 0L));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
	pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
	pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
	pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
	pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Set pass
	switch (ps_r2_ao)
	{
	case 1: // SSAO
		RCache.set_Element(s_ambient_occlusion->E[0]);
		break;
	case 2: // HDAO
		RCache.set_Element(s_ambient_occlusion->E[1]);
		break;
	case 3: // HBAO
		RCache.set_Element(s_ambient_occlusion->E[2]);
		break;
	}

	// Set geometry
	RCache.set_Geometry(g_combine);

	// Set constants
	RCache.set_c("ao_resolution", w, h, 1 / w, 1 / h);

	// Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_vertical_filter()
{
	// Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	if (ps_r2_ao_quality <= 2)
	{
		w *= 0.5f;
		h *= 0.5f;
	}
	else
	{
		w *= 0.75f;
		h *= 0.75f;
	}

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	// Set output RT
	u_setrt(rt_ao_blurred1, nullptr, nullptr, HW.pBaseZB);

	CHK_DX(HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, C, 1.0f, 0L));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
	pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
	pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
	pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
	pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Set pass
	RCache.set_Element(s_ambient_occlusion_blur->E[0]);

	// Set geometry
	RCache.set_Geometry(g_combine);

	// Set constants
	RCache.set_c("ao_resolution", w, h, 1 / w, 1 / h);

	// Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_horizontal_filter()
{
	// Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	if (ps_r2_ao_quality <= 2)
	{
		w *= 0.5f;
		h *= 0.5f;
	}
	else
	{
		w *= 0.75f;
		h *= 0.75f;
	}

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	// Set output RT
	u_setrt(rt_ao_blurred2, nullptr, nullptr, HW.pBaseZB);

	CHK_DX(HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, C, 1.0f, 0L));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
	pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
	pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
	pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
	pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Set pass
	RCache.set_Element(s_ambient_occlusion_blur->E[1]);

	// Set geometry
	RCache.set_Geometry(g_combine);

	// Set constants
	RCache.set_c("ao_resolution", w, h, 1 / w, 1 / h);

	// Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_vertical_filter_pass_2()
{
	// Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	if (ps_r2_ao_quality <= 2)
	{
		w *= 0.5f;
		h *= 0.5f;
	}
	else
	{
		w *= 0.75f;
		h *= 0.75f;
	}

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	// Set output RT
	u_setrt(rt_ao_blurred1, nullptr, nullptr, HW.pBaseZB);

	CHK_DX(HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, C, 1.0f, 0L));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
	pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
	pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
	pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
	pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Set pass
	RCache.set_Element(s_ambient_occlusion_blur->E[2]);

	// Set geometry
	RCache.set_Geometry(g_combine);

	// Set constants
	RCache.set_c("ao_resolution", w, h, 1 / w, 1 / h);

	// Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_horizontal_filter_pass_2()
{
	// Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	if (ps_r2_ao_quality <= 2)
	{
		w *= 0.5f;
		h *= 0.5f;
	}
	else
	{
		w *= 0.75f;
		h *= 0.75f;
	}

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	// Set output RT
	u_setrt(rt_ao_blurred2, nullptr, nullptr, HW.pBaseZB);

	CHK_DX(HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, C, 1.0f, 0L));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
	pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
	pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
	pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
	pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Set pass
	RCache.set_Element(s_ambient_occlusion_blur->E[3]);

	// Set geometry
	RCache.set_Geometry(g_combine);

	// Set constants
	RCache.set_c("ao_resolution", w, h, 1 / w, 1 / h);

	// Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_upscale()
{
	// Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	// Set output RT
	u_setrt(rt_ao, nullptr, nullptr, HW.pBaseZB);

	CHK_DX(HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, C, 1.0f, 0L));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));

	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	// Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y);
	pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
	pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y);
	pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y);
	pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	// Set pass
	RCache.set_Element(s_ambient_occlusion_blur->E[4]);

	// Set geometry
	RCache.set_Geometry(g_combine);

	// Set constants
	RCache.set_c("ao_resolution", w, h, 1 / w, 1 / h);

	// Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::phase_filtering()
{
	phase_vertical_filter();
	phase_horizontal_filter();

	for (int i = 0; i < 3; i++)
	{
		phase_vertical_filter_pass_2();
		phase_horizontal_filter_pass_2();
	}

	phase_upscale();
}

void CRenderTarget::phase_ao()
{
	phase_create_ao();
	phase_filtering();
}
///////////////////////////////////////////////////////////////////////////////////