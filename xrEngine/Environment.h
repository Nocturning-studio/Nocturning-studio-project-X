#ifndef EnvironmentH
#define EnvironmentH

// refs
class ENGINE_API	IRender_Visual;
class ENGINE_API	CInifile;
class ENGINE_API 	CEnvironment;

// refs - effects
class ENGINE_API	CEnvironment;
class ENGINE_API	CLensFlare;
class ENGINE_API	CEffect_Rain;
class ENGINE_API	CEffect_Thunderbolt;

class ENGINE_API	CPerlinNoise1D;

struct SThunderboltDesc;
struct SThunderboltCollection;
class CLensFlareDescriptor;

#define DAY_LENGTH		86400.f

#include "blenders\blender.h"
class CBlender_skybox : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "INTERNAL: combiner"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C)
	{
		C.r_Pass("sky2", "sky2", FALSE, TRUE, FALSE);
		C.r_Sampler_clf("s_sky0", "$null");
		C.r_Sampler_clf("s_sky1", "$null");
		C.r_Sampler_rtf("s_tonemap", "$user$tonemap");	//. hack
		C.r_End();
	}
};

// t-defs
class ENGINE_API	CEnvModifier
{
public:
	Fvector3			position;
	float				radius;
	float				power;

	float				far_plane;
	Fvector3			fog_color;
	float				fog_density;
	Fvector3			ambient;
	Fvector3			sky_color;
	Fvector3			hemi_color;
	Flags16				use_flags;

	bool				load(IReader* fs);
	float				sum(CEnvModifier& _another, Fvector3& view);
};

class ENGINE_API	CEnvAmbient {
public:
	struct SEffect {
		u32 			life_time;
		ref_sound		sound;
		shared_str		particles;
		Fvector			offset;
		float			wind_gust_factor;
		float			wind_blast_in_time;
		float			wind_blast_out_time;
		float			wind_blast_strength;
		Fvector			wind_blast_direction;

		~SEffect() {}
	};
	DEFINE_VECTOR(SEffect*, EffectVec, EffectVecIt);
	struct SSndChannel
	{
		shared_str				m_load_section;
		Fvector2				m_sound_dist;
		Ivector4				m_sound_period;

		typedef xr_vector<ref_sound>	sounds_type;

		void					load(CInifile& config, LPCSTR sect);
		ref_sound& get_rnd_sound() { return sounds()[Random.randI(sounds().size())]; }
		u32						get_rnd_sound_time() { return (m_sound_period.z < m_sound_period.w) ? Random.randI(m_sound_period.z, m_sound_period.w) : 0; }
		u32						get_rnd_sound_first_time() { return (m_sound_period.x < m_sound_period.y) ? Random.randI(m_sound_period.x, m_sound_period.y) : 0; }
		float					get_rnd_sound_dist() { return (m_sound_dist.x < m_sound_dist.y) ? Random.randF(m_sound_dist.x, m_sound_dist.y) : 0; }
		~SSndChannel() {}
		IC sounds_type& sounds() { return m_sounds; }

	protected:
		xr_vector<ref_sound>	m_sounds;
	};
	DEFINE_VECTOR(SSndChannel*, SSndChannelVec, SSndChannelVecIt);

protected:
	shared_str				m_load_section;

	EffectVec				m_effects;
	Ivector2				m_effect_period;

	SSndChannelVec			m_sound_channels;
	shared_str			  m_ambients_config_filename;
public:
	IC const shared_str& name() { return m_load_section; }
	IC const shared_str& get_ambients_config_filename() { return m_ambients_config_filename; }

	void			load(
		CInifile& ambients_config,
		CInifile& sound_channels_config,
		CInifile& effects_config,
		const shared_str& section
	);
	IC SEffect* get_rnd_effect() { return effects().empty() ? 0 : effects()[Random.randI(effects().size())]; }
	IC u32					get_rnd_effect_time() { return Random.randI(m_effect_period.x, m_effect_period.y); }

