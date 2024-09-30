#include "t3f/t3f.h"
#include "t3f/music.h"
#include <allegro5/allegro_native_dialog.h>

ALLEGRO_FILECHOOSER * ge_file_selector = NULL;
ALLEGRO_FONT * ge_font = NULL;
const char * ge_filename = NULL;
char ge_config_filename[PATH_MAX] = {0};
float ge_music_pos = 0.0;
float ge_music_length = 0.0;
float ge_gain = 1.0;

bool ge_save_gain(void)
{
	ALLEGRO_PATH * path = NULL;
	ALLEGRO_CONFIG * config = NULL;
	char buffer[256] = {0};
	
	path = al_create_path(ge_filename);
	if(path)
	{
		al_set_path_extension(path, ".ini");
		strcpy(ge_config_filename, al_path_cstr(path, '/'));
		config = al_load_config_file(ge_config_filename);
		if(!config)
		{
			config = al_create_config();
		}
		if(config)
		{
			sprintf(buffer, "%1.2f", ge_gain);
			al_set_config_value(config, "settings", "gain", buffer);
			al_save_config_file(ge_config_filename, config);
			return true;
		}
	}
	return false;
}

void ge_logic(void * data)
{
	ALLEGRO_PATH * path = NULL;
	ALLEGRO_CONFIG * config = NULL;
	const char * val;
	
	if(t3f_key_pressed(ALLEGRO_KEY_ESCAPE))
	{
		t3f_exit();
	}
	if(t3f_key_pressed(ALLEGRO_KEY_F3))
	{
		ge_file_selector = al_create_native_file_dialog(ge_filename, "Select music file", "*.*;*.ogg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(ge_file_selector)
		{
			al_show_native_file_dialog(t3f_display, ge_file_selector);
			if(al_get_native_file_dialog_count(ge_file_selector) > 0)
			{
				ge_filename = al_get_native_file_dialog_path(ge_file_selector, 0);
				if(ge_filename)
				{
					t3f_play_music(ge_filename);
					al_rest(1.0);
					ge_music_length = al_get_audio_stream_length_secs(t3f_stream);
					path = al_create_path(ge_filename);
					if(path)
					{
						al_set_path_extension(path, ".ini");
						strcpy(ge_config_filename, al_path_cstr(path, '/'));
						config = al_load_config_file(ge_config_filename);
						if(config)
						{
							val = al_get_config_value(config, "settings", "gain");
							if(val)
							{
								ge_gain = atof(val);
							}
							al_destroy_config(config);
						}
					}
				}
			}
		}
		t3f_use_key_press(ALLEGRO_KEY_F3);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_UP))
	{
		ge_gain += 0.05;
		al_set_audio_stream_gain(t3f_stream, ge_gain);
		ge_save_gain();
		t3f_use_key_press(ALLEGRO_KEY_UP);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_DOWN))
	{
		ge_gain -= 0.05;
		al_set_audio_stream_gain(t3f_stream, ge_gain);
		if(ge_gain < 0.0)
		{
			ge_gain = 0.0;
		}
		ge_save_gain();
		t3f_use_key_press(ALLEGRO_KEY_DOWN);
	}
	if(t3f_stream)
	{
		ge_music_pos = al_get_audio_stream_position_secs(t3f_stream);
	}
}

void ge_render(void * data)
{
	float ox;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.25, 1.0));
	al_draw_filled_rectangle(16.0, 120.0 - 2.0, 640.0 - 16.0 - 1.0, 120.0 + 2.0, al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	
	/* render overall view */
	ox = (ge_music_pos / ge_music_length) * (640.0 - 32.0);
	al_draw_line(16.0 + ox + 0.5, 120.0 - 8.0, 16.0 + ox + 0.5, 120.0 + 8.0, al_map_rgba_f(0.0, 1.0, 0.0, 1.0), 1.0);
	al_draw_textf(ge_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Press F3 to load a song");
	al_draw_textf(ge_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 16, 0, "Gain = %f", ge_gain);
}

bool ge_initialize(void)
{
	if(!t3f_initialize("T3F Gain Editor", 640, 240, 60.0, ge_logic, ge_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_SOUND, NULL))
	{
		return false;
	}
	ge_font = al_load_bitmap_font("fonts/basic_font.png");
	if(!ge_font)
	{
		return false;
	}
	return true;
}

void ge_exit(void)
{
}

int main(int argc, char * argv[])
{
	if(!ge_initialize())
	{
		return -1;
	}
	t3f_run();
	ge_exit();
	return 0;
}
