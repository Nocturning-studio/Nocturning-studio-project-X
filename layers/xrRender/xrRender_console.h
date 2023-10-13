///////////////////////////////////////////////////////////////////////////////////
#ifndef xrRender_consoleH
#define xrRender_consoleH
///////////////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------------------------------*/
// Render common values
/*-------------------------------------------------------------------------------*/
extern ENGINE_API	int			ps_r__Supersample;
extern ECORE_API	int			ps_r__LightSleepFrames;

extern ECORE_API	float		ps_r__Detail_l_ambient;
extern ECORE_API	float		ps_r__Detail_l_aniso;
extern ECORE_API	float		ps_r__Detail_density;

extern ECORE_API	float		ps_r__Tree_w_rot;
extern ECORE_API	float		ps_r__Tree_w_speed;
extern ECORE_API	float		ps_r__Tree_w_amp;
extern ECORE_API	float		ps_r__Tree_SBC;		// scale bias correct
extern ECORE_API	Fvector		ps_r__Tree_Wave;

extern ECORE_API	float		ps_r__WallmarkTTL		;
extern ECORE_API	float		ps_r__WallmarkSHIFT		;
extern ECORE_API	float		ps_r__WallmarkSHIFT_V	;

extern ECORE_API	float		ps_r__GLOD_ssa_start;
extern ECORE_API	float		ps_r__GLOD_ssa_end	;
extern ECORE_API	float		ps_r__LOD			;
extern ECORE_API	float		ps_r__ssaDISCARD	;
extern ECORE_API	float		ps_r__ssaDONTSORT	;
extern ECORE_API	float		ps_r__ssaHZBvsTEX	;
extern ECORE_API	int			ps_r__tf_Anisotropic;

//Postprocess effects
extern ECORE_API u32			ps_vignette_mode;

//Render common flags
extern ECORE_API Flags32		ps_render_flags;
enum
{
	RFLAG_LENS_FLARES = (1 << 0),
};

/*-------------------------------------------------------------------------------*/
// R1-specific values
/*-------------------------------------------------------------------------------*/

extern ECORE_API	float		ps_r1_ssaLOD_A;
extern ECORE_API	float		ps_r1_ssaLOD_B;
extern ECORE_API	float		ps_r1_tf_Mipbias;
extern ECORE_API	float		ps_r1_lmodel_lerp;
extern ECORE_API	float		ps_r1_dlights_clip;
extern ECORE_API	float		ps_r1_pps_u;
extern ECORE_API	float		ps_r1_pps_v;
extern ECORE_API	int			ps_r1_GlowsPerFrame;

// R1-specific flags
extern ECORE_API Flags32		ps_r1_flags;
enum
{
	R1FLAG_DLIGHTS				= (1<<0),
};

/*-------------------------------------------------------------------------------*/
// R2/R2a/R2.5-specific values
/*-------------------------------------------------------------------------------*/

//Render subtypes
extern ENGINE_API	BOOL		r2_advanced_pp; //R2.5

extern ECORE_API float			ps_r2_ssaLOD_A;
extern ECORE_API float			ps_r2_ssaLOD_B;
extern ECORE_API float			ps_r2_detalization_distance;

extern ECORE_API float			ps_r2_tf_Mipbias;
	
extern ECORE_API float			ps_r2_gmaterial;

extern ECORE_API float			ps_r2_autoexposure_middlegray;
extern ECORE_API float			ps_r2_autoexposure_adaptation;
extern ECORE_API float			ps_r2_autoexposure_low_lum;
extern ECORE_API float			ps_r2_autoexposure_amount;

extern ECORE_API float			ps_r2_ls_bloom_kernel_scale;	// gauss
extern ECORE_API float			ps_r2_ls_bloom_kernel_g;		// gauss
extern ECORE_API float			ps_r2_ls_bloom_kernel_b;		// bilinear
extern ECORE_API float			ps_r2_ls_bloom_threshold;	
extern ECORE_API float			ps_r2_ls_bloom_speed;

extern ECORE_API float			ps_r2_ls_dsm_kernel;		
extern ECORE_API float			ps_r2_ls_psm_kernel;		
extern ECORE_API float			ps_r2_ls_ssm_kernel;

extern ECORE_API u32			ps_r2_aa;
extern ECORE_API u32			ps_r2_aa_quality;
extern ECORE_API Fvector		ps_r2_aa_barier;			
extern ECORE_API Fvector		ps_r2_aa_weight;			
extern ECORE_API float			ps_r2_aa_kernel;

extern ECORE_API float			ps_r2_mblur;				// .5f

extern ECORE_API Fvector3		ps_r2_dof;					//	x - min (0), y - focus (1.4), z - max (100)
extern ECORE_API float			ps_r2_dof_sky;				//	distance to sky
extern ECORE_API float			ps_r2_dof_kernel_size;		//	7.0f
extern ECORE_API u32			ps_r2_dof_quality;

extern ECORE_API int			ps_r2_GI_depth;				// 1..5
extern ECORE_API int			ps_r2_GI_photons;			// 8..256
extern ECORE_API float			ps_r2_GI_clip;				// EPS
extern ECORE_API float			ps_r2_GI_refl;				// .9f

