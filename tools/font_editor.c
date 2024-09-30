#include <allegro5/allegro_native_dialog.h>
#include "t3f/t3f.h"
#include "t3f/vector.h"

ALLEGRO_FONT * font = NULL;
T3F_VECTOR_FONT * vector_font = NULL;
int character = 0;
float scale = 8.0;

void vfe_logic(void * data)
{
	if(t3f_key_pressed(ALLEGRO_KEY_F2) && vector_font)
	{
		ALLEGRO_FILECHOOSER * file_save_dialog = NULL;
		ALLEGRO_PATH * save_path = NULL;
		const char * rp = NULL;
	
		file_save_dialog = al_create_native_file_dialog(NULL, "Save Font", "*.t3vf", ALLEGRO_FILECHOOSER_SAVE);
		if(file_save_dialog)
		{
			al_stop_timer(t3f_timer);
			al_show_native_file_dialog(t3f_display, file_save_dialog);
			rp = al_get_native_file_dialog_path(file_save_dialog, 0);
			if(rp)
			{
				save_path = al_create_path(rp);
				al_set_path_extension(save_path, ".t3vf");
				t3f_save_vector_font(vector_font, al_path_cstr(save_path, '/'));
				al_destroy_path(save_path);
			}
			al_destroy_native_file_dialog(file_save_dialog);
			al_start_timer(t3f_timer);
		}
		t3f_use_key_press(ALLEGRO_KEY_F2);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_F3))
	{
		ALLEGRO_FILECHOOSER * file_load_dialog = NULL;
		const char * rp = NULL;
	
		file_load_dialog = al_create_native_file_dialog(NULL, "Load Object", "*.t3v", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(file_load_dialog)
		{
			al_stop_timer(t3f_timer);
			al_show_native_file_dialog(t3f_display, file_load_dialog);
			rp = al_get_native_file_dialog_path(file_load_dialog, 0);
			if(rp)
			{
				if(vector_font)
				{
					t3f_destroy_vector_font(vector_font);
				}
				vector_font = t3f_load_vector_font(rp);
			}
			al_destroy_native_file_dialog(file_load_dialog);
			al_start_timer(t3f_timer);
		}
		t3f_use_key_press(ALLEGRO_KEY_F3);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_F4))
	{
		if(vector_font)
		{
			t3f_destroy_vector_font(vector_font);
		}
		vector_font = t3f_create_vector_font();
		t3f_use_key_press(ALLEGRO_KEY_F4);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_F5))
	{
		ALLEGRO_FILECHOOSER * file_load_dialog = NULL;
		const char * rp = NULL;
		T3F_VECTOR_OBJECT * vp = NULL;
	
		file_load_dialog = al_create_native_file_dialog(NULL, "Load Object", "*.t3v", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(file_load_dialog)
		{
			al_stop_timer(t3f_timer);
			al_show_native_file_dialog(t3f_display, file_load_dialog);
			rp = al_get_native_file_dialog_path(file_load_dialog, 0);
			if(rp)
			{
				vp = t3f_load_vector_object(rp);
				if(vp)
				{
					if(vector_font->character[character])
					{
						t3f_remove_vector_character(vector_font, character);
					}
					t3f_add_vector_character(vector_font, character, vp, 32);
				}
			}
			al_destroy_native_file_dialog(file_load_dialog);
			al_start_timer(t3f_timer);
		}
		t3f_use_key_press(ALLEGRO_KEY_F5);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_RIGHT) && character < T3F_VECTOR_FONT_MAX_CHARACTERS - 1)
	{
		character++;
		t3f_use_key_press(ALLEGRO_KEY_RIGHT);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_LEFT) && character > 0)
	{
		character--;
		t3f_use_key_press(ALLEGRO_KEY_LEFT);
	}
	
	if(vector_font)
	{
		if(vector_font->character[character])
		{
			if(t3f_key_pressed(ALLEGRO_KEY_MINUS))
			{
				vector_font->character[character]->width -= 1.0;
				t3f_use_key_press(ALLEGRO_KEY_MINUS);
			}
			if(t3f_key_pressed(ALLEGRO_KEY_EQUALS))
			{
				vector_font->character[character]->width += 1.0;
				t3f_use_key_press(ALLEGRO_KEY_EQUALS);
			}
			if(t3f_key_pressed(ALLEGRO_KEY_DELETE))
			{
				t3f_remove_vector_character(vector_font, character);
				t3f_use_key_press(ALLEGRO_KEY_DELETE);
			}
		}
	}
}

void vfe_render(void * data)
{
	al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
	
	if(!vector_font)
	{
		al_draw_textf(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "No Object");
	}
	else
	{
		if(vector_font->character[character])
		{
			t3f_draw_morphed_vector_object(vector_font->character[character]->object, 32, 32, 0.0, scale, scale, scale, 1.0);
		}
		al_draw_textf(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Character: %c", character);
		t3f_draw_vector_text(vector_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 300, 0, 1.0, "abcdefghijklmnop");
		t3f_draw_vector_text(vector_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 350, 0, 1.0, "pqrstuvwxyzyx");
		t3f_draw_vector_text(vector_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 400, 0, 1.0, "1234567890:<>01");
	}
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("T3F Vector Object Editor", 640, 480, 60.0, vfe_logic, vfe_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE, NULL))
	{
		printf("failed to initialize\n");
		return -1;
	}
	font = al_load_bitmap_font("fonts/basic_font.png");
	if(!font)
	{
		return -1;
	}
	t3f_run();
	return 0;
}
