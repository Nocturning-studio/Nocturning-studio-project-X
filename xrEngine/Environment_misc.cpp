#include "stdafx.h"
#pragma hdrstop

#include "Environment.h"
#include "xr_efflensflare.h"
#include "thunderbolt.h"
#include "rain.h"
#include "IGame_Level.h"
#include "../xrGame/object_broker.h"
#include "../xrGame/LevelGameDef.h"
#include "resourcemanager.h"

//-----------------------------------------------------------------------------
// Environment modifier
//-----------------------------------------------------------------------------
bool CEnvModifier::load(IReader* fs)
{
	//	Fvector			dummy;
	fs->r_fvector3(position);
	radius = fs->r_float();
	power = fs->r_float();
	far_plane = fs->r_float();
	fs->r_fvector3(fog_color);
	fog_density = fs->r_float();
	fs->r_fvector3(ambient);
	fs->r_fvector3(sky_color);
	fs->r_fvector3(hemi_color);

	return true;
}

float CEnvModifier::sum(CEnvModifier& M, Fvector3& view)
{
	float _dist_sq = view.distance_to_sqr(M.position);
	if (_dist_sq >= (M.radius * M.radius))
		return 0;

	float _att = 1 - _sqrt(_dist_sq) / M.radius; //[0..1];
	float _power = M.power * _att;

	if (M.use_flags.test(eViewDist))
	{
		far_plane += M.far_plane * _power;
		use_flags.set(eViewDist, TRUE);
	}
	if (M.use_flags.test(eFogColor))
	{
		fog_color.mad(M.fog_color, _power);
		use_flags.set(eFogColor, TRUE);
	}
	if (M.use_flags.test(eFogDensity))
	{
		fog_density += M.fog_density * _power;
		use_flags.set(eFogDensity, TRUE);
	}
	if (M.use_flags.test(eAmbientColor))
	{
		ambient.mad(M.ambient, _power);
		use_flags.set(eAmbientColor, TRUE);
	}
	if (M.use_flags.test(eSkyColor))
	{
		sky_color.mad(M.sky_color, _power);
		use_flags.set(eSkyColor, TRUE);
	}
	if (M.use_flags.test(eHemiColor))
	{
		hemi_color.mad(M.hemi_color, _power);
		use_flags.set(eHemiColor, TRUE);
	}

	return _power;
}

//-----------------------------------------------------------------------------
// Environment ambient
//-----------------------------------------------------------------------------
void CEnvAmbient::SSndChannel::load(CInifile& config, LPCSTR sect)
{
	m_load_section = sect;

	m_sound_dist.x = config.r_float(m_load_section, "min_distance");
	m_sound_dist.y = config.r_float(m_load_section, "max_distance");
	m_sound_period.x = config.r_s32(m_load_section, "period0");
	m_sound_period.y = config.r_s32(m_load_section, "period1");
	m_sound_period.z = config.r_s32(m_load_section, "period2");
	m_sound_period.w = config.r_s32(m_load_section, "period3");

	R_ASSERT(m_sound_period.x <= m_sound_period.y && m_sound_period.z <= m_sound_period.w);
	R_ASSERT2(m_sound_dist.y > m_sound_dist.x, sect);

	LPCSTR snds = config.r_string(sect, "sounds");
	u32 cnt = _GetItemCount(snds);
	string_path tmp;
	R_ASSERT3(cnt, "sounds empty", sect);

	m_sounds.resize(cnt);

	for (u32 k = 0; k < cnt; ++k)
	{
		_GetItem(snds, k, tmp);
		m_sounds[k].create(tmp, st_Effect, sg_SourceType);
	}
}

CEnvAmbient::SEffect* CEnvAmbient::create_effect(CInifile& config, LPCSTR id)
{
	SEffect* result = xr_new<SEffect>();
	result->life_time = iFloor(config.r_float(id, "life_time") * 1000.f);
	result->particles = config.r_string(id, "particles");
	VERIFY(result->particles.size());
	result->offset = config.r_fvector3(id, "offset");
	result->wind_gust_factor = config.r_float(id, "wind_gust_factor");

	if (config.line_exist(id, "sound"))
		result->sound.create(config.r_string(id, "sound"), st_Effect, sg_SourceType);

	if (config.line_exist(id, "wind_blast_strength"))
	{
		result->wind_blast_strength = config.r_float(id, "wind_blast_strength");
		result->wind_blast_direction.setHP(deg2rad(config.r_float(id, "wind_blast_longitude")), 0.f);
		result->wind_blast_in_time = config.r_float(id, "wind_blast_in_time");
		result->wind_blast_out_time = config.r_float(id, "wind_blast_out_time");
		return (result);
	}

	result->wind_blast_strength = 0.f;
	result->wind_blast_direction.set(0.f, 0.f, 1.f);
	result->wind_blast_in_time = 0.f;
	result->wind_blast_out_time = 0.f;

	return (result);
}

