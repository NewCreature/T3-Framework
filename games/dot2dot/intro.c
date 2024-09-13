#include "t3f/t3f.h"
#include "main.h"

void dot_intro_logic(void)
{
	dot_tick++;
	if(t3f_mouse_button_pressed(0))
	{
		srand(time(0));
		dot_game_initialize();
	}
}

void dot_intro_render(void)
{
	int i;
	
	al_clear_to_color(al_map_rgb_f(0.5, 0.5, 0.5));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < 480 / 16; i++)
	{
		al_draw_bitmap(dot_bitmap[DOT_BITMAP_BALL_RED + i % 6]->bitmap, i * 16, sin((float)(i * 2 + dot_tick) / 10.0) * 32 + 64, 0);
		al_draw_bitmap(dot_bitmap[DOT_BITMAP_BALL_RED + i % 6]->bitmap, i * 16, cos((float)(i * 2 + dot_tick) / 10.0) * 32 + 480 - 64 - 16 - 1, 0);
	}
	dot_game_render_hud();
	dot_shadow_text(dot_font, t3f_color_white, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 240, 240 - 8, 2, 2, ALLEGRO_ALIGN_CENTRE, "Click anywhere to begin...");
	al_hold_bitmap_drawing(false);
}
