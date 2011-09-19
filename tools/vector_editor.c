#include <allegro5/allegro_native_dialog.h>
#include "../t3f/t3f.h"
#include "../t3f/vector.h"

ALLEGRO_FONT * font = NULL;
T3F_VECTOR_OBJECT * object = NULL;
int segment = -1;
int point = -1;
int hover_segment = -1;
int hover_point = -1;;
bool segment_started = false;
ALLEGRO_COLOR color;
bool old_mouse_b[2] = {false};

/* options */
float scale = 8.0;
int drawing_width = 32;
int drawing_height = 32;
bool grid_snap = true;

void ve_logic(void)
{
	int i, j;
	
	if(t3f_key[ALLEGRO_KEY_F2] && object)
	{
		ALLEGRO_FILECHOOSER * file_save_dialog = NULL;
		ALLEGRO_PATH * save_path = NULL;
		const char * rp = NULL;
	
		file_save_dialog = al_create_native_file_dialog(NULL, "Save Object", "*.t3v", ALLEGRO_FILECHOOSER_SAVE);
		if(file_save_dialog)
		{
			al_stop_timer(t3f_timer);
			al_show_native_file_dialog(t3f_display, file_save_dialog);
			rp = al_get_native_file_dialog_path(file_save_dialog, 0);
			if(rp)
			{
				save_path = al_create_path(rp);
				al_set_path_extension(save_path, ".t3v");
				t3f_save_vector_object(object, al_path_cstr(save_path, '/'));
				al_destroy_path(save_path);
			}
			al_destroy_native_file_dialog(file_save_dialog);
			al_start_timer(t3f_timer);
		}
		t3f_key[ALLEGRO_KEY_F2] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_F3])
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
				if(object)
				{
					t3f_destroy_vector_object(object);
				}
				object = t3f_load_vector_object(rp);
			}
			al_destroy_native_file_dialog(file_load_dialog);
			al_start_timer(t3f_timer);
		}
		t3f_key[ALLEGRO_KEY_F3] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_F4])
	{
		if(object)
		{
			t3f_destroy_vector_object(object);
		}
		object = t3f_create_vector_object();
		t3f_key[ALLEGRO_KEY_F4] = 0;
	}
	
	if(object)
	{
		if(t3f_key[ALLEGRO_KEY_DELETE])
		{
			if(segment < object->segments)
			{
				t3f_remove_vector_segment(object, segment);
				if(segment >= object->segments)
				{
					segment = 0;
				}
			}
			t3f_key[ALLEGRO_KEY_DELETE] = 0;
		}
		if(!t3f_mouse_button[0])
		{
			hover_segment = -1;
			hover_point = -1;
			for(i = 0; i < object->segments; i++)
			{
				for(j = 0; j < 2; j++)
				{
					if(t3f_mouse_x >= 32 + (object->segment[i]->point[j].x) * scale - 4.0 && t3f_mouse_x <= 32 + (object->segment[i]->point[j].x) * scale + 4.0 && t3f_mouse_y >= 32 + (object->segment[i]->point[j].y) * scale - 4.0 && t3f_mouse_y <= 32 + (object->segment[i]->point[j].y) * scale + 4.0)
					{
						hover_segment = i;
						hover_point = j;
						break;
					}
				}
			}
		}
		
		if(t3f_mouse_button[0])
		{
			if(hover_segment >= 0 && hover_point >= 0)
			{
				segment = hover_segment;
				point = hover_point;
			}
			if(segment >= 0 && point >= 0)
			{
				if(grid_snap)
				{
					object->segment[segment]->point[point].x = (int)((float)(t3f_mouse_x - 32) / scale + 0.5);
					object->segment[segment]->point[point].y = (int)((float)(t3f_mouse_y - 32) / scale + 0.5);
				}
				else
				{
					object->segment[segment]->point[point].x = (float)(t3f_mouse_x - 32) / scale;
					object->segment[segment]->point[point].y = (float)(t3f_mouse_y - 32) / scale;
				}
			}
		}
		
		if(t3f_mouse_button[1])
		{
			if(!old_mouse_b[1])
			{
				if(!segment_started)
				{
					if(grid_snap)
					{
						t3f_add_vector_segment(object, (int)((float)(t3f_mouse_x - 32) / scale + 0.5), (int)((float)(t3f_mouse_y - 32) / scale + 0.5), 0.0, (int)((float)(t3f_mouse_x - 32) / scale + 0.5), (int)((float)(t3f_mouse_y - 32) / scale + 0.5), 0.0, color, 1.0);
					}
					else
					{
						t3f_add_vector_segment(object, (float)(t3f_mouse_x - 32) / scale, (float)(t3f_mouse_y - 32) / scale, 0.0, (float)(t3f_mouse_x - 32) / scale, (float)(t3f_mouse_y - 32) / scale, 0.0, color, 1.0);
					}
					segment = object->segments - 1;
					segment_started = true;
				}
				else
				{
					segment_started = false;
				}
				old_mouse_b[1] = true;
			}
		}
		else
		{
			old_mouse_b[1] = false;
		}
		if(segment_started)
		{
			if(grid_snap)
			{
				object->segment[object->segments - 1]->point[1].x = (int)((float)(t3f_mouse_x - 32) / scale + 0.5);
				object->segment[object->segments - 1]->point[1].y = (int)((float)(t3f_mouse_y - 32) / scale + 0.5);
			}
			else
			{
				object->segment[object->segments - 1]->point[1].x = (float)(t3f_mouse_x - 32) / scale;
				object->segment[object->segments - 1]->point[1].y = (float)(t3f_mouse_y - 32) / scale;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				object->segment[object->segments - 1]->thickness += 0.5;
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN] && object->segment[object->segments - 1]->thickness > 0.0)
			{
				object->segment[object->segments - 1]->thickness -= 0.5;
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
		}
	}
}

void ve_render(void)
{
	int i;
	
	al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
	
	/* render guides */
	for(i = 0; i < drawing_height + 1; i++)
	{
		al_draw_line(32.0, 32.0 + (float)i * scale, 32.0 + drawing_width * scale, 32.0 + (float)i * scale, (i == 0 || i == drawing_height) ? al_map_rgba_f(1.0, 1.0, 1.0, 0.75) : al_map_rgba_f(1.0, 1.0, 1.0, 0.25), 1.0);
	}
	for(i = 0; i < drawing_width + 1; i++)
	{
		al_draw_line(32.0 + (float)i * scale, 32, 32.0 + i * scale, 32.0 + (float)drawing_height * scale, (i == 0 || i == drawing_height) ? al_map_rgba_f(1.0, 1.0, 1.0, 0.75) : al_map_rgba_f(1.0, 1.0, 1.0, 0.25), 1.0);
	}
//	al_draw_rectangle(32, 32, 32 + drawing_width * scale, 32 + drawing_height * scale, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 1.0);
	
	if(!object)
	{
		al_draw_textf(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "No Object");
	}
	else
	{
		t3f_draw_morphed_vector_object(object, 32, 32, 0.0, scale, scale, scale, 1.0);
		al_draw_textf(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Segment: %d (Hover: %d - %d)", segment, hover_segment, hover_point);
	}
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("T3F Vector Object Editor", 640, 480, 60.0, ve_logic, ve_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE))
	{
		return -1;
	}
	font = al_load_bitmap_font("fonts/basic_font.png");
	if(!font)
	{
		return -1;
	}
	color = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
	t3f_run();
	return 0;
}