CEnvAmbient::SSndChannel* CEnvAmbient::create_sound_channel(CInifile& config, LPCSTR id)
{
	SSndChannel* result = xr_new<SSndChannel>();
	result->load(config, id);
	return (result);
}

CEnvAmbient::~CEnvAmbient()
{
	destroy();
}

void CEnvAmbient::destroy()
{
	delete_data(m_effects);
	delete_data(m_sound_channels);
}

void CEnvAmbient::load(CInifile& ambients_config, CInifile& sound_channels_config, CInifile& effects_config,
					   const shared_str& sect)
{
	m_ambients_config_filename = ambients_config.fname();
	m_load_section = sect;
	string_path tmp;

	// sounds
	LPCSTR channels = ambients_config.r_string(sect, "sound_channels");
	u32 cnt = _GetItemCount(channels);
	m_sound_channels.resize(cnt);

	for (u32 i = 0; i < cnt; ++i)
		m_sound_channels[i] = create_sound_channel(sound_channels_config, _GetItem(channels, i, tmp));

	// effects
	m_effect_period.set(iFloor(ambients_config.r_float(sect, "min_effect_period") * 1000.f),
						iFloor(ambients_config.r_float(sect, "max_effect_period") * 1000.f));
	LPCSTR effs = ambients_config.r_string(sect, "effects");
	cnt = _GetItemCount(effs);

	m_effects.resize(cnt);
	for (u32 k = 0; k < cnt; ++k)
		m_effects[k] = create_effect(effects_config, _GetItem(effs, k, tmp));

	R_ASSERT(!m_sound_channels.empty() || !m_effects.empty());
}

//-----------------------------------------------------------------------------
// Environment descriptor
//-----------------------------------------------------------------------------
CEnvDescriptor::CEnvDescriptor(shared_str const& identifier) : m_identifier(identifier)
{
	exec_time = 0.0f;
	exec_time_loaded = 0.0f;

	clouds_color.set(1, 1, 1, 1);
	sky_color.set(1, 1, 1);
	sky_rotation = 0.0f;

	far_plane = 400.0f;

	fog_color.set(1, 1, 1);
	fog_density = 0.0f;
	fog_sky_influence = 0.0f;
	vertical_fog_intensity = 0.0001f;
	vertical_fog_density = 0.0001f;
	vertical_fog_height = 0.8f;

	rain_density = 0.0f;
	rain_color.set(0, 0, 0);

	bolt_period = 0.0f;
	bolt_duration = 0.0f;

	wind_velocity = 0.0f;
	wind_direction = 0.0f;

	ambient.set(0, 0, 0);
	hemi_color.set(1, 1, 1, 1);
	sun_color.set(1, 1, 1);
	sun_dir.set(0, -1, 0);

	m_fSunShaftsIntensity = 0;
	m_fWaterIntensity = 1;

	lens_flare_id = "";
	tb_id = "";

	m_fTreeAmplitude = 0.005f;
	m_fTreeSpeed = 1.00f;
	m_fTreeRotation = 10.0f;
	m_fTreeWave.set(.1f, .01f, .11f);

	m_SepiaColor.set(1.0f, 1.0f, 1.0f);
	m_SepiaPower = 0.0f;
	m_VignettePower = 0.2f;

	env_ambient = NULL;
}

