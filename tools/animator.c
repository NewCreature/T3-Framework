#include <allegro5/allegro_native_dialog.h>
#include "t3f/t3f.h"
#include "t3f/animation.h"

#define ANIMATOR_VIEW_BITMAPS 0
#define ANIMATOR_VIEW_FRAMES  1

/* data */
char filename[1024] = {0};
char last_animation_filename[1024] = {0};
char last_filename[1024] = {0};
ALLEGRO_FONT * font = NULL;
char * animation_fn = NULL;
T3F_ANIMATION * animation = NULL;
int current_frame = 0;
int current_bitmap = 0;
int view = ANIMATOR_VIEW_BITMAPS;
float zoom = 2.0;
unsigned long tick = 0;

/* mouse data */
bool pegged = false;
int pegx, pegy;

char * select_file(const char * initial, const char * title, const char * types, int flags)
{
	al_stop_timer(t3f_timer);
	ALLEGRO_FILECHOOSER * file_selector = NULL;
	const char * rpath = NULL;
	file_selector = al_create_native_file_dialog(initial, title, types, flags);
	al_show_native_file_dialog(t3f_display, file_selector);
	if(al_get_native_file_dialog_count(file_selector))
	{
		rpath = al_get_native_file_dialog_path(file_selector, 0);
	}
	if(rpath)
	{
		strcpy(filename, rpath);
	}
	al_destroy_native_file_dialog(file_selector);
	al_start_timer(t3f_timer);
	if(rpath)
	{
		return filename;
	}
	return NULL;
}

void global_logic(void)
{
	const char * fn = NULL;
	ALLEGRO_PATH * temp_path = NULL;
	
	if(t3f_key[ALLEGRO_KEY_F3])
	{
		fn = select_file(last_animation_filename, "Open Animation", "ani", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(fn)
		{
			if(animation)
			{
				t3f_destroy_animation(animation);
			}
			animation = t3f_load_animation(fn);
			strcpy(last_animation_filename, fn);
		}
		t3f_key[ALLEGRO_KEY_F3] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_F4])
	{
		animation = t3f_create_animation();
		t3f_key[ALLEGRO_KEY_F4] = 0;
	}
	if(animation)
	{
		if(t3f_key[ALLEGRO_KEY_F2])
		{
			fn = select_file(last_animation_filename, "Save Animation", "ani", ALLEGRO_FILECHOOSER_SAVE);
			if(fn)
			{
				temp_path = al_create_path(fn);
				if(temp_path)
				{
					al_set_path_extension(temp_path, ".t3a");
					t3f_save_animation(animation, al_path_cstr(temp_path, '/'));
					strcpy(last_animation_filename, al_path_cstr(temp_path, '/'));
					al_destroy_path(temp_path);
				}
			}
			t3f_key[ALLEGRO_KEY_F2] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_TAB])
		{
			view++;
			if(view > 1)
			{
				view = 0;
			}
			t3f_key[ALLEGRO_KEY_TAB] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_PLUS] && zoom < 10.0)
		{
			zoom += 1.0;
			t3f_key[ALLEGRO_KEY_PAD_PLUS] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_MINUS] && zoom > 1.0)
		{
			zoom -= 1.0;
			t3f_key[ALLEGRO_KEY_PAD_MINUS] = 0;
		}
	}
}

void bitmap_logic(void)
{
	const char * fn = NULL;
	ALLEGRO_BITMAP * bp = NULL;
	
	if(t3f_key[ALLEGRO_KEY_INSERT] || t3f_key[ALLEGRO_KEY_I])
	{
		fn = select_file(last_filename, "Open Image", "*.*;*.pcx;*.png;*.tga;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(fn)
		{
			bp = al_load_bitmap(fn);
			if(bp)
			{
				t3f_animation_add_bitmap(animation, bp);
				current_bitmap = animation->bitmaps - 1;
			}
			strcpy(last_filename, fn);
		}
		t3f_key[ALLEGRO_KEY_INSERT] = 0;
		t3f_key[ALLEGRO_KEY_I] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE])
	{
		if(current_bitmap < animation->bitmaps)
		{
			t3f_animation_delete_bitmap(animation, current_bitmap);
			if(current_bitmap >= animation->bitmaps)
			{
				current_bitmap = animation->bitmaps - 1;
			}
		}
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_ENTER])
	{
		fn = select_file(last_filename, "Open Image", "*.*;*.pcx;*.png;*.tga;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(fn)
		{
			bp = al_load_bitmap(fn);
			if(bp)
			{
				if(animation->bitmap[current_bitmap])
				{
					al_destroy_bitmap(animation->bitmap[current_bitmap]);
				}
				animation->bitmap[current_bitmap] = bp;
			}
			strcpy(last_filename, fn);
		}
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	if(animation->bitmaps > 0)
	{
		if(t3f_key[ALLEGRO_KEY_LEFT])
		{
			current_bitmap--;
			if(current_bitmap < 0)
			{
				current_bitmap = animation->bitmaps - 1;
			}
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT])
		{
			current_bitmap++;
			if(current_bitmap >= animation->bitmaps)
			{
				current_bitmap = 0;
			}
			t3f_key[ALLEGRO_KEY_RIGHT] = 0;
		}
	}
}

