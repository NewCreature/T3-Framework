#include "t3f/t3f.h"
#include "t3f/gui.h"

T3F_BITMAP * bitmap = NULL;
T3F_FONT * font = NULL;
T3F_GUI * page = NULL;
float x = 0.0;
float y = 0.0;
char text[1024] = {0};
int text_pos = 0;

ALLEGRO_COLOR bgcolor;

void logic(void * data)
{
	if(t3f_key_held(ALLEGRO_KEY_ESCAPE))
	{
		t3f_exit();
	}
	t3f_process_gui(page, 0, data);
}

void render(void * data)
{
	al_clear_to_color(bgcolor);
	t3f_render_gui(page, 0);
}

int red_proc(void * d, int i, void * p)
{
	bgcolor = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
	return 1;
}

int green_proc(void * d, int i, void * p)
{
	bgcolor = al_map_rgba_f(0.0, 1.0, 0.0, 1.0);
	return 1;
}

int blue_proc(void * d, int i, void * p)
{
	bgcolor = al_map_rgba_f(0.0, 0.0, 1.0, 1.0);
	return 1;
}

int black_proc(void * d, int i, void * p)
{
	bgcolor = al_map_rgba_f(0.0, 0.0, 0.0, 1.0);
	return 1;
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_gui", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE, NULL))
	{
		return 1;
	}
	bitmap = t3f_load_bitmap("data/bitmap.png", false, 0);
	if(!bitmap)
	{
		return 1;
	}
	font = t3f_load_font("data/font.png", T3F_FONT_TYPE_AUTO, 0, 0, false);
	if(!font)
	{
		return 1;
	}
	t3f_set_gui_driver(NULL);
	page = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(page, red_proc, font, t3f_color_white, "Red", 16, 0, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(page, green_proc, font, t3f_color_white, "Green", 16, 24, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(page, blue_proc, font, t3f_color_white, "Blue", 16, 48, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_image_element(page, black_proc, bitmap, t3f_color_white, 16, 120, T3F_GUI_ELEMENT_SHADOW);
	t3f_run();
	return 0;
}