extern ECORE_API float			ps_r2_ls_depth_scale;		// 1.0f
extern ECORE_API float			ps_r2_ls_depth_bias;		// -0.0001f
extern ECORE_API float			ps_r2_ls_squality;			// 1.0f

extern ECORE_API float			ps_r2_sun_near;				// 10.0f
extern ECORE_API float			ps_r2_sun_far;
extern ECORE_API u32			ps_r2_sun_quality;
extern ECORE_API float			ps_r2_sun_near_border;		// 1.0f
extern ECORE_API float			ps_r2_sun_tsm_projection;	// 0.2f
extern ECORE_API float			ps_r2_sun_tsm_bias;			// 0.0001f
extern ECORE_API float			ps_r2_sun_depth_far_scale;	// 1.00001f
extern ECORE_API float			ps_r2_sun_depth_far_bias;	// -0.0001f
extern ECORE_API float			ps_r2_sun_depth_near_scale;	// 1.00001f
extern ECORE_API float			ps_r2_sun_depth_near_bias;	// -0.0001f
extern ECORE_API float			ps_r2_sun_lumscale;			// 0.5f
extern ECORE_API float			ps_r2_sun_lumscale_hemi;	// 1.0f
extern ECORE_API float			ps_r2_sun_lumscale_amb;		// 1.0f
extern ECORE_API u32			ps_r2_sun_shafts;			//=	0;
extern ECORE_API xr_token		qsun_shafts_token[];
extern ECORE_API u32			ps_r2_shadow_filtering;

extern ECORE_API float			ps_r2_zfill;				// .1f

extern ECORE_API float			ps_r2_dhemi_scale;			// 1.5f
extern ECORE_API int			ps_r2_dhemi_count;			// 5

extern ECORE_API float			ps_r2_slight_fade;			// 1.f

extern ECORE_API int			ps_r2_wait_sleep;

extern ECORE_API u32			ps_r2_ao;
extern ECORE_API u32			ps_r2_ao_quality;

extern ECORE_API u32			ps_r2_debug_frame_layers;

extern ECORE_API float			ps_r2_df_parallax_h;
extern ECORE_API u32			ps_r2_bump_mode;
extern ECORE_API u32			ps_r2_tdetail_bump_mode;
extern ECORE_API u32			ps_r2_terrain_bump_mode;

extern ECORE_API u32			ps_r2_fog_quality;

// R2/R2a/R2.5-specific flags
extern ECORE_API Flags32		ps_r2_lighting_flags;
enum
{
	R2FLAG_SUN = (1 << 0),
	R2FLAG_SUN_FOCUS = (1 << 1),
	R2FLAG_SUN_TSM = (1 << 2),
	R2FLAG_SUN_DETAILS = (1 << 3),
	R2FLAG_SUN_IGNORE_PORTALS = (1 << 4),
	R2FLAGEXT_SUN_ZCULLING = (1 << 5),
	R2FLAG_GI = (1 << 6),
	R2FLAG_R1LIGHTS = (1 << 7),
	R2FLAG_EXP_DONT_TEST_UNSHADOWED = (1 << 8)
};

extern ECORE_API Flags32		ps_r2_postprocess_flags;
enum
{
	R2FLAG_AUTOEXPOSURE = (1 << 0),
	R2FLAG_FASTBLOOM = (1 << 1),
	R2FLAG_SOFT_WATER = (1 << 2),
	R2FLAG_SOFT_PARTICLES = (1 << 3),
	R2FLAG_CHROMATIC_ABBERATION = (1 << 4),
	R2FLAG_BLOOM = (1 << 5),
	R2FLAG_DOF = (1 << 6),
	R2FLAG_MBLUR = (1 << 7),
	R2FLAG_SEPIA = (1 << 8),
	R2FLAG_HDR = (1 << 9)
};

extern ECORE_API Flags32		ps_r2_overlay_flags;
enum
{
	R2FLAG_PHOTO_GRID = (1 << 0),
	R2FLAG_CINEMA_BORDERS = (1 << 1),
	R2FLAG_WATERMARK = (1 << 2)
};		

extern ECORE_API Flags32		ps_r2_ls_flags;
enum
{
	R2FLAG_GLOBALMATERIAL = (1<<0),
	R2FLAG_ZFILL = (1<<1),
	R2FLAG_EXP_SPLIT_SCENE = (1<<2),
	R2FLAG_USE_NVDBT = (1<<3),
	R2FLAG_USE_NVSTENCIL = (1<<4),
	R2FLAG_EXP_MT_CALC = (1<<5),
	R2FLAG_HARD_OPTIMIZATION = (1<<6)
};

/*-------------------------------------------------------------------------------*/
// Functions
/*-------------------------------------------------------------------------------*/
extern void						xrRender_initconsole	();
extern BOOL						xrRender_test_hw		();
extern void						xrRender_apply_tf		();
///////////////////////////////////////////////////////////////////////////////////
#endif
///////////////////////////////////////////////////////////////////////////////////