void frame_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_INSERT] || t3f_key[ALLEGRO_KEY_I])
	{
		t3f_animation_add_frame(animation, current_bitmap, 0, 0, 0, al_get_bitmap_width(animation->bitmap[current_bitmap]), al_get_bitmap_height(animation->bitmap[current_bitmap]), 0, 1);
		current_frame = animation->frames - 1;
		t3f_animation_build_frame_list(animation);
		t3f_key[ALLEGRO_KEY_INSERT] = 0;
		t3f_key[ALLEGRO_KEY_I] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE])
	{
		if(current_frame < animation->frames)
		{
			t3f_animation_delete_frame(animation, current_frame);
			if(current_frame >= animation->frames)
			{
				current_frame = animation->frames - 1;
			}
		}
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}
	if(animation->frames > 0)
	{
		if(t3f_key[ALLEGRO_KEY_LEFT])
		{
			current_frame--;
			if(current_frame < 0)
			{
				current_frame = animation->frames - 1;
			}
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT])
		{
			current_frame++;
			if(current_frame >= animation->frames)
			{
				current_frame = 0;
			}
			t3f_key[ALLEGRO_KEY_RIGHT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_UP])
		{
			animation->frame[current_frame]->ticks++;
			t3f_animation_build_frame_list(animation);
			t3f_key[ALLEGRO_KEY_UP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_DOWN])
		{
			if(animation->frame[current_frame]->ticks > 1)
			{
				animation->frame[current_frame]->ticks--;
				t3f_animation_build_frame_list(animation);
			}
			t3f_key[ALLEGRO_KEY_DOWN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_ASTERISK])
		{
			animation->frame[current_frame]->width *= 2.0;
			animation->frame[current_frame]->height *= 2.0;
			t3f_key[ALLEGRO_KEY_PAD_ASTERISK] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_SLASH])
		{
			animation->frame[current_frame]->width /= 2.0;
			animation->frame[current_frame]->height /= 2.0;
			t3f_key[ALLEGRO_KEY_PAD_SLASH] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_ENTER])
		{
			animation->frame[current_frame]->width = al_get_bitmap_width(animation->bitmap[animation->frame[current_frame]->bitmap]);
			animation->frame[current_frame]->height = al_get_bitmap_height(animation->bitmap[animation->frame[current_frame]->bitmap]);
			t3f_key[ALLEGRO_KEY_PAD_ENTER] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PGUP])
		{
			animation->frame[current_frame]->bitmap--;
			if(animation->frame[current_frame]->bitmap < 0)
			{
				animation->frame[current_frame]->bitmap = animation->bitmaps - 1;
			}
			t3f_key[ALLEGRO_KEY_PGUP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PGDN])
		{
			animation->frame[current_frame]->bitmap++;
			if(animation->frame[current_frame]->bitmap >= animation->bitmaps)
			{
				animation->frame[current_frame]->bitmap = 0;
			}
			t3f_key[ALLEGRO_KEY_PGDN] = 0;
		}
		
		if(t3f_mouse_button[0])
		{
			if(!pegged)
			{
				pegx = t3f_mouse_x;
				pegy = t3f_mouse_y;
				pegged = true;
			}
			animation->frame[current_frame]->x += (t3f_mouse_x - pegx) / zoom;
			animation->frame[current_frame]->y += (t3f_mouse_y - pegy) / zoom;
			pegx = t3f_mouse_x;
			pegy = t3f_mouse_y;
		}
		else
		{
			pegged = false;
		}
	}
}

void logic(void)
{
	global_logic();
	if(animation)
	{
		switch(view)
		{
			case ANIMATOR_VIEW_BITMAPS:
			{
				bitmap_logic();
				break;
			}
			case ANIMATOR_VIEW_FRAMES:
			{
				frame_logic();
				break;
			}
		}
		
		/* toggle animation type */
		if(t3f_key[ALLEGRO_KEY_O])
		{
			animation->flags ^= T3F_ANIMATION_FLAG_ONCE;
			t3f_animation_build_frame_list(animation);
			t3f_key[ALLEGRO_KEY_O] = 0;
		}
	}
	tick++;
}

