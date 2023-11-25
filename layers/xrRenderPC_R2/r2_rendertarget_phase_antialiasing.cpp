///////////////////////////////////////////////////////////////////////////////////
// Created: 15.11.2023
// Author: Deathman
// Nocturning studio for NS Project X
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "r2_rendertarget.h"
///////////////////////////////////////////////////////////////////////////////////
#pragma todo(Deathman to all: То как сейчас делается многопроходное сглаживание это очень плохо, но я не знаю как сделать так, чтобы мы могли выбрать одно сглаживание и в одном методе его запихивать в цикл)
int CRenderTarget::select_antialiasing_iterations_count()
{
	switch (ps_r_aa_iterations)
	{
	case 1:
		return 1;
		break;
	case 2:
		return 2;
		break;
	case 3:
		return 4;
		break;
	case 4:
		return 8;
		break;
	}
}

void CRenderTarget::antialiasing_phase_dlaa()
{
	for (int i = 0; i < select_antialiasing_iterations_count(); i++)
	{
		u_setrt(rt_GBuffer_Albedo, NULL, NULL, NULL);

		RCache.set_CullMode(CULL_NONE);
		RCache.set_Stencil(FALSE);

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
		RCache.set_Element(s_antialiasing->E[0]);

		// Set geometry
		RCache.set_Geometry(g_combine);

		// Draw
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}
}

void CRenderTarget::antialiasing_phase_dlaa_with_edge_detecting()
{
	for (int i = 0; i < select_antialiasing_iterations_count(); i++)
	{
		u_setrt(rt_GBuffer_Albedo, NULL, NULL, NULL);

		RCache.set_CullMode(CULL_NONE);
		RCache.set_Stencil(FALSE);

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
		RCache.set_Element(s_antialiasing->E[1]);

		RCache.set_c("e_barrier", ps_r2_aa_barier.x, ps_r2_aa_barier.y, ps_r2_aa_barier.z, 0);
		RCache.set_c("e_weights", ps_r2_aa_weight.x, ps_r2_aa_weight.y, ps_r2_aa_weight.z, 0);
		RCache.set_c("e_kernel", ps_r2_aa_kernel, ps_r2_aa_kernel, ps_r2_aa_kernel, 0);

		// Set geometry
		RCache.set_Geometry(g_combine);

		// Draw
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}
}

void CRenderTarget::antialiasing_phase_fxaa()
{
	for (int i = 0; i < select_antialiasing_iterations_count(); i++)
	{
		u_setrt(rt_GBuffer_Albedo, NULL, NULL, NULL);

		RCache.set_CullMode(CULL_NONE);
		RCache.set_Stencil(FALSE);

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
		RCache.set_Element(s_antialiasing->E[2]);

		// Set geometry
		RCache.set_Geometry(g_combine);

		// Draw
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}
}

void CRenderTarget::select_antialiasing_type()
{
	switch (ps_r_aa)
	{
	case 1:
		return antialiasing_phase_dlaa();
		break;
	case 2:
		return antialiasing_phase_dlaa_with_edge_detecting();
		break;
	case 3:
		return antialiasing_phase_fxaa();
		break;
	}
}

void CRenderTarget::phase_antialiasing()
{
	select_antialiasing_type();
}
///////////////////////////////////////////////////////////////////////////////////
