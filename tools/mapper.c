#include "t3f/t3f.h"
#include "t3f/tilemap.h"
#include <allegro5/allegro_native_dialog.h>

#define MAPPER_MAX_SPRITES  256

#define MAPPER_VIEW_TILESET   0
#define MAPPER_VIEW_TILEMAP   1
#define MAPPER_VIEW_TILE_ANI  2

typedef struct
{
	
	float x, y, z;
	
} MAPPER_CAMERA;

/* main data */
ALLEGRO_FONT * mapper_font = NULL;
T3F_TILESET * mapper_tileset = NULL;
T3F_TILEMAP * mapper_tilemap = NULL;
char mapper_filename[1024] = {0};
char mapper_last_level_filename[1024] = {0};
char mapper_last_filename[1024] = {0};
int mapper_view = MAPPER_VIEW_TILESET;
unsigned long mapper_tick = 0;
int mapper_bg_color = 0;

/* tileset data */
int mapper_current_tile = 0;
int mapper_hover_tile = 0;
int mapper_tile_width = 32;
int mapper_tile_height = 32;
int mapper_current_frame = 0;

/* tilemap data */
MAPPER_CAMERA mapper_camera;
int mapper_tilemap_layer_visible[16] = {true, true, true, true, true};
int mapper_tilemap_hover_x = 0;
int mapper_tilemap_hover_y = 0;
bool mapper_tilemap_hover_fail = false;
int mapper_current_layer = 0;
int mapper_map_width = 100;
int mapper_map_height = 100;
int mapper_map_layers = 1;

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
		strcpy(mapper_filename, rpath);
	}
	al_destroy_native_file_dialog(file_selector);
	al_start_timer(t3f_timer);
	if(rpath)
	{
		return mapper_filename;
	}
	return NULL;
}

const char * mapper_get_extension(const char * fn)
{
	int i;
	
	for(i = strlen(fn); i >= 0; i--)
	{
		if(fn[i] == '.')
		{
			return &fn[i + 1];
		}
	}
	return NULL;
}

void mapper_replace_tiles(ALLEGRO_BITMAP * bp)
{
	int i, j;
	T3F_ANIMATION * ap = NULL;
	ALLEGRO_BITMAP * newbp = NULL;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int current_tile = mapper_current_tile;
	
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	for(i = 0; i < al_get_bitmap_height(bp) / mapper_tile_height; i++)
	{
		for(j = 0; j < al_get_bitmap_width(bp) / mapper_tile_width; j++)
		{
			if(current_tile < mapper_tileset->tiles)
			{
				ap = t3f_create_animation();
				if(ap)
				{
					newbp = al_create_bitmap(mapper_tile_width, mapper_tile_height);
					if(newbp)
					{
						al_set_target_bitmap(newbp);
						al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
						al_draw_bitmap(bp, -j * mapper_tile_width, -i * mapper_tile_height, 0);
						t3f_animation_add_bitmap(ap, newbp);
						t3f_animation_add_frame(ap, 0, 0, 0, 0, mapper_tile_width, mapper_tile_height, 0, 1);
						t3f_destroy_animation(mapper_tileset->tile[current_tile]->ap);
						mapper_tileset->tile[current_tile]->ap = ap;
					}
				}
				current_tile++;
			}
			else
			{
				ap = t3f_create_animation();
				if(ap)
				{
					newbp = al_create_bitmap(mapper_tile_width, mapper_tile_height);
					if(newbp)
					{
						al_set_target_bitmap(newbp);
						al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
						al_draw_bitmap(bp, -j * mapper_tile_width, -i * mapper_tile_height, 0);
						t3f_animation_add_bitmap(ap, newbp);
						t3f_animation_add_frame(ap, 0, 0, 0, 0, mapper_tile_width, mapper_tile_height, 0, 1);
						t3f_add_tile(mapper_tileset, ap);
					}
				}
			}
		}
	}
	al_restore_state(&old_state);
}

void mapper_import_tiles(ALLEGRO_BITMAP * bp)
{
	int i, j;
	T3F_ANIMATION * ap = NULL;
	ALLEGRO_BITMAP * newbp = NULL;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	for(i = 0; i < al_get_bitmap_height(bp) / mapper_tile_height; i++)
	{
		for(j = 0; j < al_get_bitmap_width(bp) / mapper_tile_width; j++)
		{
			ap = t3f_create_animation();
			if(ap)
			{
				newbp = al_create_bitmap(mapper_tile_width, mapper_tile_height);
				if(newbp)
				{
					al_set_target_bitmap(newbp);
					al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
					al_draw_bitmap(bp, -j * mapper_tile_width, -i * mapper_tile_height, 0);
					t3f_animation_add_bitmap(ap, newbp);
					t3f_animation_add_frame(ap, 0, 0, 0, 0, mapper_tile_width, mapper_tile_height, 0, 1);
					t3f_add_tile(mapper_tileset, ap);
				}
			}
		}
	}
	al_restore_state(&old_state);
}

