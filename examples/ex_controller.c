#include "t3f.h"
#include "controller.h"

#define CONTROL_UP    0
#define CONTROL_DOWN  1
#define CONTROL_LEFT  2
#define CONTROL_RIGHT 3
#define CONTROL_FLIP  4

ALLEGRO_BITMAP * bitmap = NULL;
ALLEGRO_FONT * font = NULL;
T3F_CONTROLLER * controller = NULL;

float x = 0.0;
float y = 0.0;
bool flip = false;

void logic(void)
{
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	t3f_read_controller(controller);
	t3f_update_controller(controller);
	if(controller->state[CONTROL_UP].held)
	{
		y -= 0.5;
	}
	if(controller->state[CONTROL_DOWN].held)
	{
		y += 0.5;
	}
	if(controller->state[CONTROL_LEFT].held)
	{
		x -= 0.5;
	}
	if(controller->state[CONTROL_RIGHT].held)
	{
		x += 0.5;
	}
	if(controller->state[CONTROL_FLIP].pressed)
	{
		flip = flip ? false : true;
	}
}

void render(void)
{
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_bitmap(bitmap, x, y, flip ? ALLEGRO_FLIP_VERTICAL : 0);
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_controller", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD))
	{
		return 1;
	}
	bitmap = al_load_bitmap("data/bitmap.png");
	if(!bitmap)
	{
		return 1;
	}
	font = t3f_load_font("data/font.png");
	if(!font)
	{
		return 1;
	}
	controller = t3f_create_controller(5);
	al_rest(0.5);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_text(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Press key/button for up...");
	al_flip_display();
	t3f_bind_controller(controller, CONTROL_UP);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_text(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Press key/button for down...");
	al_flip_display();
	t3f_bind_controller(controller, CONTROL_DOWN);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_text(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Press key/button for left...");
	al_flip_display();
	t3f_bind_controller(controller, CONTROL_LEFT);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_text(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Press key/button for right...");
	al_flip_display();
	t3f_bind_controller(controller, CONTROL_RIGHT);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_text(font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, "Press key/button for flip...");
	al_flip_display();
	t3f_bind_controller(controller, CONTROL_FLIP);
	t3f_run();
	return 0;
}
