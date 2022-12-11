#include "stdafx.h"
#include "build.h"
#include "common_compilers\xrThread.h"
#include "hash2D.h"
#include "DXT.h"

xrCriticalSection	implicit_CS;
xr_vector<int>		implicit_task_pool;

class ImplicitDeflector
{
public:
	b_BuildTexture*			texture;
	lm_layer				lmap;
	vecFace					faces;
	
	ImplicitDeflector() : texture(0)
	{
	}
	~ImplicitDeflector()
	{
		Deallocate	();
	}
	
	void			Allocate	()
	{
		lmap.create	(Width(),Height());
	}
	void			Deallocate	()
	{
		lmap.destroy();
	}
	
	u32			Width	()						{ return texture->dwWidth; }
	u32			Height	()						{ return texture->dwHeight; }
	
	u32&		Texel	(u32 x, u32 y)			{ return texture->pSurface[y*Width()+x]; }
	base_color& Lumel	(u32 x, u32 y)			{ return lmap.surface[y*Width()+x];	}
	u8&			Marker	(u32 x, u32 y)			{ return lmap.marker [y*Width()+x];	}
	
	void	Bounds	(u32 ID, Fbox2& dest)
	{
		Face* F		= faces[ID];
		_TCF& TC	= F->tc[0];
		dest.min.set	(TC.uv[0]);
		dest.max.set	(TC.uv[0]);
		dest.modify		(TC.uv[1]);
		dest.modify		(TC.uv[2]);
	}
	void	Bounds_Summary (Fbox2& bounds)
	{
		bounds.invalidate();
		for (u32 I=0; I<faces.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
};


DEF_MAP(Implicit,u32,ImplicitDeflector);

typedef hash2D <Face*,384,384>		IHASH;
static IHASH*						ImplicitHash;

class ImplicitThread : public CThread
{
public:
	ImplicitDeflector*	DATA;			// Data for this thread
	u32					total_implicit;

	ImplicitThread		(u32 ID, ImplicitDeflector* _DATA, u32 TOTALI ) : CThread (ID)
	{
		DATA			= _DATA;
		total_implicit	= TOTALI;
	}
	virtual void		Execute	()
	{
		// Priority
		SetThreadPriority		(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		if (b_highest_priority) SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		Sleep					(0);

		R_ASSERT				(DATA);
		ImplicitDeflector&		defl	= *DATA;
		CDB::COLLIDER			DB;
		
		// Setup variables
		Fvector2	dim,half;
		dim.set		(float(defl.Width()),float(defl.Height()));
		half.set	(.5f/dim.x,.5f/dim.y);
		
		// Jitter data
		Fvector2	JS;
		JS.set		(.499f/dim.x, .499f/dim.y);
		u32			Jcount;
		Fvector2*	Jitter;
		Jitter_Select(Jitter, Jcount);
		
		// Lighting itself
		DB.ray_options	(0);
		for (;;)
		{
			u32 iter;
			// Get task
			implicit_CS.Enter();
			
			thProgress = 1.f - float(implicit_task_pool.size()) / float(total_implicit);

			if (implicit_task_pool.empty())
			{
				implicit_CS.Leave();
				return;
			}

			iter = implicit_task_pool.back();
			implicit_task_pool.pop_back();
			implicit_CS.Leave();

			for (u32 U=0; U<defl.Width(); U++)
			{
				base_color_c	C;
				u32				Fcount	= 0;

				for (u32 J = 0; J<Jcount; J++)
				{
					// LUMEL space
					Fvector2				P;
					P.x = float(U) / dim.x + half.x + Jitter[J].x * JS.x;
					P.y = float(iter) / dim.y + half.y + Jitter[J].y * JS.y;
					xr_vector<Face*>& space = ImplicitHash->query(P.x, P.y);

					// World space
					Fvector wP, wN, B;
					for (vecFaceIt it = space.begin(); it != space.end(); ++it)
					{
						Face	*F = *it;
						_TCF&	tc = F->tc[0];
						if (tc.isInside(P, B))
						{
							// We found triangle and have barycentric coords
							Vertex	*V1 = F->v[0];
							Vertex	*V2 = F->v[1];
							Vertex	*V3 = F->v[2];
							wP.from_bary(V1->P, V2->P, V3->P, B);
							wN.from_bary(V1->N, V2->N, V3->N, B);
							wN.normalize();
							LightPoint(&DB, RCAST_Model, C, wP, wN, pBuild->L_static, (b_norgb ? LP_dont_rgb : 0) | (b_nosun ? LP_dont_sun : 0), F);
							Fcount++;
						}
					}
				}

				if (Fcount) 
				{
					// Calculate lighting amount
					C.scale				(Fcount);
					C.mul				(.5f);
					defl.Lumel(U,iter)._set(C);
					defl.Marker(U,iter)	= 255;
				} 
				else defl.Marker(U,iter)	= 0;				
			}
		}
	}
};

void CBuild::ImplicitLighting()
{
	if (g_params.m_quality==ebqDraft) return;

	Implicit		calculator;
	ImplicitHash	= xr_new<IHASH>	();
	
	// Sorting
	Status("Sorting faces...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); ++I)
	{
		Face* F = *I;
		if (F->pDeflector)				continue;
		if (!F->hasImplicitLighting())	continue;
		
		Progress(float(I - g_faces.begin()) / float(g_faces.size()));

		b_material&		M	= materials		[F->dwMaterial];
		u32				Tid = M.surfidx;
		b_BuildTexture*	T	= &(textures[Tid]);
		
		Implicit_it		it	= calculator.find(Tid);
		if (it==calculator.end()) 
		{
			ImplicitDeflector	ImpD;
			ImpD.texture		= T;
			ImpD.faces.push_back(F);
			calculator.insert	(mk_pair(Tid,ImpD));
		} 
		else 
		{
			ImplicitDeflector&	ImpD = it->second;
			ImpD.faces.push_back(F);
		}
	}
	
	// Lighing
	for (Implicit_it imp=calculator.begin(); imp!=calculator.end(); ++imp)
	{
		ImplicitDeflector& defl = imp->second;
		Status			("Lighting implicit map '%s'...",defl.texture->name);
		
		Progress(0);

		defl.Allocate	();
		
		// Setup cache
		Progress(0);
		Fbox2 bounds;
		defl.Bounds_Summary			(bounds);
		ImplicitHash->initialize	(bounds,defl.faces.size());
		for (u32 fid=0; fid<defl.faces.size(); fid++)
		{
			Face* F				= defl.faces[fid];
			F->AddChannel		(F->tc[0].uv[0],F->tc[0].uv[1],F->tc[0].uv[2]); // make compatible format with LMAPs
			defl.Bounds			(fid,bounds);
			ImplicitHash->add	(bounds,F);
		}

		// Start threads
		CTimer	implicit_time;
		implicit_time.Start();

		//u32 threads_count = 16;
		extern int			i_thread_count;
		u32 threads_count = i_thread_count;
		u32 count = defl.Height();

		for (u32 itr = 0; itr<count; itr++)	implicit_task_pool.push_back(itr);

		CThreadManager			tmanager;
		for (u32 thID=0; thID<threads_count; thID++)
			tmanager.start		(xr_new<ImplicitThread>(thID, &defl,count));
		tmanager.wait			(500);

		clMsg("%f seconds", implicit_time.GetElapsed_sec());

		// Expand
		Status	("Processing lightmap...");
		for (u32 ref=254; ref>0; ref--)	if (!ApplyBorders(defl.lmap,ref)) break;

		Status	("Mixing lighting with texture...");
		{
			b_BuildTexture& TEX		=	*defl.texture;
			VERIFY					(TEX.pSurface);
			u32*			color	= TEX.pSurface;
			for (u32 V=0; V<defl.Height(); V++)	{
				for (u32 U=0; U<defl.Width(); U++)	{
					// Retreive Texel
					float	h	= defl.Lumel(U,V).h._r();
					u32 &C		= color[V*defl.Width() + U];
					C			= subst_alpha(C,u8_clr(h));
				}
			}
		}

		// base
		Status	("Saving base...");
		{
			string_path				name, out_name;
			sscanf					(strstr(Core.Params,"-f")+2,"%s",name);
			R_ASSERT				(name[0] && defl.texture);
			b_BuildTexture& TEX		=	*defl.texture;
			strconcat				(sizeof(out_name),out_name,name,"\\",TEX.name,".dds");
			FS.update_path			(out_name,_game_levels_,out_name);
			clMsg					("Saving texture '%s'...",out_name);
			VerifyPath				(out_name);
			BYTE* raw_data			=	LPBYTE(TEX.pSurface);
			u32	w					=	TEX.dwWidth;
			u32	h					=	TEX.dwHeight;
	
			DXTCompress				(out_name,raw_data,w,h, b_lmap_rgba);
		}

		// lmap
		Status	("Saving lmap...");
		{
			xr_vector<u32>			packed;
			defl.lmap.Pack			(packed);

			string_path				name, out_name;
			sscanf					(strstr(GetCommandLine(),"-f")+2,"%s",name);
			b_BuildTexture& TEX		=	*defl.texture;
			strconcat				(sizeof(out_name),out_name,name,"\\",TEX.name,"_lm.dds");
			FS.update_path			(out_name,_game_levels_,out_name);
			clMsg					("Saving texture '%s'...",out_name);
			VerifyPath				(out_name);
			BYTE* raw_data			= LPBYTE(&*packed.begin());
			u32	w					= TEX.dwWidth;
			u32	h					= TEX.dwHeight;

			DXTCompress				(out_name,raw_data,w,h, b_lmap_rgba);
		}
		defl.Deallocate				();
	}

	xr_delete			(ImplicitHash);
	calculator.clear	();
}