void mapper_global_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_TAB])
	{
		mapper_view++;
		if(mapper_view > 1)
		{
			mapper_view = 0;
		}
		t3f_key[ALLEGRO_KEY_TAB] = 0;
	}
}

void mapper_tilemap_replace_tile(int tile, int new_tile)
{
	int i, j, k;
	
	/* replace tiles in tile animations */
	for(i = 0; i < mapper_tileset->tiles; i++)
	{
		for(j = 0; j < mapper_tileset->tile[i]->frame_list_total; j++)
		{
			if(mapper_tileset->tile[i]->frame_list[j] == tile)
			{
				mapper_tileset->tile[i]->frame_list[j] = new_tile;
			}
		}
	}
	
	/* replace tiles in tilemap */
	if(mapper_tilemap)
	{
		for(i = 0; i < mapper_tilemap->layers; i++)
		{
			for(j = 0; j < mapper_tilemap->layer[i]->height; j++)
			{
				for(k = 0; k < mapper_tilemap->layer[i]->width; k++)
				{
					if(mapper_tilemap->layer[i]->data[j][k] == tile)
					{
						mapper_tilemap->layer[i]->data[j][k] = new_tile;
					}
				}
			}
		}
	}
}

void mapper_tileset_logic(void)
{
	const char * fn = NULL;
	const char * ext = NULL;
	T3F_ANIMATION * ap = NULL;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_PATH * temp_path = NULL;
	int i;
	
	if(t3f_key[ALLEGRO_KEY_F3])
	{
		fn = select_file(mapper_last_level_filename, "Open Tileset", "*.*;*.t3t", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(fn)
		{
			if(mapper_tileset)
			{
				t3f_destroy_tileset(mapper_tileset);
			}
			mapper_tileset = t3f_load_tileset(fn);
			strcpy(mapper_last_level_filename, fn);
		}
		t3f_key[ALLEGRO_KEY_F3] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_F4])
	{
		if(mapper_tileset)
		{
			t3f_destroy_tileset(mapper_tileset);
		}
		mapper_tileset = t3f_create_tileset(mapper_tile_width, mapper_tile_height);
		t3f_key[ALLEGRO_KEY_F4] = 0;
	}
	if(mapper_tileset)
	{
		if(t3f_key[ALLEGRO_KEY_F2])
		{
			fn = select_file(mapper_last_level_filename, "Save Tileset", "*.*;*.t3t", ALLEGRO_FILECHOOSER_SAVE);
			if(fn)
			{
				temp_path = al_create_path(fn);
				if(temp_path)
				{
					al_set_path_extension(temp_path, ".t3t");
					t3f_save_tileset(mapper_tileset, al_path_cstr(temp_path, '/'));
					strcpy(mapper_last_level_filename, al_path_cstr(temp_path, '/'));
					al_destroy_path(temp_path);
				}
			}
			t3f_key[ALLEGRO_KEY_F2] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_INSERT])
		{
			fn = select_file(mapper_last_filename, "Open Animation", "*.*;*.ani;*.pcx;*.png;*.tga;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
			if(fn)
			{
				ext = mapper_get_extension(fn);
				if(!strcmp(ext, "ani"))
				{
					ap = t3f_load_animation(fn);
					if(ap)
					{
						t3f_add_tile(mapper_tileset, ap);
						mapper_current_tile = mapper_tileset->tiles - 1;
					}
				}
				else
				{
					bp = al_load_bitmap(fn);
					if(bp)
					{
						mapper_import_tiles(bp);
					}
					al_destroy_bitmap(bp);
				}
				strcpy(mapper_last_filename, fn);
			}
			t3f_key[ALLEGRO_KEY_INSERT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_ENTER])
		{
			fn = select_file(mapper_last_filename, "Open Animation", "*.*;*.ani;*.pcx;*.png;*.tga;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
			if(fn)
			{
				ext = mapper_get_extension(fn);
				if(!strcmp(ext, "t3a"))
				{
					ap = t3f_load_animation(fn);
					if(ap)
					{
						t3f_destroy_animation(mapper_tileset->tile[mapper_current_tile]->ap);
						mapper_tileset->tile[mapper_current_tile]->ap = ap;
					}
				}
				else
				{
					bp = al_load_bitmap(fn);
					if(bp)
					{
						mapper_replace_tiles(bp);
					}
					al_destroy_bitmap(bp);
				}
				strcpy(mapper_last_filename, fn);
			}
			t3f_key[ALLEGRO_KEY_ENTER] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_DELETE])
		{
			if(mapper_current_tile < mapper_tileset->tiles)
			{
				t3f_destroy_tile(mapper_tileset->tile[mapper_current_tile]);
				mapper_tilemap_replace_tile(mapper_current_tile, 0);
				for(i = mapper_current_tile; i < mapper_tileset->tiles - 1; i++)
				{
					mapper_tileset->tile[i] = mapper_tileset->tile[i + 1];
					mapper_tilemap_replace_tile(i + 1, i);
				}
				mapper_tileset->tiles--;
			}
			t3f_key[ALLEGRO_KEY_DELETE] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_A])
		{
			mapper_view = MAPPER_VIEW_TILE_ANI;
			t3f_key[ALLEGRO_KEY_A] = 0;
		}
		mapper_hover_tile = (t3f_mouse_y / mapper_tile_height) * (t3f_virtual_display_width / mapper_tile_width) + (t3f_mouse_x / mapper_tile_width) % (t3f_virtual_display_width / mapper_tile_width);
		if(t3f_mouse_button[0] && mapper_hover_tile < mapper_tileset->tiles)
		{
			mapper_current_tile = mapper_hover_tile;
		}
	}
}