void help_render(void)
{
	al_hold_bitmap_drawing(false);
	al_draw_filled_rectangle(0, 0, t3f_virtual_display_width, t3f_virtual_display_height, al_map_rgba_f(0.0, 0.0, 0.0, 0.75));
	al_hold_bitmap_drawing(true);
	al_draw_textf(font, t3f_color_white, 0, 0, 0, "Global Keys");
	al_draw_textf(font, t3f_color_white, 0, 16, 0, "-------------");
	al_draw_textf(font, t3f_color_white, 0, 32, 0, "F1: Help");
	al_draw_textf(font, t3f_color_white, 0, 48, 0, "F2: Save");
	al_draw_textf(font, t3f_color_white, 0, 64, 0, "F3: Load");
	al_draw_textf(font, t3f_color_white, 0, 80, 0, "F4: New");
	al_draw_textf(font, t3f_color_white, 0, 96, 0, "Tab: Change View");
	al_draw_textf(font, t3f_color_white, 0, 112, 0, "+/-: Zoom");

	al_draw_textf(font, t3f_color_white, 0, 144, 0, "Bitmap Keys");
	al_draw_textf(font, t3f_color_white, 0, 160, 0, "-------------");
	al_draw_textf(font, t3f_color_white, 0, 176, 0, "Insert: Add bitmap");
	al_draw_textf(font, t3f_color_white, 0, 192, 0, "Delete: Remove bitmap");
	al_draw_textf(font, t3f_color_white, 0, 208, 0, "Left/Right: Select bitmap");
	
	al_draw_textf(font, t3f_color_white, 0, 240, 0, "Frame Keys");
	al_draw_textf(font, t3f_color_white, 0, 256, 0, "------------");
	al_draw_textf(font, t3f_color_white, 0, 272, 0, "Insert: Add frame");
	al_draw_textf(font, t3f_color_white, 0, 288, 0, "Delete: Remove frame");
	al_draw_textf(font, t3f_color_white, 0, 304, 0, "Left/Right: Select frame");
	al_draw_textf(font, t3f_color_white, 0, 320, 0, "Up/Down: Change animation speed");
	al_draw_textf(font, t3f_color_white, 0, 336, 0, "*: Double size of frame");
	al_draw_textf(font, t3f_color_white, 0, 352, 0, "/: Half size of frame");
	al_draw_textf(font, t3f_color_white, 0, 368, 0, "Enter: Reset size of frame");
	al_draw_textf(font, t3f_color_white, 0, 384, 0, "Mouse Click: Move bitmap within frame");
}

void bitmap_render(void)
{
	al_draw_textf(font, t3f_color_white, 0, 360, 0, "bitmap[%d]", current_bitmap);
	if(current_bitmap < animation->bitmaps)
	{
		al_draw_scaled_bitmap(animation->bitmap[current_bitmap], 0, 0, al_get_bitmap_width(animation->bitmap[current_bitmap]), al_get_bitmap_height(animation->bitmap[current_bitmap]), 32, 32, (float)al_get_bitmap_width(animation->bitmap[current_bitmap]) * zoom, (float)al_get_bitmap_height(animation->bitmap[current_bitmap]) * zoom, 0);
	}
}

void frame_render(void)
{
	T3F_ANIMATION_FRAME * fp;
	if(current_frame < animation->frames)
	{
		al_draw_textf(font, t3f_color_white, 0, 360, 0, "frame[%d].ticks = %d", current_frame, animation->frame[current_frame]->ticks);
		al_draw_line(32, 32, 64, 32, al_map_rgba_f(0.0, 1.0, 0.0, 0.5), 1.0);
		al_draw_line(32, 32, 32, 64, al_map_rgba_f(0.0, 1.0, 0.0, 0.5), 1.0);
		fp = animation->frame[current_frame];
		t3f_draw_scaled_bitmap(animation->bitmap[fp->bitmap], t3f_color_white, 32 + fp->x * zoom, 32 + fp->y * zoom, fp->z, fp->width * zoom, fp->height * zoom, 0);
	}
	t3f_draw_scaled_animation(animation, t3f_color_white, tick, 320 + 32, 32, 0, zoom, 0);
}

void render(void)
{
	al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.1));
	al_hold_bitmap_drawing(true);
	if(animation)
	{
		switch(view)
		{
			case ANIMATOR_VIEW_BITMAPS:
			{
				bitmap_render();
				break;
			}
			case ANIMATOR_VIEW_FRAMES:
			{
				frame_render();
				break;
			}
		}
	}
	else
	{
		al_draw_textf(font, t3f_color_white, 0, 0, 0, "No animation loaded");
	}
	if(t3f_key[ALLEGRO_KEY_F1])
	{
		help_render();
	}
	al_hold_bitmap_drawing(false);
}

bool initialize(void)
{
	if(!t3f_initialize("T3F Animator", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE))
	{
		printf("Failed to initialize T3F.\n");
		return false;
	}
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	font = al_load_bitmap_font("fonts/basic_font.png");
	if(!font)
	{
		return false;
	}
	if(animation_fn)
	{
		animation = t3f_load_animation(animation_fn);
	}
	return true;
}

void check_arguments(int argc, char * argv[])
{
	if(argc > 1)
	{
		animation_fn = argv[1];
	}
}

int main(int argc, char * argv[])
{
	check_arguments(argc, argv);
	if(!initialize())
	{
		printf("Failed to initialize Animator.\n");
		return -1;
	}
	t3f_run();
	return 0;
}
