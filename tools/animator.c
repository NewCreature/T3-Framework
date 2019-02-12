#include <allegro5/allegro_native_dialog.h>
#include "t3f/t3f.h"
#include "t3f/animation.h"
#include "t3f/draw.h"
#include "t3f/menu.h"

#define ANIMATOR_VIEW_BITMAPS 0
#define ANIMATOR_VIEW_FRAMES  1

/* data */
char filename[1024] = {0};
char last_animation_filename[1024] = {0};
char last_bitmap_filename[1024] = {0};
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

/* menus */
ALLEGRO_MENU * main_menu;
ALLEGRO_MENU * file_menu;
ALLEGRO_MENU * view_menu;
ALLEGRO_MENU * bitmap_menu;
ALLEGRO_MENU * frame_menu;

void read_config(void)
{
	const char * val;

	val = al_get_config_value(t3f_config, "Preferences", "last_animation_filename");
	if(val)
	{
		strcpy(last_animation_filename, val);
	}

	val = al_get_config_value(t3f_config, "Preferences", "last_bitmap_filename");
	if(val)
	{
		strcpy(last_bitmap_filename, val);
	}
}

void update_config(void)
{
	al_set_config_value(t3f_config, "Preferences", "last_animation_filename", last_animation_filename);
	al_set_config_value(t3f_config, "Preferences", "last_bitmap_filename", last_bitmap_filename);
}

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