void mapper_tile_ani_logic(void)
{
	int i, hover_frame;
	
	if(t3f_key[ALLEGRO_KEY_INSERT])
	{
		if(!(mapper_tileset->tile[mapper_current_tile]->flags & T3F_TILE_FLAG_ANIMATED))
		{
			mapper_tileset->tile[mapper_current_tile]->frame_list_total = 0;
		}
		mapper_tileset->tile[mapper_current_tile]->flags |= T3F_TILE_FLAG_ANIMATED;
		mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_tileset->tile[mapper_current_tile]->frame_list_total] = mapper_current_tile;
		mapper_tileset->tile[mapper_current_tile]->frame_list_total++;
		mapper_current_frame = mapper_tileset->tile[mapper_current_tile]->frame_list_total - 1;
		t3f_key[ALLEGRO_KEY_INSERT] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE] && (mapper_tileset->tile[mapper_current_tile]->flags & T3F_TILE_FLAG_ANIMATED))
	{
		for(i = mapper_current_frame; i < mapper_tileset->tile[mapper_current_tile]->frame_list_total - 1; i++)
		{
			mapper_tileset->tile[mapper_current_tile]->frame_list[i] = mapper_tileset->tile[mapper_current_tile]->frame_list[i + 1];
		}
		mapper_tileset->tile[mapper_current_tile]->frame_list_total--;
		if(mapper_current_frame >= mapper_tileset->tile[mapper_current_tile]->frame_list_total)
		{
			mapper_current_frame = mapper_tileset->tile[mapper_current_tile]->frame_list_total - 1;
		}
		if(mapper_tileset->tile[mapper_current_tile]->frame_list_total < 1)
		{
			mapper_tileset->tile[mapper_current_tile]->flags ^= T3F_TILE_FLAG_ANIMATED;
		}
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_A])
	{
		mapper_view = MAPPER_VIEW_TILESET;
		t3f_key[ALLEGRO_KEY_A] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_MINUS])
	{
		mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_current_frame]--;
		if(mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_current_frame] < 0)
		{
			mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_current_frame] = mapper_tileset->tiles - 1;
		}
		t3f_key[ALLEGRO_KEY_MINUS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_EQUALS])
	{
		mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_current_frame]++;
		if(mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_current_frame] >= mapper_tileset->tiles)
		{
			mapper_tileset->tile[mapper_current_tile]->frame_list[mapper_current_frame] = 0;
		}
		t3f_key[ALLEGRO_KEY_EQUALS] = 0;
	}
	hover_frame = (t3f_mouse_y / mapper_tile_height) * (t3f_virtual_display_width / mapper_tile_width) + (t3f_mouse_x / mapper_tile_width) % (t3f_virtual_display_width / mapper_tile_width);
	if(t3f_mouse_button[0] && hover_frame < mapper_tileset->tile[mapper_current_tile]->frame_list_total)
	{
		mapper_current_frame = hover_frame;
	}
}

float mapper_get_speed(float z)
{
	return t3f_project_x(1.0, z) - t3f_project_x(0.0, z);
}

float mapper_get_scale(float z)
{
	return 1.0 / mapper_get_speed(z);
}

float mapper_get_offset_x(float z)
{
	return -t3f_project_x(0.0, z) / mapper_get_speed(z);
}

float mapper_get_offset_y(float z)
{
	return -t3f_project_y(0.0, z) / mapper_get_speed(z);
}

void mapper_tilemap_insert_row(int layer, int row)
{
	int i, j;
	
	for(i = mapper_tilemap->layer[layer]->height - 1; i > row; i--)
	{
		for(j = 0; j < mapper_tilemap->layer[layer]->width; j++)
		{
			mapper_tilemap->layer[layer]->data[i][j] = mapper_tilemap->layer[layer]->data[i - 1][j];
		}
	}
}

void mapper_tilemap_insert_column(int layer, int column)
{
	int i, j;
	
	for(i = mapper_tilemap->layer[layer]->width - 1; i > column; i--)
	{
		for(j = 0; j < mapper_tilemap->layer[layer]->height; j++)
		{
			mapper_tilemap->layer[layer]->data[j][i] = mapper_tilemap->layer[layer]->data[j][i - 1];
		}
	}
}

