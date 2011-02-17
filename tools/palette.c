#include <allegro5/allegro5.h>
#include "edit.h"

#include <allegro5/allegro5.h>
#include <allegro5/a5_primitives.h>
#include "../t3f/animation.h"

#include "studio.h"

static int pal_r = 0;
static int pal_g = 0;
static int pal_b = 0;
static int pal_a = 0;

void studio_palette_event_handler(ALLEGRO_EVENT * event)
{
	if(event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
	{
		*studio_color = al_map_rgba(pal_r, pal_g, pal_b, pal_a);
		studio_state = STUDIO_STATE_EDIT;
	}
}

void studio_palette_logic(void)
{
	if(studio_left_held)
	{
		if(studio_mouse_x >= 0 && studio_mouse_x < 16 && studio_mouse_y < 256)
		{
			pal_r = studio_mouse_y;
		}
		else if(studio_mouse_x >= 16 && studio_mouse_x < 32 && studio_mouse_y < 256)
		{
			pal_g = studio_mouse_y;
		}
		else if(studio_mouse_x >= 32 && studio_mouse_x < 48 && studio_mouse_y < 256)
		{
			pal_b = studio_mouse_y;
		}
		else if(studio_mouse_x >= 48 && studio_mouse_x < 64 && studio_mouse_y < 256)
		{
			pal_a = studio_mouse_y;
		}
	}
}

void studio_palette_render(void)
{
	al_clear_to_color(al_map_rgb(0, 0, 64));
	al_draw_filled_rectangle(0, pal_r, 15, pal_r + 3, al_map_rgba(128, 128, 128, 255));
	al_draw_filled_rectangle(16, pal_g, 31, pal_g + 3, al_map_rgba(128, 128, 128, 255));
	al_draw_filled_rectangle(32, pal_b, 47, pal_b + 3, al_map_rgba(128, 128, 128, 255));
	al_draw_filled_rectangle(48, pal_a, 63, pal_a + 3, al_map_rgba(128, 128, 128, 255));

	al_draw_filled_rectangle(320, 0, 320 + 31, 31, al_map_rgba(pal_r, pal_g, pal_b, pal_a));
}