int menu_update_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		al_set_menu_item_flags(menu, item, 0);
	}
	else
	{
		al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

int menu_update_bitmap_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		if(view == ANIMATOR_VIEW_BITMAPS)
		{
			al_set_menu_item_flags(menu, item, 0);
			return 0;
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_bitmap_delete_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		if(view == ANIMATOR_VIEW_BITMAPS)
		{
			if(animation->bitmaps->count)
			{
				al_set_menu_item_flags(menu, item, 0);
				return 0;
			}
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_bitmap_iter_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		if(view == ANIMATOR_VIEW_BITMAPS)
		{
			if(animation->bitmaps->count > 1)
			{
				al_set_menu_item_flags(menu, item, 0);
				return 0;
			}
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_mode_bitmaps_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		if(view == ANIMATOR_VIEW_BITMAPS)
		{
			al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_CHECKED);
		}
		else
		{
			al_set_menu_item_flags(menu, item, 0);
		}
	}
	else
	{
		al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

int menu_update_mode_frames_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		if(view == ANIMATOR_VIEW_FRAMES)
		{
			al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_CHECKED);
		}
		else
		{
			al_set_menu_item_flags(menu, item, 0);
		}
	}
	else
	{
		al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

int menu_update_frame_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation && animation->bitmaps->count)
	{
		if(view == ANIMATOR_VIEW_FRAMES)
		{
			al_set_menu_item_flags(menu, item, 0);
			return 0;
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_frames_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation)
	{
		if(view == ANIMATOR_VIEW_BITMAPS && animation->bitmaps->count)
		{
			al_set_menu_item_flags(menu, item, 0);
			return 0;
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_frame_delete_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation && animation->bitmaps->count)
	{
		if(view == ANIMATOR_VIEW_FRAMES)
		{
			if(animation->frames)
			{
				al_set_menu_item_flags(menu, item, 0);
				return 0;
			}
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_frame_tick_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation && animation->bitmaps->count)
	{
		if(view == ANIMATOR_VIEW_FRAMES)
		{
			if(animation->frames && animation->frame[current_frame]->ticks > 1)
			{
				al_set_menu_item_flags(menu, item, 0);
				return 0;
			}
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_update_frame_iter_proc(ALLEGRO_MENU * menu, int item, void * data)
{
	if(animation && animation->bitmaps->count)
	{
		if(view == ANIMATOR_VIEW_FRAMES)
		{
			if(animation->frames > 1)
			{
				al_set_menu_item_flags(menu, item, 0);
				return 0;
			}
		}
	}
	al_set_menu_item_flags(menu, item, ALLEGRO_MENU_ITEM_DISABLED);
	return 0;
}

int menu_proc_file_new(int i, void * data)
{
	if(animation)
	{
		t3f_destroy_animation(animation);
	}
	animation = t3f_create_animation();
	t3f_refresh_menus();
	return 0;
}

int menu_proc_file_load(int i, void * data)
{
	const char * filename;

	filename = select_file(last_animation_filename, "Load Animation", "*.t3a", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if(filename)
	{
		if(animation)
		{
			t3f_destroy_animation(animation);
		}
		strcpy(last_animation_filename, filename);
		animation = t3f_load_animation(last_animation_filename);
		update_config();
		t3f_refresh_menus();
	}
	return 0;
}

int menu_proc_file_save(int i, void * data)
{
	t3f_save_animation(animation, last_animation_filename);
	return 0;
}

int menu_proc_file_save_as(int i, void * data)
{
	const char * filename;

	filename = select_file(last_animation_filename, "Save Animation", "*.t3a", ALLEGRO_FILECHOOSER_SAVE);
	if(filename)
	{
		strcpy(last_animation_filename, filename);
		menu_proc_file_save(0, data);
		update_config();
	}
	return 0;
}

int menu_proc_file_quit(int i, void * data)
{
	t3f_exit();
	return 0;
}

int menu_proc_view_bitmaps(int i, void * data)
{
	view = ANIMATOR_VIEW_BITMAPS;
	t3f_refresh_menus();
	return 0;
}

int menu_proc_view_frames(int i, void * data)
{
	view = ANIMATOR_VIEW_FRAMES;
	t3f_refresh_menus();
	return 0;
}

int menu_proc_bitmap_add(int i, void * data)
{
	ALLEGRO_BITMAP * bp;
	const char * fn;

	fn = select_file(last_bitmap_filename, "Open Image", "*.*;*.pcx;*.png;*.tga;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if(fn)
	{
		bp = al_load_bitmap(fn);
		if(bp)
		{
			t3f_animation_add_bitmap(animation, bp);
			current_bitmap = animation->bitmaps->count - 1;
		}
		strcpy(last_bitmap_filename, fn);
		update_config();
		t3f_refresh_menus();
	}
	return 0;
}

int menu_proc_bitmap_delete(int i, void * data)
{
	if(current_bitmap < animation->bitmaps->count)
	{
		t3f_animation_delete_bitmap(animation, current_bitmap);
		if(current_bitmap >= animation->bitmaps->count)
		{
			current_bitmap = animation->bitmaps->count - 1;
		}
		t3f_refresh_menus();
	}
	return 0;
}

int menu_proc_bitmap_previous(int i, void * data)
{
	current_bitmap--;
	if(current_bitmap < 0)
	{
		current_bitmap = animation->bitmaps->count - 1;
	}
	return 0;
}

int menu_proc_bitmap_next(int i, void * data)
{
	current_bitmap++;
	if(current_bitmap >= animation->bitmaps->count)
	{
		current_bitmap = 0;
	}
	return 0;
}

int menu_proc_bitmap_load(int i, void * data)
{
	ALLEGRO_BITMAP * bp;
	const char * fn;

	fn = select_file(last_bitmap_filename, "Open Image", "*.*;*.pcx;*.png;*.tga;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if(fn)
	{
		bp = al_load_bitmap(fn);
		if(bp)
		{
			if(animation->bitmaps->bitmap[current_bitmap])
			{
				al_destroy_bitmap(animation->bitmaps->bitmap[current_bitmap]);
			}
			animation->bitmaps->bitmap[current_bitmap] = bp;
		}
		strcpy(last_bitmap_filename, fn);
	}
	return 0;
}

int menu_proc_frame_add(int i, void * data)
{
	t3f_animation_add_frame(animation, current_bitmap, 0, 0, 0, al_get_bitmap_width(animation->bitmaps->bitmap[current_bitmap]), al_get_bitmap_height(animation->bitmaps->bitmap[current_bitmap]), 0, 1, 0);
	current_frame = animation->frames - 1;
	t3f_animation_build_frame_list(animation);
	t3f_refresh_menus();
	return 0;
}

int menu_proc_frame_delete(int i, void * data)
{
	if(current_frame < animation->frames)
	{
		t3f_animation_delete_frame(animation, current_frame);
		if(current_frame >= animation->frames)
		{
			current_frame = animation->frames - 1;
		}
		t3f_refresh_menus();
	}
	return 0;
}

int menu_proc_frame_previous_bitmap(int i, void * data)
{
	animation->frame[current_frame]->bitmap--;
	if(animation->frame[current_frame]->bitmap < 0)
	{
		animation->frame[current_frame]->bitmap = animation->bitmaps->count - 1;
	}
	return 0;
}

int menu_proc_frame_next_bitmap(int i, void * data)
{
	animation->frame[current_frame]->bitmap++;
	if(animation->frame[current_frame]->bitmap >= animation->bitmaps->count)
	{
		animation->frame[current_frame]->bitmap = 0;
	}
	return 0;
}

int menu_proc_frame_double(int i, void * data)
{
	animation->frame[current_frame]->width *= 2.0;
	animation->frame[current_frame]->height *= 2.0;
	return 0;
}

int menu_proc_frame_half(int i, void * data)
{
	animation->frame[current_frame]->width /= 2.0;
	animation->frame[current_frame]->height /= 2.0;
	return 0;
}

int menu_proc_frame_reset(int i, void * data)
{
	animation->frame[current_frame]->width = al_get_bitmap_width(animation->bitmaps->bitmap[animation->frame[current_frame]->bitmap]);
	animation->frame[current_frame]->height = al_get_bitmap_height(animation->bitmaps->bitmap[animation->frame[current_frame]->bitmap]);
	return 0;
}

int menu_proc_frame_lengthen(int i, void * data)
{
	animation->frame[current_frame]->ticks++;
	t3f_animation_build_frame_list(animation);
	return 0;
}

int menu_proc_frame_shorten(int i, void * data)
{
	animation->frame[current_frame]->ticks--;
	t3f_animation_build_frame_list(animation);
	return 0;
}

int menu_proc_frame_previous(int i, void * data)
{
	current_frame--;
	if(current_frame < 0)
	{
		current_frame = animation->frames - 1;
	}
	return 0;
}

int menu_proc_frame_next(int i, void * data)
{
	current_frame++;
	if(current_frame >= animation->frames)
	{
		current_frame = 0;
	}
	return 0;
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

ALLEGRO_BITMAP * pad_bitmap(ALLEGRO_BITMAP * bp)
{
	ALLEGRO_BITMAP * ret_bp = NULL;
	ALLEGRO_STATE old_state;

	ret_bp = al_create_bitmap(al_get_bitmap_width(bp) + 2, al_get_bitmap_height(bp) + 2);
	if(ret_bp)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
		al_set_target_bitmap(ret_bp);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		al_draw_bitmap(bp, 1, 1, 0);
		al_restore_state(&old_state);
		al_destroy_bitmap(bp);
		return ret_bp;
	}
	return NULL;
}

void bitmap_logic(void)
{
	int i;

	if(t3f_key[ALLEGRO_KEY_INSERT] || t3f_key[ALLEGRO_KEY_I])
	{
		menu_proc_bitmap_add(0, NULL);
		t3f_key[ALLEGRO_KEY_INSERT] = 0;
		t3f_key[ALLEGRO_KEY_I] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE])
	{
		menu_proc_bitmap_delete(0, NULL);
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_ENTER])
	{
		menu_proc_bitmap_load(0, NULL);
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_P])
	{
		for(i = 0; i < animation->bitmaps->count; i++)
		{
			animation->bitmaps->bitmap[i] = pad_bitmap(animation->bitmaps->bitmap[i]);
		}
		for(i = 0; i < animation->frames; i++)
		{
			animation->frame[i]->x -= 1.0;
			animation->frame[i]->y -= 1.0;
			animation->frame[i]->width += 2.0;
			animation->frame[i]->height += 2.0;
		}
		t3f_key[ALLEGRO_KEY_P] = 0;
	}
	if(animation->bitmaps > 0)
	{
		if(t3f_key[ALLEGRO_KEY_LEFT])
		{
			menu_proc_bitmap_previous(0, NULL);
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT])
		{
			menu_proc_bitmap_next(0, NULL);
			t3f_key[ALLEGRO_KEY_RIGHT] = 0;
		}
	}
}

void frame_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_INSERT] || t3f_key[ALLEGRO_KEY_I])
	{
		menu_proc_frame_add(0, NULL);
		t3f_key[ALLEGRO_KEY_INSERT] = 0;
		t3f_key[ALLEGRO_KEY_I] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE])
	{
		menu_proc_frame_delete(0, NULL);
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}
	if(animation->frames > 0)
	{
		if(t3f_key[ALLEGRO_KEY_LEFT])
		{
			menu_proc_frame_previous(0, NULL);
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT])
		{
			menu_proc_frame_next(0, NULL);
			t3f_key[ALLEGRO_KEY_RIGHT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_UP])
		{
			menu_proc_frame_lengthen(0, NULL);
			t3f_key[ALLEGRO_KEY_UP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_DOWN])
		{
			if(animation->frame[current_frame]->ticks > 1)
			{
				menu_proc_frame_shorten(0, NULL);
			}
			t3f_key[ALLEGRO_KEY_DOWN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_ASTERISK])
		{
			menu_proc_frame_double(0, NULL);
			t3f_key[ALLEGRO_KEY_PAD_ASTERISK] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_SLASH])
		{
			menu_proc_frame_half(0, NULL);
			t3f_key[ALLEGRO_KEY_PAD_SLASH] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_ENTER])
		{
			menu_proc_frame_reset(0, NULL);
			t3f_key[ALLEGRO_KEY_PAD_ENTER] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PGUP])
		{
			menu_proc_frame_previous_bitmap(0, NULL);
			t3f_key[ALLEGRO_KEY_PGUP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PGDN])
		{
			menu_proc_frame_next_bitmap(0, NULL);
			t3f_key[ALLEGRO_KEY_PGDN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_H])
		{
			animation->frame[current_frame]->x = (animation->frame[current_frame]->height - animation->frame[current_frame]->width) / 2;
			t3f_key[ALLEGRO_KEY_H] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_F])
		{
			animation->frame[current_frame]->flags = 0;
			t3f_key[ALLEGRO_KEY_F] = 0;
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

void logic(void * data)
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
	if(animation->bitmaps->count && current_bitmap < animation->bitmaps->count)
	{
		al_draw_scaled_bitmap(animation->bitmaps->bitmap[current_bitmap], 0, 0, al_get_bitmap_width(animation->bitmaps->bitmap[current_bitmap]), al_get_bitmap_height(animation->bitmaps->bitmap[current_bitmap]), 32, 32, (float)al_get_bitmap_width(animation->bitmaps->bitmap[current_bitmap]) * zoom, (float)al_get_bitmap_height(animation->bitmaps->bitmap[current_bitmap]) * zoom, 0);
	}
}