void mapper_tilemap_delete_row(int layer, int row)
{
	int i, j;
	
	for(i = row + 1; i < mapper_tilemap->layer[layer]->height - 1; i++)
	{
		for(j = 0; j < mapper_tilemap->layer[layer]->width; j++)
		{
			mapper_tilemap->layer[layer]->data[i][j] = mapper_tilemap->layer[layer]->data[i + 1][j];
		}
	}
}

void mapper_tilemap_delete_column(int layer, int column)
{
	int i, j;
	
	for(i = column; i < mapper_tilemap->layer[layer]->width - 1; i++)
	{
		for(j = 0; j < mapper_tilemap->layer[layer]->height; j++)
		{
			mapper_tilemap->layer[layer]->data[j][i] = mapper_tilemap->layer[layer]->data[j][i + 1];
		}
	}
}

void mapper_tilemap_move_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_LEFT])
	{
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			mapper_camera.x -= mapper_tileset->width;
			mapper_camera.x = (int)(mapper_camera.x / mapper_tileset->width) * mapper_tileset->width;
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		else
		{
			mapper_camera.x -= 1.0;
		}
	}
	if(t3f_key[ALLEGRO_KEY_RIGHT])
	{
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			mapper_camera.x += mapper_tileset->width;
			mapper_camera.x = (int)(mapper_camera.x / mapper_tileset->width) * mapper_tileset->width;
			t3f_key[ALLEGRO_KEY_RIGHT] = 0;
		}
		else
		{
			mapper_camera.x += 1.0;
		}
	}
	if(t3f_key[ALLEGRO_KEY_UP])
	{
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			mapper_camera.y -= mapper_tileset->height;
			mapper_camera.y = (int)(mapper_camera.y / mapper_tileset->height) * mapper_tileset->height;
			t3f_key[ALLEGRO_KEY_UP] = 0;
		}
		else
		{
			mapper_camera.y -= 1.0;
		}
	}
	if(t3f_key[ALLEGRO_KEY_DOWN])
	{
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			mapper_camera.y += mapper_tileset->height;
			mapper_camera.y = (int)(mapper_camera.y / mapper_tileset->height) * mapper_tileset->height;
			t3f_key[ALLEGRO_KEY_DOWN] = 0;
		}
		else
		{
			mapper_camera.y += 1.0;
		}
	}
}

