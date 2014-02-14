#include "t3f/t3f.h"
#include "t3f/collision.h"
#include "t3f/rng.h"

#define MAX_SPRITES 32

typedef struct
{
	
	T3F_COLLISION_OBJECT * object;
	float x, y;
	float vx, vy, angle;
	
} SPRITE;

SPRITE sprite[MAX_SPRITES];
ALLEGRO_BITMAP * sprite_image = NULL;
T3F_RNG_STATE rng_state;

void logic(void * data)
{
	int i, j;
	
	for(i = 0; i < MAX_SPRITES; i++)
	{
		/* move horizontally */
		sprite[i].x += sprite[i].vx;
		
		/* reverse direction if sprite is past edge of screen */
		if(sprite[i].x < 0)
		{
			sprite[i].x = 0;
			sprite[i].vx = -sprite[i].vx;
		}
		else if(sprite[i].x >= 640 - 16)
		{
			sprite[i].x = 639 - 16;
			sprite[i].vx = -sprite[i].vx;
		}
		
		/* move the collision object and see if the sprite collides with any other
		 * sprites */
		t3f_move_collision_object_x(sprite[i].object, sprite[i].x);
		for(j = 0; j < MAX_SPRITES; j++)
		{
			if(i != j && t3f_check_object_collision(sprite[i].object, sprite[j].object))
			{
				/* move to edge of sprite */
				sprite[i].x = t3f_get_object_collision_x(sprite[i].object, sprite[j].object);
				t3f_move_collision_object_x(sprite[i].object, sprite[i].x);
				
				/* reverse direction of sprite[j] if it was moving toward
				 * sprite[i] */
				if((sprite[i].vx > 0.0 && sprite[j].vx < 0.0) || (sprite[j].vx > 0.0 && sprite[i].vx < 0.0))
				{
					sprite[j].vx = -sprite[j].vx;
				}
				
				/* reverse direction */
				sprite[i].vx = -sprite[i].vx;
				break;
			}
		}
		/* move vertically */
		sprite[i].y += sprite[i].vy;
		
		/* reverse direction if sprite is past edge of screen */
		if(sprite[i].y < 0)
		{
			sprite[i].y = 0;
			sprite[i].vy = -sprite[i].vy;
		}
		else if(sprite[i].y >= 480 - 16)
		{
			sprite[i].y = 479 - 16;
			sprite[i].vy = -sprite[i].vy;
		}
		
		/* move the collision object and see if the sprite collides with any other
		 * sprites */
		t3f_move_collision_object_y(sprite[i].object, sprite[i].y);
		for(j = 0; j < MAX_SPRITES; j++)
		{
			if(i != j && t3f_check_object_collision(sprite[i].object, sprite[j].object))
			{
				/* move to edge of sprite */
				sprite[i].y = t3f_get_object_collision_y(sprite[i].object, sprite[j].object);
				t3f_move_collision_object_y(sprite[i].object, sprite[i].y);
				
				/* reverse direction of sprite[j] if it was moving toward
				 * sprite[i] */
				if((sprite[i].vy > 0.0 && sprite[j].vy < 0.0) || (sprite[j].vy > 0.0 && sprite[i].vy < 0.0))
				{
					sprite[j].vy = -sprite[j].vy;
				}
				
				/* reverse direction */
				sprite[i].vy = -sprite[i].vy;
				break;
			}
		}
	}
}

void render(void * data)
{
	int i;
	
	al_clear_to_color(al_map_rgb_f(1.0, 1.0, 1.0));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < MAX_SPRITES; i++)
	{
		al_draw_bitmap(sprite_image, sprite[i].x, sprite[i].y, 0);
	}
	al_hold_bitmap_drawing(false);
}

bool initialize(void)
{
	int i;
	
	if(!t3f_initialize("ex_collision", 640, 480, 60, logic, render, T3F_USE_KEYBOARD, NULL))
	{
		return false;
	}
	t3f_srand(&rng_state, time(0));
	sprite_image = al_load_bitmap("data/listener.png");
	if(!sprite_image)
	{
		return false;
	}
	for(i = 0; i < MAX_SPRITES; i++)
	{
		sprite[i].x = (i % 8) * (640 / 8);
		sprite[i].y = (i / 8) * (480 / 4);
		sprite[i].angle = t3f_drand(&rng_state) * (ALLEGRO_PI * 2);
		sprite[i].vx = 2 * cos(sprite[i].angle);
		sprite[i].vy = 2 * sin(sprite[i].angle);
		sprite[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		if(!sprite[i].object)
		{
			return false;
		}
		t3f_move_collision_object_xy(sprite[i].object, sprite[i].x, sprite[i].y);
	}
	return true;
}

int main(void)
{
	if(!initialize())
	{
		return 1;
	}
	t3f_run();
	return 0;
}
