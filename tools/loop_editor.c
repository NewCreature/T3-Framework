#include "t3f/t3f.h"
#include "t3f/music.h"
#include <allegro5/allegro_native_dialog.h>

#define LE_PROGRESS_Y  60.0
#define LE_ZOOM_Y     180.0

ALLEGRO_FILECHOOSER * le_file_selector = NULL;
ALLEGRO_FONT * le_font = NULL;
const char * le_filename = NULL;
char le_config_filename[PATH_MAX] = {0};
float le_music_pos = 0.0;
float le_music_length = 0.0;
float le_loop_start = 0.0;
float le_loop_end = 0.0;
float le_loop_range_start = 0.0;
float le_loop_range_end = 0.0;

void le_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	if(t3f_key[ALLEGRO_KEY_OPENBRACE])
	{
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			le_loop_start -= 0.001;
		}
		else
		{
			le_loop_end -= 0.001;
		}
		al_set_audio_stream_loop_secs(t3f_stream, le_loop_start, le_loop_end);
		t3f_key[ALLEGRO_KEY_OPENBRACE] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_CLOSEBRACE])
	{
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			le_loop_start += 0.001;
		}
		else
		{
			le_loop_end += 0.001;
		}
		al_set_audio_stream_loop_secs(t3f_stream, le_loop_start, le_loop_end);
		t3f_key[ALLEGRO_KEY_CLOSEBRACE] = 0;
	}
	le_music_pos = al_get_audio_stream_position_secs(t3f_stream);
	if(le_music_pos > le_loop_range_end && le_music_pos < le_loop_range_start)
	{
		al_seek_audio_stream_secs(t3f_stream, le_loop_range_start);
	}
}

void le_render(void)
{
	float ox, time;
	int i, ticks;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.25, 1.0));
	
	/* render overall view */
	al_draw_filled_rectangle(16.0, LE_PROGRESS_Y - 2.0, 640.0 - 16.0 - 1.0, LE_PROGRESS_Y + 2.0, al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	ox = (le_loop_start / le_music_length) * (640.0 - 32.0);
	al_draw_line(16.0 + ox + 0.5, LE_PROGRESS_Y - 8.0, 16.0 + ox + 0.5, LE_PROGRESS_Y + 8.0, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), 1.0);
	al_draw_textf(le_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 16.0 + ox, LE_PROGRESS_Y - 8.0 - al_get_font_line_height(le_font), ALLEGRO_ALIGN_CENTRE, "%3.3f", le_loop_start);
	ox = (le_loop_end / le_music_length) * (640.0 - 32.0);
	al_draw_line(16.0 + ox + 0.5, LE_PROGRESS_Y - 8.0, 16.0 + ox + 0.5, LE_PROGRESS_Y + 8.0, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), 1.0);
	al_draw_textf(le_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 16.0 + ox, LE_PROGRESS_Y - 8.0 - al_get_font_line_height(le_font), ALLEGRO_ALIGN_CENTRE, "%3.3f", le_loop_end);
	ox = (le_music_pos / le_music_length) * (640.0 - 32.0);
	al_draw_line(16.0 + ox + 0.5, LE_PROGRESS_Y - 8.0, 16.0 + ox + 0.5, LE_PROGRESS_Y + 8.0, al_map_rgba_f(0.0, 1.0, 0.0, 1.0), 1.0);
	
	/* render loop start close up */
	al_draw_filled_rectangle(16.0, LE_ZOOM_Y - 2.0, 320.0 - 16.0 - 1.0, LE_ZOOM_Y + 2.0, al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	ox = le_music_pos / 10.0;
	ticks = le_music_length * 10.0;
	for(i = 0; i < ticks; i++)
	{
		time = 0.1 * (float)i;
		ox = le_loop_start;
		if(time - le_loop_start > 0.0 && time - le_loop_start < le_music_length)
		{
			al_draw_line(16.0 + (float)i * 10.0 - le_loop_start * 10.0 + 0.5, LE_ZOOM_Y, 16.0 + (float)i * 10.0 - le_loop_start * 10.0 + 0.5, LE_ZOOM_Y + 4.0, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 1.0);
		}
	}
	ox = 160.0;
	al_draw_line(16.0 + ox + 0.5, LE_ZOOM_Y - 8.0, 16.0 + ox + 0.5, LE_ZOOM_Y + 8.0, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), 1.0);
}

bool le_initialize(void)
{
	ALLEGRO_PATH * path = NULL;
	ALLEGRO_CONFIG * config = NULL;
	const char * val;
	
	if(!t3f_initialize("T3F Loop Editor", 640, 240, 60.0, le_logic, le_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_SOUND))
	{
		return false;
	}
	le_font = al_load_bitmap_font("fonts/basic_font.png");
	if(!le_font)
	{
		return false;
	}
	le_file_selector = al_create_native_file_dialog(le_filename, "Select music file", "*.*;*.ogg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if(!le_file_selector)
	{
		return false;
	}
	al_show_native_file_dialog(t3f_display, le_file_selector);
	if(al_get_native_file_dialog_count(le_file_selector) < 1)
	{
		return false;
	}
	le_filename = al_get_native_file_dialog_path(le_file_selector, 0);
	if(!le_filename)
	{
		return false;
	}
	t3f_play_music(le_filename);
	al_rest(1.0);
	le_music_length = al_get_audio_stream_length_secs(t3f_stream);
	le_loop_end = le_music_length;
	path = al_create_path(le_filename);
	if(path)
	{
		al_set_path_extension(path, ".ini");
		strcpy(le_config_filename, al_path_cstr(path, '/'));
		config = al_load_config_file(le_config_filename);
		if(config)
		{
			val = al_get_config_value(config, "loop", "start");
			if(val)
			{
				le_loop_start = atof(val);
			}
			val = al_get_config_value(config, "loop", "end");
			if(val)
			{
				le_loop_end = atof(val);
			}
			al_destroy_config(config);
		}
	}
	le_loop_range_start = le_loop_end - 3.0;
	if(le_loop_range_start < le_loop_start)
	{
		le_loop_range_start = le_loop_start;
	}
	le_loop_range_end = le_loop_start + 3.0;
	if(le_loop_range_end > le_loop_end)
	{
		le_loop_range_end = le_loop_end;
	}
	return true;
}

void le_exit(void)
{
	ALLEGRO_CONFIG * config = NULL;
	char val[256];
	
	config = al_load_config_file(le_config_filename);
	if(config)
	{
		sprintf(val, "%f", le_loop_start);
		al_set_config_value(config, "loop", "start", val);
		sprintf(val, "%f", le_loop_end);
		al_set_config_value(config, "loop", "end", val);
		al_save_config_file(le_config_filename, config);
		al_destroy_config(config);
	}
}

int main(int argc, char * argv[])
{
	if(!le_initialize())
	{
		return -1;
	}
	t3f_run();
	le_exit();
	return 0;
}