void mapper_tilemap_logic(void)
{
	int i, j;
	T3F_TILEMAP_LAYER * new_layer = NULL;
	ALLEGRO_PATH * temp_path = NULL;
	const char * fn = NULL;
	int mx, my;
	
	if(t3f_key[ALLEGRO_KEY_F3])
	{
		fn = select_file(mapper_last_level_filename, "Open Tilemap", "*.*;*.t3m", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(fn)
		{
			if(mapper_tilemap)
			{
				t3f_destroy_tilemap(mapper_tilemap);
			}
			mapper_tilemap = t3f_load_tilemap(fn);
			strcpy(mapper_last_level_filename, fn);
		}
		t3f_key[ALLEGRO_KEY_F3] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_F4])
	{
		if(mapper_tilemap)
		{
			t3f_destroy_tilemap(mapper_tilemap);
		}
		mapper_tilemap = t3f_create_tilemap(mapper_map_width, mapper_map_height, mapper_map_layers);
		t3f_key[ALLEGRO_KEY_F4] = 0;
	}
	if(mapper_tilemap)
	{
		if(t3f_key[ALLEGRO_KEY_F2])
		{
			fn = select_file(mapper_last_level_filename, "Save Tilemap", "*.*;*.t3m", ALLEGRO_FILECHOOSER_SAVE);
			if(fn)
			{
				temp_path = al_create_path(fn);
				if(temp_path)
				{
					al_set_path_extension(temp_path, ".t3m");
					t3f_save_tilemap(mapper_tilemap, al_path_cstr(temp_path, '/'));
					strcpy(mapper_last_level_filename, al_path_cstr(temp_path, '/'));
					al_destroy_path(temp_path);
				}
			}
			t3f_key[ALLEGRO_KEY_F2] = 0;
		}
		mapper_tilemap_move_logic();
		if(t3f_key[ALLEGRO_KEY_MINUS])
		{
			mapper_current_tile--;
			if(mapper_current_tile < 0)
			{
				mapper_current_tile = mapper_tileset->tiles - 1;
			}
			t3f_key[ALLEGRO_KEY_MINUS] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_EQUALS])
		{
			mapper_current_tile++;
			if(mapper_current_tile >= mapper_tileset->tiles)
			{
				mapper_current_tile = 0;
			}
			t3f_key[ALLEGRO_KEY_EQUALS] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PGUP])
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL] && mapper_current_layer < mapper_tilemap->layers - 1)
			{
				new_layer = mapper_tilemap->layer[mapper_current_layer + 1];
				mapper_tilemap->layer[mapper_current_layer + 1] = mapper_tilemap->layer[mapper_current_layer];
				mapper_tilemap->layer[mapper_current_layer] = new_layer;
			}
			mapper_current_layer++;
			if(mapper_current_layer >= mapper_tilemap->layers)
			{
				mapper_current_layer = 0;
			}
			t3f_key[ALLEGRO_KEY_PGUP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PGDN])
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL] && mapper_current_layer > 0)
			{
				new_layer = mapper_tilemap->layer[mapper_current_layer - 1];
				mapper_tilemap->layer[mapper_current_layer - 1] = mapper_tilemap->layer[mapper_current_layer];
				mapper_tilemap->layer[mapper_current_layer] = new_layer;
			}
			mapper_current_layer--;
			if(mapper_current_layer < 0)
			{
				mapper_current_layer = mapper_tilemap->layers - 1;
			}
			t3f_key[ALLEGRO_KEY_PGDN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_MINUS])
		{
			mapper_tilemap->layer[mapper_current_layer]->z -= 1.0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_PLUS])
		{
			mapper_tilemap->layer[mapper_current_layer]->z += 1.0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_ASTERISK])
		{
			
			if(t3f_key[ALLEGRO_KEY_LCTRL])
			{
				/* reset the scale and offset of the current layer */
				if(t3f_key[ALLEGRO_KEY_LSHIFT])
				{
					mapper_tilemap->layer[mapper_current_layer]->scale = 1.0;
					mapper_tilemap->layer[mapper_current_layer]->x = 0.0;
					mapper_tilemap->layer[mapper_current_layer]->y = 0.0;
				}
				/* scale the current layer so that the tiles are their actual size at the layer's current z-coordinate */
				else
				{
					mapper_tilemap->layer[mapper_current_layer]->scale = mapper_get_scale(mapper_tilemap->layer[mapper_current_layer]->z);
					mapper_tilemap->layer[mapper_current_layer]->x = mapper_get_offset_x(mapper_tilemap->layer[mapper_current_layer]->z);
					mapper_tilemap->layer[mapper_current_layer]->y = mapper_get_offset_y(mapper_tilemap->layer[mapper_current_layer]->z);
				}
			}
			else
			{
				mapper_tilemap->layer[mapper_current_layer]->z = 0.0;
			}
			t3f_key[ALLEGRO_KEY_PAD_ASTERISK] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_S])
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL])
			{
				mapper_tilemap->layer[mapper_current_layer]->flags ^= T3F_TILEMAP_LAYER_STATIC;
			}
			else
			{
				mapper_tilemap->layer[mapper_current_layer]->flags ^= T3F_TILEMAP_LAYER_SOLID;
			}
			t3f_key[ALLEGRO_KEY_S] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_INSERT])
		{
			if(mapper_tilemap->layers < 32)
			{
				new_layer = t3f_create_tilemap_layer(mapper_tilemap->layer[mapper_current_layer]->width, mapper_tilemap->layer[mapper_current_layer]->height);
				if(new_layer)
				{
					new_layer->x = mapper_tilemap->layer[mapper_current_layer]->x;
					new_layer->y = mapper_tilemap->layer[mapper_current_layer]->y;
					new_layer->z = mapper_tilemap->layer[mapper_current_layer]->z;
					new_layer->scale = mapper_tilemap->layer[mapper_current_layer]->scale;
					new_layer->speed_x = mapper_tilemap->layer[mapper_current_layer]->speed_x;
					new_layer->speed_y = mapper_tilemap->layer[mapper_current_layer]->speed_y;
					if(t3f_key[ALLEGRO_KEY_LCTRL])
					{
						for(i = 0; i < new_layer->height; i++)
						{
							for(j = 0; j < new_layer->width; j++)
							{
								new_layer->data[i][j] = mapper_tilemap->layer[mapper_current_layer]->data[i][j];
							}
						}
					}
					for(i = mapper_tilemap->layers; i > mapper_current_layer; i--)
					{
						mapper_tilemap->layer[i] = mapper_tilemap->layer[i - 1];
					}
					mapper_tilemap->layer[mapper_current_layer] = new_layer;
					mapper_tilemap->layers++;
				}
			}
			t3f_key[ALLEGRO_KEY_INSERT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_DELETE])
		{
			if(mapper_tilemap->layers > 1)
			{
				t3f_destroy_tilemap_layer(mapper_tilemap->layer[mapper_current_layer]);
				for(i = mapper_current_layer; i < mapper_tilemap->layers - 1; i++)
				{
					mapper_tilemap->layer[i] = mapper_tilemap->layer[i + 1];
				}
				mapper_tilemap->layers--;
				if(mapper_current_layer >= mapper_tilemap->layers)
				{
					mapper_current_layer = mapper_tilemap->layers - 1;
				}
			}
			t3f_key[ALLEGRO_KEY_DELETE] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_1])
		{
			mapper_tilemap->layer[mapper_current_layer]->speed_x = 1.0;
			mapper_tilemap->layer[mapper_current_layer]->speed_y = 1.0;
			t3f_key[ALLEGRO_KEY_1] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_C])
		{
			mapper_bg_color = 1 - mapper_bg_color;
			t3f_key[ALLEGRO_KEY_C] = 0;
		}
		mapper_tilemap_hover_x = (int)(t3f_mouse_x + mapper_camera.x - mapper_tilemap->layer[mapper_current_layer]->x) / (mapper_tileset->width * mapper_tilemap->layer[mapper_current_layer]->scale);
		mapper_tilemap_hover_y = (int)(t3f_mouse_y + mapper_camera.y - mapper_tilemap->layer[mapper_current_layer]->y) / (mapper_tileset->height * mapper_tilemap->layer[mapper_current_layer]->scale);
		if(t3f_key[ALLEGRO_KEY_H])
		{
			if(t3f_key[ALLEGRO_KEY_LSHIFT])
			{
				mapper_tilemap_delete_row(mapper_current_layer, mapper_tilemap_hover_y);
			}
			else
			{
				mapper_tilemap_insert_row(mapper_current_layer, mapper_tilemap_hover_y);
			}
			t3f_key[ALLEGRO_KEY_H] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_V])
		{
			if(t3f_key[ALLEGRO_KEY_LSHIFT])
			{
				mapper_tilemap_delete_column(mapper_current_layer, mapper_tilemap_hover_x);
			}
			else
			{
				mapper_tilemap_insert_column(mapper_current_layer, mapper_tilemap_hover_x);
			}
			t3f_key[ALLEGRO_KEY_V] = 0;
		}
		t3f_get_mouse_mickeys(&mx, &my, NULL);
		if(mapper_tilemap_hover_x < 0 || mapper_tilemap_hover_x >= mapper_tilemap->layer[mapper_current_layer]->width || mapper_tilemap_hover_y < 0 || mapper_tilemap_hover_y >= mapper_tilemap->layer[mapper_current_layer]->height)
		{
			mapper_tilemap_hover_fail = true;
		}
		else
		{
			mapper_tilemap_hover_fail = false;
			if(t3f_mouse_button[0])
			{
				if(t3f_key[ALLEGRO_KEY_LCTRL])
				{
					mapper_tilemap->layer[mapper_current_layer]->x -= mx;
					mapper_tilemap->layer[mapper_current_layer]->y -= my;
				}
				else if(t3f_key[ALLEGRO_KEY_LSHIFT])
				{
					for(i = 0; i < mapper_tilemap->layer[mapper_current_layer]->height; i++)
					{
						for(j = 0; j < mapper_tilemap->layer[mapper_current_layer]->width; j++)
						{
							mapper_tilemap->layer[mapper_current_layer]->data[j][i] = mapper_current_tile;
						}
					}
				}
				else
				{
					mapper_tilemap->layer[mapper_current_layer]->data[mapper_tilemap_hover_y][mapper_tilemap_hover_x] = mapper_current_tile;
				}
			}
		}
		mapper_camera.z = mapper_tilemap->layer[mapper_current_layer]->z;
	}
}