	SEffect* create_effect(CInifile& config, LPCSTR id);
	SSndChannel* create_sound_channel(CInifile& config, LPCSTR id);
	~CEnvAmbient();
	void			destroy();
	IC EffectVec& effects() { return m_effects; }
	IC SSndChannelVec& get_snd_channels() { return m_sound_channels; }
};

class ENGINE_API	CEnvDescriptor
{
public:
	float				exec_time;
	float				exec_time_loaded;

	shared_str			sky_texture_name;
	shared_str			sky_texture_env_name;
	shared_str			clouds_texture_name;

	ref_texture			sky_texture;
	ref_texture			sky_texture_env;
	ref_texture			clouds_texture;

	Fvector4			clouds_color;
	Fvector3			sky_color;
	float				sky_rotation;

	float				far_plane;

	Fvector3			fog_color;
	float				fog_density;
	float				fog_distance;

	float				rain_density;
	Fvector3			rain_color;

	float				bolt_period;
	float				bolt_duration;

	float				wind_velocity;
	float				wind_direction;

	Fvector3			ambient;
	Fvector4			hemi_color;	// w = R2 correction
	Fvector3			sun_color;
	Fvector3			sun_dir;
	float				m_fSunShaftsIntensity;
	float				m_fWaterIntensity;

	shared_str			lens_flare_id;
	shared_str			tb_id;

	CEnvAmbient* env_ambient;

	CEnvDescriptor(shared_str const& identifier);

	void				load(CEnvironment& environment, CInifile& config);
	void				copy(const CEnvDescriptor& src)
	{
		float tm0 = exec_time;
		float tm1 = exec_time_loaded;
		*this = src;
		exec_time = tm0;
		exec_time_loaded = tm1;
	}

	void				on_device_create();
	void				on_device_destroy();

	shared_str			m_identifier;
};

class ENGINE_API		CEnvDescriptorMixer : public CEnvDescriptor {
public:
	STextureList		sky_r_textures;
	STextureList		sky_r_textures_env;
	STextureList		clouds_r_textures;
	float				weight;

	float				fog_near;
	float				fog_far;
public:
	CEnvDescriptorMixer(shared_str const& identifier);
	void				lerp(CEnvironment* parent, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& M, float m_power);
	void				clear();
	void				destroy();
};

