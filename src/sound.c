#include <math.h>
#include <stdio.h>
#include "t3f.h"
#include "sound.h"

static float t3f_sound_volume = 1.0;
static ALLEGRO_SAMPLE * t3f_sample_queue[T3F_MAX_QUEUED_SAMPLES] = {NULL};
static int t3f_queued_samples = 0;
static ALLEGRO_SAMPLE_INSTANCE * t3f_queue_sample_instance = NULL;
static ALLEGRO_AUDIO_STREAM * _t3f_sample_stream[T3F_MAX_SAMPLE_STREAMS] = {NULL};
ALLEGRO_SAMPLE_ID t3f_sample_id;

void _t3f_clean_up_sound_data(void)
{
	int i;

	for(i = 0; i < T3F_MAX_SAMPLE_STREAMS; i++)
	{
		if(_t3f_sample_stream[i])
		{
			al_destroy_audio_stream(_t3f_sample_stream[i]);
			_t3f_sample_stream[i] = NULL;
		}
	}
}

void _t3f_get_sample_settings(const char * fn, float * loop_start, float * loop_end, bool * loop_disabled, float * gain)
{
	ALLEGRO_PATH * path = NULL;
	ALLEGRO_CONFIG * config = NULL;
	const char * val;

	path = al_create_path(fn);
	if(path)
	{
		al_set_path_extension(path, ".ini");
		config = al_load_config_file(al_path_cstr(path, '/'));
		if(config)
		{
			if(loop_disabled)
			{
				val = al_get_config_value(config, "loop", "disabled");
				if(val && !strcasecmp(val, "true"))
				{
					*loop_disabled = true;
				}
				else
				{
					*loop_disabled = false;
				}
			}
			if(loop_start)
			{
				val = al_get_config_value(config, "loop", "start");
				if(val)
				{
					*loop_start = atof(val);
				}
				else
				{
					*loop_start = 0.0;
				}
			}
			if(loop_end)
			{
				val = al_get_config_value(config, "loop", "end");
				if(val)
				{
					*loop_end = atof(val);
				}
				else
				{
					*loop_end = 0.0;
				}
			}
			if(gain)
			{
				val = al_get_config_value(config, "settings", "gain");
				if(val)
				{
					*gain = atof(val);
					if(*gain < 0.0)
					{
						*gain = 0;
					}
					if(*gain > 10.0)
					{
						*gain = 10.0;
					}
				}
				else
				{
					*gain = 1.0;
				}
			}
			al_destroy_config(config);
		}
		al_destroy_path(path);
	}
}

void t3f_set_sound_volume(float volume)
{
	t3f_sound_volume = volume;
}

float t3f_get_sound_volume(void)
{
	return t3f_sound_volume;
}

bool t3f_play_sample(ALLEGRO_SAMPLE * sp, float vol, float pan, float speed)
{
	if(t3f_flags & T3F_USE_SOUND)
	{
		return al_play_sample(sp, t3f_sound_volume * vol, pan, speed, ALLEGRO_PLAYMODE_ONCE, &t3f_sample_id);
	}
	return false;
}

ALLEGRO_AUDIO_STREAM * t3f_stream_sample(const char * fn, float vol, float pan, float speed)
{
	ALLEGRO_AUDIO_STREAM * ret = NULL;
	float loop_start, loop_end, gain;
	bool loop_disabled;
	int i;

	for(i = 0; i < T3F_MAX_SAMPLE_STREAMS; i++)
	{
		if(_t3f_sample_stream[i] && !al_get_audio_stream_playing(_t3f_sample_stream[i]))
		{
			al_destroy_audio_stream(_t3f_sample_stream[i]);
			_t3f_sample_stream[i] = NULL;
		}
		if(!_t3f_sample_stream[i])
		{
			_t3f_sample_stream[i] = al_load_audio_stream(fn, 4, 4096);
			if(_t3f_sample_stream[i])
			{
				_t3f_get_sample_settings(fn, &loop_start, &loop_end, &loop_disabled, &gain);
				if(loop_end <= loop_start)
				{
					loop_disabled = true;
				}
				ret = _t3f_sample_stream[i];
				if(loop_disabled)
				{
					al_set_audio_stream_playmode(_t3f_sample_stream[i], ALLEGRO_PLAYMODE_ONCE);
				}
				else
				{
					al_set_audio_stream_loop_secs(_t3f_sample_stream[i], loop_start, loop_end);
					al_set_audio_stream_playmode(_t3f_sample_stream[i], ALLEGRO_PLAYMODE_LOOP);
				}
				al_set_audio_stream_gain(_t3f_sample_stream[i], t3f_sound_volume * vol * gain);
				al_attach_audio_stream_to_mixer(_t3f_sample_stream[i], al_get_default_mixer());
			}
			break;
		}
	}
	return ret;
}