void mapper_logic(void)
{
	mapper_global_logic();
	switch(mapper_view)
	{
		case MAPPER_VIEW_TILESET:
		{
			mapper_tileset_logic();
			break;
		}
		case MAPPER_VIEW_TILEMAP:
		{
			mapper_tilemap_logic();
			break;
		}
		case MAPPER_VIEW_TILE_ANI:
		{
			mapper_tile_ani_logic();
			break;
		}
	}
	mapper_tick++;
}

void mapper_tileset_render(void)
{
	int i;
	int prow;
	int row, col;
	
	if(mapper_tileset)
	{
		prow = (t3f_virtual_display_width / mapper_tileset->width);
		for(i = 0; i < mapper_tileset->tiles; i++)
		{
			row = i % prow;
			col = i / prow;
			t3f_draw_animation(mapper_tileset->tile[i]->ap, t3f_color_white, mapper_tick, row * mapper_tileset->width, col * mapper_tileset->height, 0, 0);
			if(i == mapper_hover_tile)
			{
				al_hold_bitmap_drawing(false);
				al_draw_rectangle(row * mapper_tileset->width + 0.5, col * mapper_tileset->height + 0.5, row * mapper_tileset->width + mapper_tileset->width - 1 + 0.5, col * mapper_tileset->height + mapper_tileset->height - 1 + 0.5, al_map_rgba_f(0.0, 1.0, 0.0, 0.5), 1.0);
				al_hold_bitmap_drawing(true);
			}
			if(i == mapper_tileset->tiles - 1)
			{
				al_hold_bitmap_drawing(false);
				al_draw_rectangle(row * mapper_tileset->width + 0.5, col * mapper_tileset->height + 0.5, row * mapper_tileset->width + mapper_tileset->width - 1 + 0.5, col * mapper_tileset->height + mapper_tileset->height - 1 + 0.5, al_map_rgba_f(1.0, 0.0, 0.0, 0.5), 1.0);
				al_hold_bitmap_drawing(true);
			}
			if(i == mapper_current_tile)
			{
				al_hold_bitmap_drawing(false);
				al_draw_rectangle(row * mapper_tileset->width + 0.5, col * mapper_tileset->height + 0.5, row * mapper_tileset->width + mapper_tileset->width - 1 + 0.5, col * mapper_tileset->height + mapper_tileset->height - 1 + 0.5, al_map_rgba_f(1.0, 1.0, 1.0, 0.5), 1.0);
				al_hold_bitmap_drawing(true);
			}
		}
	}
	else
	{
		al_draw_textf(mapper_font, t3f_color_white, 0, 0, 0, "Please create or load a tileset.");
	}
}