void frame_render(void)
{
	T3F_ANIMATION_FRAME * fp;
	if(animation->frames > 0 && current_frame < animation->frames)
	{
		al_draw_textf(font, t3f_color_white, 0, 360, 0, "frame[%d].ticks = %d", current_frame, animation->frame[current_frame]->ticks);
		al_draw_line(32, 32, 64, 32, al_map_rgba_f(0.0, 1.0, 0.0, 0.5), 1.0);
		al_draw_line(32, 32, 32, 64, al_map_rgba_f(0.0, 1.0, 0.0, 0.5), 1.0);
		fp = animation->frame[current_frame];
		t3f_draw_scaled_bitmap(animation->bitmaps->bitmap[fp->bitmap], t3f_color_white, 32 + fp->x * zoom, 32 + fp->y * zoom, fp->z, fp->width * zoom, fp->height * zoom, 0);
	}
	t3f_draw_scaled_animation(animation, t3f_color_white, tick, 320 + 32, 32, 0, zoom, 0);
}

void render(void * data)
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

bool setup_menus(void)
{
	file_menu = al_create_menu();
	if(!file_menu)
	{
		return false;
	}
	t3f_add_menu_item(file_menu, "&New", 0, NULL, menu_proc_file_new, NULL);
	t3f_add_menu_item(file_menu, "&Load", 0, NULL, menu_proc_file_load, NULL);
	t3f_add_menu_item(file_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(file_menu, "&Save", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_file_save, menu_update_proc);
	t3f_add_menu_item(file_menu, "Save &As", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_file_save_as, menu_update_proc);
	#ifndef ALLEGRO_MACOSX
		t3f_add_menu_item(file_menu, NULL, 0, NULL, NULL, NULL);
		t3f_add_menu_item(file_menu, "&Quit", 0, NULL, menu_proc_file_quit, NULL);
	#endif

	view_menu = al_create_menu();
	if(!view_menu)
	{
		return false;
	}
	t3f_add_menu_item(view_menu, "&Bitmaps", ALLEGRO_MENU_ITEM_CHECKBOX | ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_view_bitmaps, menu_update_mode_bitmaps_proc);
	t3f_add_menu_item(view_menu, "&Frames", ALLEGRO_MENU_ITEM_CHECKBOX | ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_view_frames, menu_update_mode_frames_proc);

	bitmap_menu = al_create_menu();
	if(!bitmap_menu)
	{
		return false;
	}
	t3f_add_menu_item(bitmap_menu, "&Add", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_bitmap_add, menu_update_bitmap_proc);
	t3f_add_menu_item(bitmap_menu, "&Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_bitmap_delete, menu_update_bitmap_delete_proc);
	t3f_add_menu_item(bitmap_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(bitmap_menu, "&Previous", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_bitmap_previous, menu_update_bitmap_iter_proc);
	t3f_add_menu_item(bitmap_menu, "&Next", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_bitmap_next, menu_update_bitmap_iter_proc);
	t3f_add_menu_item(bitmap_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(bitmap_menu, "&Load", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_bitmap_load, menu_update_bitmap_delete_proc);

	frame_menu = al_create_menu();
	if(!frame_menu)
	{
		return false;
	}
	t3f_add_menu_item(frame_menu, "&Add", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_add, menu_update_frame_proc);
	t3f_add_menu_item(frame_menu, "&Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_delete, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(frame_menu, "&Previous Bitmap", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_previous_bitmap, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, "&Next Bitmap", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_next_bitmap, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(frame_menu, "D&ouble Size", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_double, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, "&Half Size", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_half, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, "&Reset Size", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_reset, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(frame_menu, "&Lengthen", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_lengthen, menu_update_frame_delete_proc);
	t3f_add_menu_item(frame_menu, "Shor&ten", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_shorten, menu_update_frame_tick_proc);
	t3f_add_menu_item(frame_menu, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(frame_menu, "Pr&evious", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_previous, menu_update_frame_iter_proc);
	t3f_add_menu_item(frame_menu, "Ne&xt", ALLEGRO_MENU_ITEM_DISABLED, NULL, menu_proc_frame_next, menu_update_frame_iter_proc);

	main_menu = al_create_menu();
	if(!main_menu)
	{
		return false;
	}
	t3f_add_menu_item(main_menu, "File", 0, file_menu, NULL, NULL);
	t3f_add_menu_item(main_menu, "View", 0, view_menu, NULL, NULL);
	t3f_add_menu_item(main_menu, "Bitmap", 0, bitmap_menu, NULL, NULL);
	t3f_add_menu_item(main_menu, "Frame", 0, frame_menu, NULL, NULL);
	return true;
}

bool initialize(void)
{
	if(!t3f_initialize("T3F Animator", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_MENU, NULL))
	{
		printf("Failed to initialize T3F.\n");
		return false;
	}
	read_config();
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
		if(animation)
		{
			strcpy(last_animation_filename, animation_fn);
			update_config();
		}
	}
	if(!setup_menus())
	{
		return false;
	}
	t3f_attach_menu(main_menu);
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
