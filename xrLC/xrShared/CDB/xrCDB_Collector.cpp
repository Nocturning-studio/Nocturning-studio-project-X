//#include "stdafx.h"
#pragma hdrstop

#include "Opcode.h"

#include "xrCDB.h"

namespace CDB
{

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

	CollectorPacked::CollectorPacked(const Fbox& bb, int apx_vertices, int apx_faces)
	{
		// Params
		VMscale.set(bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.max.z - bb.min.z);
		VMmin.set(bb.min);
		VMeps.set(VMscale.x / clpMX / 2, VMscale.y / clpMY / 2, VMscale.z / clpMZ / 2);
		VMeps.x = (VMeps.x < EPS_L) ? VMeps.x : EPS_L;
		VMeps.y = (VMeps.y < EPS_L) ? VMeps.y : EPS_L;
		VMeps.z = (VMeps.z < EPS_L) ? VMeps.z : EPS_L;

		// Preallocate memory
		verts.reserve(apx_vertices);
		faces.reserve(apx_faces);

		int	_size = (clpMX + 1) * (clpMY + 1) * (clpMZ + 1);
		int	_average = (apx_vertices / _size) / 2;

		for (int ix = 0; ix < clpMX + 1; ix++)
			for (int iy = 0; iy < clpMY + 1; iy++)
				for (int iz = 0; iz < clpMZ + 1; iz++)
					VM[ix][iy][iz].reserve(_average);
	}

	void CollectorPacked::add_face(
		const Fvector& v0, const Fvector& v1, const Fvector& v2,	// vertices
		u16 material, u16 sector									// misc
	)
	{
		TRI T;
		T.verts[0] = VPack(v0);
		T.verts[1] = VPack(v1);
		T.verts[2] = VPack(v2);
		T.material = material;
		T.sector = sector;
		faces.push_back(T);
	}

	void CollectorPacked::add_face_D(
		const Fvector& v0, const Fvector& v1, const Fvector& v2,	// vertices
#ifdef _WIN64
		u64 dummy													// misc
#else
		u32 dummy													// misc
#endif
	)
	{
		TRI T;
		T.verts[0] = VPack(v0);
		T.verts[1] = VPack(v1);
		T.verts[2] = VPack(v2);
		T.dummy = dummy;
		faces.push_back(T);
	}

	u32	CollectorPacked::VPack(const Fvector& V)
	{
		u32 P = 0xffffffff;

		u32 ix, iy, iz;
		ix = iFloor(float(V.x - VMmin.x) / VMscale.x * clpMX);
		iy = iFloor(float(V.y - VMmin.y) / VMscale.y * clpMY);
		iz = iFloor(float(V.z - VMmin.z) / VMscale.z * clpMZ);

		//		R_ASSERT(ix<=clpMX && iy<=clpMY && iz<=clpMZ);
		clamp(ix, (u32)0, clpMX);	clamp(iy, (u32)0, clpMY);	clamp(iz, (u32)0, clpMZ);

		{
			DWORDList* vl;
			vl = &(VM[ix][iy][iz]);
			for (DWORDIt it = vl->begin(); it != vl->end(); it++)
				if (verts[*it].similar(V)) 
				{
					P = *it;
					break;
				}
		}

		if (0xffffffff == P)
		{
			P = verts.size();
			verts.push_back(V);

			VM[ix][iy][iz].push_back(P);

			u32 ixE, iyE, izE;
			ixE = iFloor(float(V.x + VMeps.x - VMmin.x) / VMscale.x * clpMX);
			iyE = iFloor(float(V.y + VMeps.y - VMmin.y) / VMscale.y * clpMY);
			izE = iFloor(float(V.z + VMeps.z - VMmin.z) / VMscale.z * clpMZ);

			//			R_ASSERT(ixE<=clpMX && iyE<=clpMY && izE<=clpMZ);
			clamp(ixE, (u32)0, clpMX);	clamp(iyE, (u32)0, clpMY);	clamp(izE, (u32)0, clpMZ);

			if (ixE != ix)							VM[ixE][iy][iz].push_back(P);
			if (iyE != iy)							VM[ix][iyE][iz].push_back(P);
			if (izE != iz)							VM[ix][iy][izE].push_back(P);
			if ((ixE != ix) && (iyE != iy))				VM[ixE][iyE][iz].push_back(P);
			if ((ixE != ix) && (izE != iz))				VM[ixE][iy][izE].push_back(P);
			if ((iyE != iy) && (izE != iz))				VM[ix][iyE][izE].push_back(P);
			if ((ixE != ix) && (iyE != iy) && (izE != iz))	VM[ixE][iyE][izE].push_back(P);
		}
		return P;
	}

	void	CollectorPacked::clear()
	{
		verts.clear_and_free();
		faces.clear_and_free();
		for (u32 _x = 0; _x <= clpMX; _x++)
			for (u32 _y = 0; _y <= clpMY; _y++)
				for (u32 _z = 0; _z <= clpMZ; _z++)
					VM[_x][_y][_z].clear_and_free();
	}
};