void mapper_tile_ani_render(void)
{
	int i;
	float x, y;
	bool held = al_is_bitmap_drawing_held();
	
	if(mapper_tileset->tile[mapper_current_tile]->flags & T3F_TILE_FLAG_ANIMATED)
	{
		for(i = 0; i < mapper_tileset->tile[mapper_current_tile]->frame_list_total; i++)
		{
			x = (i % 20) * 32;
			y = (i / 20) * 32;
			t3f_draw_animation(mapper_tileset->tile[mapper_tileset->tile[mapper_current_tile]->frame_list[i]]->ap, t3f_color_white, mapper_tick, x, y, 0, 0);
			if(i == mapper_current_frame)
			{
				al_hold_bitmap_drawing(true);
				al_draw_rectangle(x + 0.5, y + 0.5, x + 31 + 0.5, y + 31 + 0.5, al_map_rgba_f(0.0, 1.0, 0.0, 1.0), 1.0);
				al_hold_bitmap_drawing(held);
			}
		}
	}
	else
	{
		t3f_draw_animation(mapper_tileset->tile[mapper_current_tile]->ap, t3f_color_white, mapper_tick, 0, 0, 0, 0);
	}
}

void mapper_tilemap_render(void)
{
	int i;
	float lx, ty, scale;
	
	if(mapper_tilemap)
	{
		for(i = 0; i < mapper_tilemap->layers; i++)
		{
			if(!t3f_key[ALLEGRO_KEY_L] || i == mapper_current_layer)
			{
				t3f_render_tilemap(mapper_tilemap, mapper_tileset, i, mapper_tick, mapper_camera.x, mapper_camera.y, mapper_camera.z, t3f_color_white);
			}
		}
		al_hold_bitmap_drawing(false);
		if(!mapper_tilemap_hover_fail)
		{
			scale = mapper_tilemap->layer[mapper_current_layer]->scale;
			lx = mapper_tilemap_hover_x * mapper_tileset->width * scale + mapper_tilemap->layer[mapper_current_layer]->x - mapper_camera.x + 0.5;
			ty = mapper_tilemap_hover_y * mapper_tileset->height * scale + mapper_tilemap->layer[mapper_current_layer]->y - mapper_camera.y + 0.5;
			al_draw_rectangle(lx, ty, lx + (float)mapper_tileset->width * scale - 1.0 + 0.5, ty + (float)mapper_tileset->height * scale - 1.0 + 0.5, al_map_rgba_f(0.0, 1.0, 0.0, 0.5), 1.0);
		}
		t3f_draw_animation(mapper_tileset->tile[mapper_current_tile]->ap, al_map_rgba_f(1.0, 1.0, 1.0, 0.5), mapper_tick, t3f_mouse_x + 16, t3f_mouse_y + 16, 0, 0);
		al_draw_textf(mapper_font, t3f_color_white, 0, 416, 0, "Layer Position = (%4.2f, %4.2f, %4.2f)", mapper_tilemap->layer[mapper_current_layer]->x, mapper_tilemap->layer[mapper_current_layer]->y, mapper_tilemap->layer[mapper_current_layer]->z);
		al_draw_textf(mapper_font, t3f_color_white, 0, 432, 0, "Camera Position = (%4.2f, %4.2f, %4.2f)", mapper_camera.x, mapper_camera.y, mapper_camera.z);
		al_draw_textf(mapper_font, t3f_color_white, 0, 448, 0, "Current Layer = %d", mapper_current_layer);
		al_draw_textf(mapper_font, t3f_color_white, 0, 464, 0, "Current Tile = %d", mapper_current_tile);
	}
	else
	{
		al_draw_textf(mapper_font, t3f_color_white, 0, 0, 0, "Please create or load a tilemap.");
	}
}

