#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "t3f.h"
#include "music.h"

ALLEGRO_AUDIO_STREAM * t3f_stream = NULL;
ALLEGRO_MUTEX * t3f_music_mutex = NULL;
static float t3f_music_volume = 1.0;

static char t3f_music_thread_fn[4096] = {0};

static const char * t3f_get_music_extension(const char * fn)
{
	int i;
	
	for(i = strlen(fn); i >= 0; i--)
	{
		if(fn[i] == '.')
		{
			return &fn[i];
		}
	}
	return NULL;
}

static void * t3f_play_music_thread(void * arg)
{
	const char * ext = NULL;
	ALLEGRO_PATH * path = NULL;
	int loop_points = 0;
	float loop_start = -1;
	float loop_end = -1;
	bool loop_disabled = false;
	float gain = 1.0;
	const char * val = NULL;
	ALLEGRO_CONFIG * config = NULL;
	
	al_lock_mutex(t3f_music_mutex);
	if(t3f_stream)
	{
		t3f_stop_music();
	}
	t3f_stream = al_load_audio_stream(t3f_music_thread_fn, 4, 4096);
	if(!t3f_stream)
	{
		al_unlock_mutex(t3f_music_mutex);
		return NULL;
	}
	
	/* look for loop data */
	path = al_create_path(t3f_music_thread_fn);
	if(path)
	{
		al_set_path_extension(path, ".ini");
		config = al_load_config_file(al_path_cstr(path, '/'));
		if(config)
		{
			val = al_get_config_value(config, "loop", "disabled");
			if(val && !strcasecmp(val, "true"))
			{
				loop_disabled = true;
			}
			if(!loop_disabled)
			{
				val = al_get_config_value(config, "loop", "start");
				if(val)
				{
					loop_start = atof(val);
					loop_points++;
				}
				val = al_get_config_value(config, "loop", "end");
				if(val)
				{
					loop_end = atof(val);
					loop_points++;
				}
			}
			val = al_get_config_value(config, "settings", "gain");
			if(val)
			{
				gain = atof(val);
				if(gain < 0.0)
				{
					gain = 0;
				}
				if(gain > 10.0)
				{
					gain = 10.0;
				}
			}
			al_destroy_config(config);
		}
		al_destroy_path(path);
	}
	
	if(loop_disabled)
	{
		al_set_audio_stream_playmode(t3f_stream, ALLEGRO_PLAYMODE_ONCE);
	}
	else
	{
		if(loop_points != 2)
		{
			/* loop entire song unless audio is MOD music */
			ext = t3f_get_music_extension(t3f_music_thread_fn);
			if(strcmp(ext, ".xm") && strcmp(ext, ".it") && strcmp(ext, ".mod") && strcmp(ext, ".s3m"))
			{
				al_set_audio_stream_loop_secs(t3f_stream, 0.0, al_get_audio_stream_length_secs(t3f_stream));
				al_set_audio_stream_playmode(t3f_stream, ALLEGRO_PLAYMODE_LOOP);
			}
		}
		else
		{
			al_set_audio_stream_loop_secs(t3f_stream, loop_start, loop_end);
			al_set_audio_stream_playmode(t3f_stream, ALLEGRO_PLAYMODE_LOOP);
		}
	}
	
	al_set_audio_stream_gain(t3f_stream, t3f_music_volume * gain);
	al_attach_audio_stream_to_mixer(t3f_stream, al_get_default_mixer());
	al_unlock_mutex(t3f_music_mutex);
	return NULL;
}

/* need to come up with a way to define loops for non-MOD audio,
 * see if there is a corresponding INI file and read loop data from that */
bool t3f_play_music(const char * fn)
{
	if(!t3f_music_mutex)
	{
		t3f_music_mutex = al_create_mutex();
	}
	if(t3f_music_mutex)
	{
		strcpy(t3f_music_thread_fn, fn);
		al_run_detached_thread(t3f_play_music_thread, NULL);
		return true;
	}
	else
	{
		return false;
	}
}

void t3f_stop_music(void)
{
	if(t3f_stream)
	{
		al_drain_audio_stream(t3f_stream);
		al_destroy_audio_stream(t3f_stream);
		t3f_stream = NULL;
	}
}

void t3f_pause_music(void)
{
	al_set_audio_stream_playing(t3f_stream, false);
}

void t3f_resume_music(void)
{
	al_set_audio_stream_playing(t3f_stream, true);
}

void t3f_set_music_volume(float volume)
{
	t3f_music_volume = volume;
	if(t3f_stream)
	{
		al_set_audio_stream_gain(t3f_stream, t3f_music_volume);
	}
}

float t3f_get_music_volume(void)
{
	return t3f_music_volume;
}
