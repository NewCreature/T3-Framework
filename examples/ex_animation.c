#include "t3f/t3f.h"
#include "t3f/animation.h"
#include "t3f/rng.h"

typedef struct
{
	
	float x, y;
	float vx, vy;
	int tick;
	
} OBJECT;

ALLEGRO_BITMAP * bitmap[8] = {NULL};
T3F_ATLAS * atlas = NULL;
T3F_ANIMATION * animation = NULL;
OBJECT object[1024];

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
		object[i].tick++;
	}
}

void render(void * data)
{
	int i;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < 1024; i++)
	{
		t3f_draw_animation(animation, t3f_color_white, object[i].tick, object[i].x, object[i].y, 0.0, 0);
	}
	al_hold_bitmap_drawing(false);
}

int main(int argc, char * argv[])
{
	int i;
	char fn[1024] = {0};
	T3F_RNG_STATE rng_state;
	
	if(!t3f_initialize("ex_animation", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE, NULL))
	{
		return 1;
	}
	t3f_srand(&rng_state, time(0));
	
	/* load bitmaps */
	for(i = 0; i < 8; i++)
	{
		sprintf(fn, "data/%d.png", i);
		bitmap[i] = al_load_bitmap(fn);
		if(!bitmap[i])
		{
			return 1;
		}
	}
	
	animation = t3f_create_animation();
	if(!animation)
	{
		return 1;
	}
	for(i = 0; i < 8; i++)
	{
		t3f_animation_add_bitmap(animation, bitmap[i]);
		t3f_animation_add_frame(animation, i, 0, 0, 0, al_get_bitmap_width(bitmap[i]), al_get_bitmap_height(bitmap[i]), 0, 10);
	}
	
	atlas = t3f_create_atlas(512, 512);
	if(atlas)
	{
		t3f_add_animation_to_atlas(atlas, animation, T3F_ATLAS_SPRITE);
	}
	
	for(i = 0; i < 1024; i++)
	{
		object[i].x = t3f_drand(&rng_state) * 640.0;
		object[i].y = t3f_drand(&rng_state) * 480.0;
		object[i].vx = t3f_drand(&rng_state) * 4.0 - 2.0;
		object[i].vy = t3f_drand(&rng_state) * 4.0 - 2.0;
		object[i].tick = t3f_rand(&rng_state) % 256;
	}
	t3f_run();
	return 0;
}