class ENGINE_API	CEnvironment
{
	struct str_pred : public std::binary_function<shared_str, shared_str, bool> {
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{
			return xr_strcmp(x, y) < 0;
		}
	};
public:
	DEFINE_VECTOR(CEnvAmbient*, EnvAmbVec, EnvAmbVecIt);
	DEFINE_VECTOR(CEnvDescriptor*, EnvVec, EnvIt);
	DEFINE_MAP_PRED(shared_str, EnvVec, EnvsMap, EnvsMapIt, str_pred);
private:
	// clouds
	FvectorVec				CloudsVerts;
	U16Vec					CloudsIndices;
private:
	float					NormalizeTime(float tm);
	float					TimeDiff(float prev, float cur);
	float					TimeWeight(float val, float min_t, float max_t);
	void					SelectEnvs(EnvVec* envs, CEnvDescriptor*& e0, CEnvDescriptor*& e1, float tm);
	void					SelectEnv(EnvVec* envs, CEnvDescriptor*& e, float tm);
public:
	static bool sort_env_pred(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{
		return x->exec_time < y->exec_time;
	}
	static bool sort_env_etl_pred(const CEnvDescriptor* x, const CEnvDescriptor* y)
	{
		return x->exec_time_loaded < y->exec_time_loaded;
	}
protected:
	CBlender_skybox			m_b_skybox;
	CPerlinNoise1D* PerlinNoise1D;

	float					fGameTime;
public:
	BOOL					bNeed_re_create_env;

	float					wind_strength_factor;
	float					wind_gust_factor;

	// wind blast params
	float					wind_blast_strength;
	Fvector					wind_blast_direction;
	Fquaternion				wind_blast_start_time;
	Fquaternion				wind_blast_stop_time;
	float					wind_blast_strength_start_value;
	float					wind_blast_strength_stop_value;
	Fquaternion				wind_blast_current;
	// Environments
	CEnvDescriptorMixer* CurrentEnv;
	CEnvDescriptor* Current[2];

	bool					bWFX;
	float					wfx_time;
	CEnvDescriptor* WFX_end_desc[2];

	EnvVec* CurrentWeather;
	shared_str				CurrentWeatherName;
	shared_str				CurrentCycleName;

	EnvsMap					WeatherCycles;
	EnvsMap					WeatherFXs;
	xr_vector<CEnvModifier>	Modifiers;
	EnvAmbVec				Ambients;

	ref_shader				sh_2sky;
	ref_geom				sh_2geom;

	ref_shader				clouds_sh;
	ref_geom				clouds_geom;

	CEffect_Rain* eff_Rain;
	CLensFlare* eff_LensFlare;
	CEffect_Thunderbolt* eff_Thunderbolt;

	float					fTimeFactor;
	ref_texture				tonemap;
	ref_texture				tsky0, tsky1;

	void					SelectEnvs(float gt);

	void					UpdateAmbient();
	CEnvAmbient* AppendEnvAmb(const shared_str& sect);

	void					Invalidate();
public:
	CEnvironment();
	~CEnvironment();

	void					load();
	void					unload();

	void					mods_load();
	void					mods_unload();

	void					OnFrame();
	void					calculate_dynamic_sun_dir();
	void					lerp(float& current_weight);

	void					calculate_dynamic_sun_dir();

	void					RenderSky();
	void					RenderClouds();
	void					RenderFlares();
	void					RenderLast();

	bool					SetWeatherFX(shared_str name);
	bool					StartWeatherFXFromTime(shared_str name, float time);
	bool					IsWFXPlaying() { return bWFX; }
	void					StopWFX();
	void					SetWeather(shared_str name, bool forced = false);
	shared_str				GetWeather() { return CurrentWeatherName; }

	//Kondr48: функция перемотки времени
	void					ChangeGameTime(float game_time);

	void					SetGameTime(float game_time, float time_factor);
	void					OnDeviceCreate();
	void					OnDeviceDestroy();

	// editor-related
#ifdef _EDITOR
public:
	float					ed_from_time;
	float					ed_to_time;
public:
	void					ED_Reload();
	float					GetGameTime() { return fGameTime; }
#endif
	CInifile* m_ambients_config;
	CInifile* m_sound_channels_config;
	CInifile* m_effects_config;
	CInifile* m_suns_config;
	CInifile* m_thunderbolt_collections_config;
	CInifile* m_thunderbolts_config;

protected:
	CEnvDescriptor* create_descriptor(shared_str const& identifier, CInifile* config);
	void					load_weathers();
	void					load_weather_effects();
	void					create_mixer();
	void					destroy_mixer();

	void					load_level_specific_ambients();

public:
	SThunderboltDesc* thunderbolt_description(CInifile& config, shared_str const& section);
	SThunderboltCollection* thunderbolt_collection(CInifile* pIni, CInifile* thunderbolts, LPCSTR section);
	SThunderboltCollection* thunderbolt_collection(xr_vector<SThunderboltCollection*>& collection, shared_str const& id);
	CLensFlareDescriptor* add_flare(xr_vector<CLensFlareDescriptor*>& collection, shared_str const& id);

public:
	float					p_var_alt;
	float					p_var_long;
	float					p_min_dist;
	float					p_tilt;
	float					p_second_prop;
	float					p_sky_color;
	float					p_sun_color;
	float					p_fog_color;
};

ENGINE_API extern Flags32	psEnvFlags;
ENGINE_API extern float		psVisDistance;

#endif //EnvironmentH
