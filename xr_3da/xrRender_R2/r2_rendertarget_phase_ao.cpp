#include "stdafx.h"

void CRenderTarget::phase_ao()
{
///////////////////////////////////////////////////////////////////////////////////////////////
//AO Build		(Here we build rt_ao)

	//Constants
	u32 Offset = 0;
	u32 C = color_rgba(0, 0, 0, 255);

	float w = float(Device.dwWidth);
	float h = float(Device.dwHeight);

	float d_Z = EPS_S;
	float d_W = 1.f;

	Fvector2 p0, p1;
	p0.set(0.5f / w, 0.5f / h);
	p1.set((w + 0.5f) / w, (h + 0.5f) / h);

	//Here we draw AO to new RT (rt_ao)

	//Set output RT
	u_setrt(rt_ao, nullptr, nullptr, nullptr);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//Fill vertex buffer
	FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	//Set pass
	RCache.set_Element(s_ao->E[0]);

	//Set geometry
	RCache.set_Geometry(g_combine);

	//Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

///////////////////////////////////////////////////////////////////////////////////////////////
//AO Filter	pt.1	(Here we sample rt_ao, and blur it (then output to rt_ao_blurred))
 
	//Set output RT
	u_setrt(rt_ao_blurred1, nullptr, nullptr, nullptr);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//Fill vertex buffer
	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	//Set pass
	RCache.set_Element(s_ao->E[1]);

	//Set geometry
	RCache.set_Geometry(g_combine);

	//Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

///////////////////////////////////////////////////////////////////////////////////////////////
//AO Filter pt.2		(Here we sample rt_ao_blurred, and blur it again)

	//Set output RT
	u_setrt(rt_ao_blurred2, nullptr, nullptr, nullptr);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//Fill vertex buffer
	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	//Set pass
	RCache.set_Element(s_ao->E[2]);

	//Set geometry
	RCache.set_Geometry(g_combine);

	//Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

///////////////////////////////////////////////////////////////////////////////////////////////
//Downsample s_position for optimization 

	//Set output RT
	u_setrt(rt_blurred_position, nullptr, nullptr, nullptr);
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	//Fill vertex buffer
	pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
	pv->set(0, h, d_Z, d_W, C, p0.x, p1.y); pv++;
	pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y); pv++;
	pv->set(w, h, d_Z, d_W, C, p1.x, p1.y); pv++;
	pv->set(w, 0, d_Z, d_W, C, p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_combine->vb_stride);

	//Set pass
	RCache.set_Element(s_ao->E[3]);

	//Set geometry
	RCache.set_Geometry(g_combine);

	//Draw
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}
