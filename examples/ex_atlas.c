#include "t3f/t3f.h"
#include "t3f/rng.h"

typedef struct
{
	
	float x, y;
	float vx, vy;
	int bitmap;
	
} OBJECT;

ALLEGRO_BITMAP * bitmap[8] = {NULL};
T3F_ATLAS * atlas = NULL;
ALLEGRO_BITMAP * sprite_sheet = NULL;
OBJECT object[1024];
T3F_RNG_STATE rng_state;

void logic(void * data)
{
	int i;
	
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	for(i = 0; i < 1024; i++)
	{
		object[i].x += object[i].vx;
		if(object[i].x > 640 || object[i].x < 0)
		{
			object[i].vx = -object[i].vx;
		}
		object[i].y += object[i].vy;
		if(object[i].y > 480 || object[i].y < 0)
		{
			object[i].vy = -object[i].vy;
		}
	}
}

void render(void * data)
{
	int i;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < 1024; i++)
	{
		al_draw_bitmap(bitmap[object[i].bitmap], object[i].x, object[i].y, 0);
	}
	al_hold_bitmap_drawing(false);
}

int main(int argc, char * argv[])
{
	int i;
	char fn[1024] = {0};
	ALLEGRO_BITMAP * bp;
	
	if(!t3f_initialize("ex_atlas", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE, NULL))
	{
		return 1;
	}
	t3f_srand(&rng_state, time(0));
	
	/* load bitmaps into sprite sheet */
	atlas = t3f_create_atlas(512, 512);
	if(!atlas)
	{
		printf("Failed to create atlas!\n");
		return 0;
	}
	for(i = 0; i < 8; i++)
	{
		sprintf(fn, "data/%d.png", i);
		bp = al_load_bitmap(fn);
		if(!bp)
		{
			return 1;
		}
		bitmap[i] = t3f_add_bitmap_to_atlas(atlas, &bp, T3F_ATLAS_SPRITE);
		if(!bitmap[i])
		{
			return 1;
		}
		al_destroy_bitmap(bp);
	}
	
	for(i = 0; i < 1024; i++)
	{
		object[i].x = t3f_drand(&rng_state) * 640.0;
		object[i].y = t3f_drand(&rng_state) * 480.0;
		object[i].vx = t3f_drand(&rng_state) * 4.0 - 2.0;
		object[i].vy = t3f_drand(&rng_state) * 4.0 - 2.0;
		object[i].bitmap = t3f_rand(&rng_state) % 8;
	}
	t3f_run();
	return 0;
}