void mapper_help_render(void)
{
	al_hold_bitmap_drawing(false);
	al_draw_filled_rectangle(0, 0, t3f_virtual_display_width, t3f_virtual_display_height, al_map_rgba_f(0.0, 0.0, 0.0, 0.75));
	al_hold_bitmap_drawing(true);
	al_draw_textf(mapper_font, t3f_color_white, 0, 0, 0, "Global Keys");
	al_draw_textf(mapper_font, t3f_color_white, 0, 16, 0, "-------------");
	al_draw_textf(mapper_font, t3f_color_white, 0, 32, 0, "F1: Help");
	al_draw_textf(mapper_font, t3f_color_white, 0, 48, 0, "Tab: Change View");

	al_draw_textf(mapper_font, t3f_color_white, 0, 80, 0, "Tileset Keys");
	al_draw_textf(mapper_font, t3f_color_white, 0, 96, 0, "-------------");
	al_draw_textf(mapper_font, t3f_color_white, 0, 112, 0, "F2: Save tileset");
	al_draw_textf(mapper_font, t3f_color_white, 0, 128, 0, "F3: Load tileset");
	al_draw_textf(mapper_font, t3f_color_white, 0, 144, 0, "F4: Create new tileset");
	al_draw_textf(mapper_font, t3f_color_white, 0, 160, 0, "Insert: Add tile");
	al_draw_textf(mapper_font, t3f_color_white, 0, 176, 0, "Delete: Delete tile");
	
	al_draw_textf(mapper_font, t3f_color_white, 0, 208, 0, "Tilemap Keys");
	al_draw_textf(mapper_font, t3f_color_white, 0, 224, 0, "-------------");
	al_draw_textf(mapper_font, t3f_color_white, 0, 240, 0, "F2: Save tilemap");
	al_draw_textf(mapper_font, t3f_color_white, 0, 256, 0, "F3: Load tilemap");
	al_draw_textf(mapper_font, t3f_color_white, 0, 272, 0, "F4: Create new tilemap");
	al_draw_textf(mapper_font, t3f_color_white, 0, 288, 0, "-/=: Select tile");
	al_draw_textf(mapper_font, t3f_color_white, 0, 304, 0, "PGUP/PGDN: Select layer");
	al_draw_textf(mapper_font, t3f_color_white, 0, 320, 0, "Pad +/-: Change Z-position of current layer");
	al_draw_textf(mapper_font, t3f_color_white, 0, 336, 0, "Ctrl+*: Scale current layer so tiles appear 1:1");
	al_draw_textf(mapper_font, t3f_color_white, 0, 352, 0, "Ctrl+Shift+*: Reset layer scale");
	al_draw_textf(mapper_font, t3f_color_white, 0, 368, 0, "*: Reset layer Z-position");
	al_draw_textf(mapper_font, t3f_color_white, 0, 384, 0, "S: Toggle layer solid flag");
	al_draw_textf(mapper_font, t3f_color_white, 0, 400, 0, "Ctrl+S: Toggle layer static flag");
	al_draw_textf(mapper_font, t3f_color_white, 0, 416, 0, "Insert: Insert layer");
	al_draw_textf(mapper_font, t3f_color_white, 0, 432, 0, "Delete: Delete layer");
}

void mapper_render(void)
{
	if(mapper_bg_color == 0)
	{
		al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.1));
	}
	else
	{
		al_clear_to_color(al_map_rgb_f(0.25, 0.25, 1.0));
	}
	al_hold_bitmap_drawing(true);
	switch(mapper_view)
	{
		case MAPPER_VIEW_TILESET:
		{
			mapper_tileset_render();
			break;
		}
		case MAPPER_VIEW_TILEMAP:
		{
			mapper_tilemap_render();
			break;
		}
		case MAPPER_VIEW_TILE_ANI:
		{
			mapper_tile_ani_render();
			break;
		}
	}
	if(t3f_key[ALLEGRO_KEY_F1])
	{
		mapper_help_render();
	}
	al_hold_bitmap_drawing(false);
}

bool mapper_initialize(void)
{
	const char * val = NULL;
	
	if(!t3f_initialize("T3F Mapper", 640, 480, 60.0, mapper_logic, mapper_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE))
	{
		return false;
	}
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	mapper_font = al_load_bitmap_font("fonts/basic_font.png");
	if(!mapper_font)
	{
		return false;
	}
	
	/* load config */
	val = al_get_config_value(t3f_config, "tileset", "width");
	if(val)
	{
		mapper_tile_width = atoi(val);
	}
	val = al_get_config_value(t3f_config, "tileset", "height");
	if(val)
	{
		mapper_tile_height = atoi(val);
	}
	val = al_get_config_value(t3f_config, "tilemap", "width");
	if(val)
	{
		mapper_map_width = atoi(val);
	}
	val = al_get_config_value(t3f_config, "tilemap", "height");
	if(val)
	{
		mapper_map_height = atoi(val);
	}
	val = al_get_config_value(t3f_config, "tilemap", "layers");
	if(val)
	{
		mapper_map_layers = atoi(val);
	}
	
	mapper_camera.x = 0;
	mapper_camera.y = 0;
	mapper_camera.z = 0;
	return true;
}

int main(void)
{
	if(!mapper_initialize())
	{
		return -1;
	}
	t3f_run();
	return 0;
}
