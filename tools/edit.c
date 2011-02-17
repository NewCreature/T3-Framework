#include <allegro5/allegro5.h>
#include "edit.h"

#include <allegro5/allegro5.h>
#include <allegro5/a5_primitives.h>
#include "../t3f/animation.h"
#include "../t3f/3d.h"

#include "studio.h"

void studio_edit_event_handler(ALLEGRO_EVENT * event)
{
	if(event->keyboard.keycode == ALLEGRO_KEY_LEFT)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_current_frame--;
			if(studio_current_frame < 0)
			{
				studio_current_frame = studio_animation->frames - 1;
			}
			studio_edit_tick--;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_RIGHT)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_current_frame++;
			if(studio_current_frame >= studio_animation->frames)
			{
				studio_current_frame = 0;
			}
			studio_edit_tick++;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_UP)
	{
		if(studio_animation && studio_animation->bitmaps > 0)
		{
			studio_current_bitmap++;
			if(studio_current_bitmap >= studio_animation->bitmaps)
			{
				studio_current_bitmap = 0;
			}
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_DOWN)
	{
		if(studio_animation && studio_animation->bitmaps > 0)
		{
			studio_current_bitmap--;
			if(studio_current_bitmap < 0)
			{
				studio_current_bitmap = studio_animation->bitmaps - 1;
			}
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_I)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_animation->frame[studio_current_frame]->y -= 1.0;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_K)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_animation->frame[studio_current_frame]->y += 1.0;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_J)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_animation->frame[studio_current_frame]->x -= 1.0;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_L)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_animation->frame[studio_current_frame]->x += 1.0;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_ENTER)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_animation->frame[studio_current_frame]->bitmap = studio_current_bitmap;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_A)
	{
		if(studio_animation && studio_animation->bitmaps > 0)
		{
			animation_add_frame(studio_animation, studio_current_bitmap, 0.0, 0.0, 0.0, al_get_bitmap_width(studio_animation->bitmap[studio_current_bitmap]), al_get_bitmap_height(studio_animation->bitmap[studio_current_bitmap]), 0.0, 1);
			animation_build_frame_list(studio_animation);
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_DELETE)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			animation_delete_frame(studio_animation, studio_current_frame);
			animation_build_frame_list(studio_animation);
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_COMMA)
	{
		if(studio_animation && studio_animation->frames > 0 && studio_animation->frame[studio_current_frame]->ticks > 1)
		{
			studio_animation->frame[studio_current_frame]->ticks--;
			animation_build_frame_list(studio_animation);
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_FULLSTOP)
	{
		if(studio_animation && studio_animation->frames > 0)
		{
			studio_animation->frame[studio_current_frame]->ticks++;
			animation_build_frame_list(studio_animation);
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_PAD_MINUS)
	{
		studio_scale -= 1.0;
		if(studio_scale < 1.0)
		{
			studio_scale = 1.0;
		}
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_PAD_PLUS)
	{
		studio_scale += 1.0;
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_1)
	{
		studio_animation->frame[studio_current_frame]->width = al_get_bitmap_width(studio_animation->bitmap[studio_animation->frame[studio_current_frame]->bitmap]);
		studio_animation->frame[studio_current_frame]->height = al_get_bitmap_height(studio_animation->bitmap[studio_animation->frame[studio_current_frame]->bitmap]);
	}
	if(event->keyboard.keycode == ALLEGRO_KEY_2)
	{
		studio_animation->frame[studio_current_frame]->width = al_get_bitmap_width(studio_animation->bitmap[studio_animation->frame[studio_current_frame]->bitmap]) * 2;
		studio_animation->frame[studio_current_frame]->height = al_get_bitmap_height(studio_animation->bitmap[studio_animation->frame[studio_current_frame]->bitmap]) * 2;
	}
	if(event->keyboard.keycode >= ALLEGRO_KEY_A && event->keyboard.keycode <= ALLEGRO_KEY_Z)
	{
		if((event->keyboard.modifiers & ALLEGRO_KEYMOD_CAPSLOCK && !(event->keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT)) || (event->keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT && !(event->keyboard.modifiers & ALLEGRO_KEYMOD_CAPSLOCK)))
		{
		}
		else
		{
		}
	}
	else if(event->keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
	{
	}
	else if(event->keyboard.keycode == ALLEGRO_KEY_ENTER)
	{
	}
	else if(event->keyboard.keycode == ALLEGRO_KEY_TAB)
	{
	}
}

void studio_edit_logic(void)
{
	ALLEGRO_COLOR color;
	if(studio_animation)
	{
		if(studio_left_held)
		{
			color = studio_left_color;
		}
		else if(studio_right_held)
		{
			color = studio_right_color;
		}
		if(studio_left_held || studio_right_held)
		{
			if(studio_mouse_x >= 320 && studio_mouse_x < 320.0 + al_get_bitmap_width(studio_animation->bitmap[studio_current_bitmap]) * studio_scale && studio_mouse_y >= 240 && studio_mouse_y < 240.0 + al_get_bitmap_height(studio_animation->bitmap[studio_current_bitmap]) * studio_scale)
			{
				ALLEGRO_BITMAP * oldtarget = al_get_target_bitmap();
				al_set_target_bitmap(studio_animation->bitmap[studio_current_bitmap]);
				al_put_pixel((studio_mouse_x - 320) / studio_scale, (studio_mouse_y - 240) / studio_scale, color);
				al_set_target_bitmap(oldtarget);
			}
			else if(studio_mouse_x >= 0 && studio_mouse_y >= 464 && studio_mouse_x < 16 && studio_mouse_y < 480)
			{
				studio_color = &studio_left_color;
				studio_state = STUDIO_STATE_PALETTE;
			}
			else if(studio_mouse_x >= 16 && studio_mouse_y >= 464 && studio_mouse_x < 32 && studio_mouse_y < 480)
			{
				studio_color = &studio_right_color;
				studio_state = STUDIO_STATE_PALETTE;
			}
		}
	}
	studio_angle += 0.01;
	studio_tick++;
}

void studio_edit_render(void)
{
	al_clear_to_color(al_map_rgb(0, 0, 64));
	if(studio_animation)
	{
		draw_animation(studio_animation, studio_tick, 0.0, 0.0, 0.0, 0);
		if(studio_animation->frames > 0)
		{
			t3f_3d_draw_scaled_bitmap(studio_animation->bitmap[studio_animation->frame[studio_current_frame]->bitmap], 320.0 + studio_animation->frame[studio_current_frame]->x, 0.0 + studio_animation->frame[studio_current_frame]->y, 0.0 + studio_animation->frame[studio_current_frame]->z, studio_animation->frame[studio_current_frame]->width, studio_animation->frame[studio_current_frame]->height, 0);
		}
		
		al_draw_textf(studio_font, 0, 240, 0, "Information");
		al_draw_textf(studio_font, 0, 252, 0, "-----------");
		if(studio_animation->frames > 0)
		{
			al_draw_textf(studio_font, 0, 264, 0, "Frame %d of %d", studio_current_frame + 1, studio_animation->frames);
			al_draw_textf(studio_font, 0, 288, 0, "Frame Info");
			al_draw_textf(studio_font, 0, 300, 0, "----------");
			al_draw_textf(studio_font, 0, 312, 0, "Bitmap %d of %d", studio_animation->frame[studio_current_frame]->bitmap + 1, studio_animation->bitmaps);
			al_draw_textf(studio_font, 0, 336, 0, "Duration = %d", studio_animation->frame[studio_current_frame]->ticks);
		}
		if(studio_animation->bitmaps > 0)
		{
			al_draw_scaled_bitmap(studio_animation->bitmap[studio_current_bitmap], 0.0, 0.0, al_get_bitmap_width(studio_animation->bitmap[studio_current_bitmap]), al_get_bitmap_height(studio_animation->bitmap[studio_current_bitmap]), 320.0, 240.0, al_get_bitmap_width(studio_animation->bitmap[studio_current_bitmap]) * studio_scale, al_get_bitmap_height(studio_animation->bitmap[studio_current_bitmap]) * studio_scale, 0);
		}
		
		al_draw_filled_rectangle(0, 464, 15, 479, studio_left_color);
		al_draw_filled_rectangle(16, 464, 31, 479, studio_right_color);
	}
}