bool t3f_queue_sample(ALLEGRO_SAMPLE * sp)
{
	if((t3f_flags & T3F_USE_SOUND) && t3f_queued_samples < T3F_MAX_QUEUED_SAMPLES && sp)
	{
		t3f_sample_queue[t3f_queued_samples] = sp;
		t3f_queued_samples++;
		return true;
	}
	return false;
}

void t3f_clear_sample_queue(void)
{
	int i;

	if(t3f_flags & T3F_USE_SOUND)
	{
		for(i = 0; i < T3F_MAX_QUEUED_SAMPLES; i++)
		{
			t3f_sample_queue[i] = NULL;
		}
		t3f_queued_samples = 0;
		if(al_get_sample_instance_playing(t3f_queue_sample_instance))
		{
			al_stop_sample_instance(t3f_queue_sample_instance);
			al_destroy_sample_instance(t3f_queue_sample_instance);
			t3f_queue_sample_instance = NULL;
		}
	}
}

ALLEGRO_SAMPLE * t3f_get_queue_sample(void)
{
	return NULL;
}

static void t3f_play_queued_sample(void)
{
	int i;

	if(t3f_sample_queue[0])
	{
		t3f_queue_sample_instance = al_create_sample_instance(t3f_sample_queue[0]);
		al_set_sample_instance_gain(t3f_queue_sample_instance, t3f_sound_volume);
		al_set_sample_instance_speed(t3f_queue_sample_instance, 1.0);
		al_set_sample_instance_pan(t3f_queue_sample_instance, 0.0);
		al_set_sample_instance_playmode(t3f_queue_sample_instance, ALLEGRO_PLAYMODE_ONCE);
		al_attach_sample_instance_to_mixer(t3f_queue_sample_instance, al_get_default_mixer());
		al_play_sample_instance(t3f_queue_sample_instance);
		for(i = 0; i < t3f_queued_samples - 1; i++)
		{
			t3f_sample_queue[i] = t3f_sample_queue[i + 1];
		}
		t3f_sample_queue[i] = NULL;
		t3f_queued_samples--;
	}
}

void t3f_poll_sound_queue(void)
{
	if(t3f_flags & T3F_USE_SOUND)
	{
		/* a queued sample is playing */
		if(t3f_queue_sample_instance)
		{
			if(!al_get_sample_instance_playing(t3f_queue_sample_instance))
			{
				al_destroy_sample_instance(t3f_queue_sample_instance);
				t3f_queue_sample_instance = NULL;
			}
		}
		if(!t3f_queue_sample_instance)
		{
			t3f_play_queued_sample();
		}
	}
}

float t3f_get_sound_position(float earx, float eary, float soundx, float soundy)
{
	return -cos(atan2(eary - soundy, earx - soundx));
}

float t3f_get_sound_gain(float earx, float eary, float soundx, float soundy, float scale)
{
	float distance;

	distance = hypot(earx - soundx, eary - soundy);

	/* sound is out of hearing range */
	if(distance > scale)
	{
		return 0.0;
	}
	return t3f_sound_volume * (1.0 - distance / scale);
}