#define C_CHECK(C)                                                                                                     \
	if (C.x < 0 || C.x > 2 || C.y < 0 || C.y > 2 || C.z < 0 || C.z > 2)                                                \
	{                                                                                                                  \
		Msg("! Invalid '%s' in env-section '%s'", #C, m_identifier.c_str());                                           \
	}
void CEnvDescriptor::load(CEnvironment& environment, CInifile& config)
{
	Ivector3 tm = {0, 0, 0};
	sscanf(m_identifier.c_str(), "%d:%d:%d", &tm.x, &tm.y, &tm.z);
	R_ASSERT3((tm.x >= 0) && (tm.x < 24) && (tm.y >= 0) && (tm.y < 60) && (tm.z >= 0) && (tm.z < 60),
			  "Incorrect weather time", m_identifier.c_str());
	exec_time = tm.x * 3600.f + tm.y * 60.f + tm.z;
	exec_time_loaded = exec_time;
	string_path st, st_env;
	strcpy(st, config.r_string(m_identifier.c_str(), "sky_texture"));
	strconcat(sizeof(st_env), st_env, st, "#small");
	sky_texture_name = st;
	sky_texture_env_name = st_env;
	clouds_texture_name = config.r_string(m_identifier.c_str(), "clouds_texture");
	LPCSTR cldclr = config.r_string(m_identifier.c_str(), "clouds_color");
	float multiplier = 0, save = 0;
	sscanf(cldclr, "%f,%f,%f,%f,%f", &clouds_color.x, &clouds_color.y, &clouds_color.z, &clouds_color.w, &multiplier);
	save = clouds_color.w;
	clouds_color.mul(.5f * multiplier);
	clouds_color.w = save;

	sky_color = config.r_fvector3(m_identifier.c_str(), "sky_color");
	sky_color.mul(.5f);

	if (config.line_exist(m_identifier.c_str(), "sky_rotation"))
		sky_rotation = deg2rad(config.r_float(m_identifier.c_str(), "sky_rotation"));
	else
		sky_rotation = 0;
	far_plane = config.r_float(m_identifier.c_str(), "far_plane");
	fog_color = config.r_fvector3(m_identifier.c_str(), "fog_color");
	fog_density = config.r_float(m_identifier.c_str(), "fog_density");

	if (config.line_exist(m_identifier.c_str(), "fog_sky_influence"))
		fog_sky_influence = config.r_float(m_identifier.c_str(), "fog_sky_influence");
	else
		fog_sky_influence = 0.0000f;

	if (config.line_exist(m_identifier.c_str(), "vertical_fog_intensity"))
		vertical_fog_intensity = config.r_float(m_identifier.c_str(), "vertical_fog_intensity");
	else
		vertical_fog_intensity = 0.0001f;

	if (config.line_exist(m_identifier.c_str(), "vertical_fog_density"))
		vertical_fog_density = config.r_float(m_identifier.c_str(), "vertical_fog_density");
	else
		vertical_fog_density = 0.0001f;

	if (config.line_exist(m_identifier.c_str(), "vertical_fog_height"))
		vertical_fog_height = config.r_float(m_identifier.c_str(), "vertical_fog_height");
	else
		vertical_fog_height = 0.8f;

	rain_density = config.r_float(m_identifier.c_str(), "rain_density");
	clamp(rain_density, 0.f, 1.f);
	rain_color = config.r_fvector3(m_identifier.c_str(), "rain_color");
	wind_velocity = config.r_float(m_identifier.c_str(), "wind_velocity");
	wind_direction = deg2rad(config.r_float(m_identifier.c_str(), "wind_direction"));
	ambient = config.r_fvector3(m_identifier.c_str(), "ambient_color");
	hemi_color = config.r_fvector4(m_identifier.c_str(), "hemisphere_color");
	sun_color = config.r_fvector3(m_identifier.c_str(), "sun_color");

	if (config.line_exist(m_identifier.c_str(), "sun_dir"))
		sun_dir.setHP(deg2rad(config.r_fvector2(m_identifier.c_str(), "sun_dir").y),
					  deg2rad(config.r_fvector2(m_identifier.c_str(), "sun_dir").x));
	else
		sun_dir.setHP(deg2rad(config.r_float(m_identifier.c_str(), "sun_altitude")),
					  deg2rad(config.r_float(m_identifier.c_str(), "sun_longitude")));
	R_ASSERT(_valid(sun_dir));
	VERIFY2(sun_dir.y < 0, "Invalid sun direction settings while loading");

	lens_flare_id = environment.eff_LensFlare->AppendDef(environment, environment.m_suns_config,
														 config.r_string(m_identifier.c_str(), "sun"));
	tb_id = environment.eff_Thunderbolt->AppendDef(environment, environment.m_thunderbolt_collections_config,
												   environment.m_thunderbolts_config,
												   config.r_string(m_identifier.c_str(), "thunderbolt_collection"));
	bolt_period = (tb_id.size()) ? config.r_float(m_identifier.c_str(), "thunderbolt_period") : 0.f;
	bolt_duration = (tb_id.size()) ? config.r_float(m_identifier.c_str(), "thunderbolt_duration") : 0.f;
	env_ambient = config.line_exist(m_identifier.c_str(), "ambient")
					  ? environment.AppendEnvAmb(config.r_string(m_identifier.c_str(), "ambient"))
					  : 0;

	if (config.line_exist(m_identifier.c_str(), "sun_shafts_intensity"))
		m_fSunShaftsIntensity = config.r_float(m_identifier.c_str(), "sun_shafts_intensity");

	if (config.line_exist(m_identifier.c_str(), "water_intensity"))
		m_fWaterIntensity = config.r_float(m_identifier.c_str(), "water_intensity");

	m_fTreeAmplitude = config.line_exist(m_identifier.c_str(), "trees_amplitude")
						   ? config.r_float(m_identifier.c_str(), "trees_amplitude")
						   : 0.005f;
	m_fTreeSpeed = config.line_exist(m_identifier.c_str(), "trees_speed")
					   ? config.r_float(m_identifier.c_str(), "trees_speed")
					   : 1.00f;
	m_fTreeRotation = config.line_exist(m_identifier.c_str(), "trees_rotation")
						  ? config.r_float(m_identifier.c_str(), "trees_rotation")
						  : 10.0f;

	if (config.line_exist(m_identifier.c_str(), "trees_wave"))
		m_fTreeWave = config.r_fvector3(m_identifier.c_str(), "trees_wave");
	else
		m_fTreeWave.set(.1f, .01f, .11f);

	if (config.line_exist(m_identifier.c_str(), "sepia_color"))
		m_SepiaColor = config.r_fvector3(m_identifier.c_str(), "sepia_color");

	if (config.line_exist(m_identifier.c_str(), "sepia_power"))
		m_SepiaPower = config.r_float(m_identifier.c_str(), "sepia_power");

	if (config.line_exist(m_identifier.c_str(), "vignette_power"))
		m_VignettePower = config.r_float(m_identifier.c_str(), "vignette_power");

	C_CHECK(clouds_color);
	C_CHECK(sky_color);
	C_CHECK(fog_color);
	C_CHECK(rain_color);
	C_CHECK(ambient);
	C_CHECK(hemi_color);
	C_CHECK(sun_color);
	on_device_create();
}

void CEnvDescriptor::on_device_create()
{
	if (sky_texture_name.size())
		sky_texture.create(sky_texture_name.c_str());
	if (sky_texture_env_name.size())
		sky_texture_env.create(sky_texture_env_name.c_str());
	if (clouds_texture_name.size())
		clouds_texture.create(clouds_texture_name.c_str());
}

void CEnvDescriptor::on_device_destroy()
{
	sky_texture.destroy();
	sky_texture_env.destroy();
	clouds_texture.destroy();
}

//-----------------------------------------------------------------------------
// Environment Mixer
//-----------------------------------------------------------------------------
CEnvDescriptorMixer::CEnvDescriptorMixer(shared_str const& identifier) : CEnvDescriptor(identifier)
{
}

void CEnvDescriptorMixer::destroy()
{
	sky_r_textures.clear();
	sky_r_textures_env.clear();
	clouds_r_textures.clear();

	on_device_destroy();

	sky_texture.destroy();
	sky_texture_env.destroy();
	clouds_texture.destroy();
}

void CEnvDescriptorMixer::clear()
{
	std::pair<u32, ref_texture> zero = mk_pair(u32(0), ref_texture(0));
	sky_r_textures.clear();
	sky_r_textures.push_back(zero);
	sky_r_textures.push_back(zero);
	sky_r_textures.push_back(zero);

	sky_r_textures_env.clear();
	sky_r_textures_env.push_back(zero);
	sky_r_textures_env.push_back(zero);
	sky_r_textures_env.push_back(zero);

	clouds_r_textures.clear();
	clouds_r_textures.push_back(zero);
	clouds_r_textures.push_back(zero);
	clouds_r_textures.push_back(zero);
}

int get_ref_count(IUnknown* ii);

void CEnvDescriptorMixer::lerp(CEnvironment*, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& Mdf,
							   float modifier_power)
{
	float modif_power = 1.f / (modifier_power + 1); // the environment itself
	float fi = 1 - f;

	sky_r_textures.clear();
	sky_r_textures.push_back(mk_pair(0, A.sky_texture));
	sky_r_textures.push_back(mk_pair(1, B.sky_texture));

	sky_r_textures_env.clear();

	sky_r_textures_env.push_back(mk_pair(0, A.sky_texture_env));
	sky_r_textures_env.push_back(mk_pair(1, B.sky_texture_env));

	clouds_r_textures.clear();
	clouds_r_textures.push_back(mk_pair(0, A.clouds_texture));
	clouds_r_textures.push_back(mk_pair(1, B.clouds_texture));

	weight = f;

	clouds_color.lerp(A.clouds_color, B.clouds_color, f);

	sky_rotation = (fi * A.sky_rotation + f * B.sky_rotation);

	if (Mdf.use_flags.test(eViewDist))
		far_plane = (fi * A.far_plane + f * B.far_plane + Mdf.far_plane) * psVisDistance * modif_power;
	else
		far_plane = (fi * A.far_plane + f * B.far_plane) * psVisDistance;

	fog_color.lerp(A.fog_color, B.fog_color, f);
	if (Mdf.use_flags.test(eFogColor))
		fog_color.add(Mdf.fog_color).mul(modif_power);

	fog_density = (fi * A.fog_density + f * B.fog_density);
	if (Mdf.use_flags.test(eFogDensity))
	{
		fog_density += Mdf.fog_density;
		fog_density *= modif_power;
	}

	fog_sky_influence = (fi * A.fog_sky_influence + f * B.fog_sky_influence);
	if (Mdf.use_flags.test(eFogSkyInfluence))
	{
		fog_sky_influence += Mdf.fog_sky_influence;
		fog_sky_influence *= modif_power;
	}

	vertical_fog_intensity = (fi * A.vertical_fog_intensity + f * B.vertical_fog_intensity);

	vertical_fog_density = (fi * A.vertical_fog_density + f * B.vertical_fog_density);

	vertical_fog_height = (fi * A.vertical_fog_height + f * B.vertical_fog_height);

	rain_density = fi * A.rain_density + f * B.rain_density;
	rain_color.lerp(A.rain_color, B.rain_color, f);
	bolt_period = fi * A.bolt_period + f * B.bolt_period;
	bolt_duration = fi * A.bolt_duration + f * B.bolt_duration;

	// wind
	wind_velocity = fi * A.wind_velocity + f * B.wind_velocity;
	wind_direction = fi * A.wind_direction + f * B.wind_direction;

	// trees
	m_fTreeAmplitude = fi * A.m_fTreeAmplitude + f * B.m_fTreeAmplitude;
	m_fTreeSpeed = fi * A.m_fTreeSpeed + f * B.m_fTreeSpeed;
	m_fTreeRotation = fi * A.m_fTreeRotation + f * B.m_fTreeRotation;
	m_fTreeWave.lerp(A.m_fTreeWave, B.m_fTreeWave, f);

	m_fSunShaftsIntensity = fi * A.m_fSunShaftsIntensity + f * B.m_fSunShaftsIntensity;
	m_fWaterIntensity = fi * A.m_fWaterIntensity + f * B.m_fWaterIntensity;

	m_SepiaColor.lerp(A.m_SepiaColor, B.m_SepiaColor, f);
	m_SepiaPower = fi * A.m_SepiaPower + f * B.m_SepiaPower;
	m_VignettePower = fi * A.m_VignettePower + f * B.m_VignettePower;

	// colors
	sky_color.lerp(A.sky_color, B.sky_color, f);
	if (Mdf.use_flags.test(eSkyColor))
		sky_color.add(Mdf.sky_color).mul(modif_power);

	ambient.lerp(A.ambient, B.ambient, f);
	if (Mdf.use_flags.test(eAmbientColor))
		ambient.add(Mdf.ambient).mul(modif_power);

	hemi_color.lerp(A.hemi_color, B.hemi_color, f);

	if (Mdf.use_flags.test(eHemiColor))
	{
		hemi_color.x += Mdf.hemi_color.x;
		hemi_color.y += Mdf.hemi_color.y;
		hemi_color.z += Mdf.hemi_color.z;
		hemi_color.x *= modif_power;
		hemi_color.y *= modif_power;
		hemi_color.z *= modif_power;
	}

	sun_color.lerp(A.sun_color, B.sun_color, f);

	R_ASSERT(_valid(A.sun_dir));
	R_ASSERT(_valid(B.sun_dir));
	sun_dir.lerp(A.sun_dir, B.sun_dir, f).normalize();
	R_ASSERT(_valid(sun_dir));

	VERIFY2(sun_dir.y < 0, "Invalid sun direction settings while lerp");
}

//-----------------------------------------------------------------------------
// Environment IO
//-----------------------------------------------------------------------------
CEnvAmbient* CEnvironment::AppendEnvAmb(const shared_str& sect)
{
	for (EnvAmbVecIt it = Ambients.begin(); it != Ambients.end(); it++)
		if ((*it)->name().equal(sect))
			return (*it);

	Ambients.push_back(xr_new<CEnvAmbient>());
	Ambients.back()->load(*m_ambients_config, *m_sound_channels_config, *m_effects_config, sect);
	return (Ambients.back());
}

void CEnvironment::mods_load()
{
	Modifiers.clear_and_free();
	string_path path;
	if (FS.exist(path, "$level$", "level.env_mod"))
	{
		IReader* fs = FS.r_open(path);
		u32 id = 0;
		while (fs->find_chunk(id))
		{
			CEnvModifier E;
			// Надо обязательно проверять, что файл корректный, даже если чанк был найден. Real Wolf.
			if (!E.load(fs))
				break;
			Modifiers.push_back(E);
			id++;
		}
		FS.r_close(fs);
	}
}

void CEnvironment::load_level_specific_ambients()
{
	const shared_str level_name = g_pGameLevel->name();

	string_path path;
	strconcat(sizeof(path), path, "environment\\ambients\\", level_name.c_str(), ".ltx");

	string_path full_path;
	CInifile* level_ambients = xr_new<CInifile>(FS.update_path(full_path, "$game_config$", path), TRUE, TRUE, FALSE);

	for (EnvAmbVecIt I = Ambients.begin(), E = Ambients.end(); I != E; ++I)
	{
		CEnvAmbient* ambient = *I;

		shared_str section_name = ambient->name();

		// choose a source ini file
		CInifile* source =
			(level_ambients && level_ambients->section_exist(section_name)) ? level_ambients : m_ambients_config;

		// check and reload if needed
		if (xr_strcmp(ambient->get_ambients_config_filename().c_str(), source->fname()))
		{
			ambient->destroy();
			ambient->load(*source, *m_sound_channels_config, *m_effects_config, section_name);
		}
	}

	xr_delete(level_ambients);
}

void CEnvironment::mods_unload()
{
	Modifiers.clear_and_free();
}

CEnvDescriptor* CEnvironment::create_descriptor(shared_str const& identifier, CInifile* config)
{
	CEnvDescriptor* result = xr_new<CEnvDescriptor>(identifier);
	if (config)
		result->load(*this, *config);
	return (result);
}

void CEnvironment::load_weathers()
{
	if (!WeatherCycles.empty())
		return;

	typedef xr_vector<LPSTR> file_list_type;
	file_list_type* file_list = FS.file_list_open("$game_weathers$", "");
	VERIFY(file_list);

	file_list_type::const_iterator i = file_list->begin();
	file_list_type::const_iterator e = file_list->end();
	for (; i != e; ++i)
	{
		u32 length = xr_strlen(*i);
		VERIFY(length >= 4);
		VERIFY((*i)[length - 4] == '.');
		VERIFY((*i)[length - 3] == 'l');
		VERIFY((*i)[length - 2] == 't');
		VERIFY((*i)[length - 1] == 'x');
		u32 new_length = length - 4;
		LPSTR identifier = (LPSTR)_alloca((new_length + 1) * sizeof(char));
		Memory.mem_copy(identifier, *i, new_length * sizeof(char));
		identifier[new_length] = 0;
		EnvVec& env = WeatherCycles[identifier];

		string_path file_name;
		FS.update_path(file_name, "$game_weathers$", identifier);
		strcat(file_name, ".ltx");
		CInifile* config = CInifile::Create(file_name);

		typedef CInifile::Root sections_type;
		sections_type& sections = config->sections();

		env.reserve(sections.size());

		sections_type::const_iterator i = sections.begin();
		sections_type::const_iterator e = sections.end();
		for (; i != e; ++i)
		{
			CEnvDescriptor* object = create_descriptor((*i)->Name, config);
			env.push_back(object);
		}

		CInifile::Destroy(config);
	}

	FS.file_list_close(file_list);

	// sorting weather envs
	EnvsMapIt _I = WeatherCycles.begin();
	EnvsMapIt _E = WeatherCycles.end();
	for (; _I != _E; _I++)
	{
		R_ASSERT3(_I->second.size() > 1, "Environment in weather must >=2", *_I->first);
		std::sort(_I->second.begin(), _I->second.end(), sort_env_etl_pred);
	}
	R_ASSERT2(!WeatherCycles.empty(), "Empty weathers.");
	SetWeather((*WeatherCycles.begin()).first.c_str());
}

void CEnvironment::load_weather_effects()
{
	if (!WeatherFXs.empty())
		return;

	typedef xr_vector<LPSTR> file_list_type;
	file_list_type* file_list = FS.file_list_open("$game_weather_effects$", "");
	VERIFY(file_list);

	file_list_type::const_iterator i = file_list->begin();
	file_list_type::const_iterator e = file_list->end();
	for (; i != e; ++i)
	{
		u32 length = xr_strlen(*i);
		VERIFY(length >= 4);
		VERIFY((*i)[length - 4] == '.');
		VERIFY((*i)[length - 3] == 'l');
		VERIFY((*i)[length - 2] == 't');
		VERIFY((*i)[length - 1] == 'x');
		u32 new_length = length - 4;
		LPSTR identifier = (LPSTR)_alloca((new_length + 1) * sizeof(char));
		Memory.mem_copy(identifier, *i, new_length * sizeof(char));
		identifier[new_length] = 0;
		EnvVec& env = WeatherFXs[identifier];

		string_path file_name;
		FS.update_path(file_name, "$game_weather_effects$", identifier);
		strcat(file_name, ".ltx");
		CInifile* config = CInifile::Create(file_name);

		typedef CInifile::Root sections_type;
		sections_type& sections = config->sections();

		env.reserve(sections.size() + 2);
		env.push_back(create_descriptor("00:00:00", false));

		sections_type::const_iterator i = sections.begin();
		sections_type::const_iterator e = sections.end();
		for (; i != e; ++i)
		{
			CEnvDescriptor* object = create_descriptor((*i)->Name, config);
			env.push_back(object);
		}

		CInifile::Destroy(config);

		env.push_back(create_descriptor("24:00:00", false));
		env.back()->exec_time_loaded = DAY_LENGTH;
	}

	FS.file_list_close(file_list);

	// sorting weather envs
	EnvsMapIt _I = WeatherFXs.begin();
	EnvsMapIt _E = WeatherFXs.end();
	for (; _I != _E; _I++)
	{
		R_ASSERT3(_I->second.size() > 1, "Environment in weather must >=2", *_I->first);
		std::sort(_I->second.begin(), _I->second.end(), sort_env_etl_pred);
	}
}

void CEnvironment::load()
{
	if (!CurrentEnv)
		create_mixer();

	autoexposure = Device.Resources->_CreateTexture("$user$autoexposure");

	if (!eff_Rain)
		eff_Rain = xr_new<CEffect_Rain>();
	if (!eff_LensFlare)
		eff_LensFlare = xr_new<CLensFlare>();
	if (!eff_Thunderbolt)
		eff_Thunderbolt = xr_new<CEffect_Thunderbolt>();

	load_weathers();
	load_weather_effects();
}

void CEnvironment::unload()
{
	EnvsMapIt _I, _E;
	// clear weathers
	_I = WeatherCycles.begin();
	_E = WeatherCycles.end();
	for (; _I != _E; _I++)
	{
		for (EnvIt it = _I->second.begin(); it != _I->second.end(); it++)
			xr_delete(*it);
	}

	WeatherCycles.clear();
	// clear weather effect
	_I = WeatherFXs.begin();
	_E = WeatherFXs.end();
	for (; _I != _E; _I++)
	{
		for (EnvIt it = _I->second.begin(); it != _I->second.end(); it++)
			xr_delete(*it);
	}
	WeatherFXs.clear();
	// clear ambient
	for (EnvAmbVecIt it = Ambients.begin(); it != Ambients.end(); it++)
		xr_delete(*it);
	Ambients.clear();
	// misc
	xr_delete(eff_Rain);
	xr_delete(eff_LensFlare);
	xr_delete(eff_Thunderbolt);
	CurrentWeather = 0;
	CurrentWeatherName = 0;
	CurrentEnv->clear();
	Invalidate();
	autoexposure = 0;
}
