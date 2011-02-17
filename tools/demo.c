#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/winalleg.h>

#include "../t3f/t3f.h"

#include "../t3f/3d.h"
#include "../t3f/tilemap.h"
#include "../t3f/collision.h"
#include "../t3f/controller.h"

#include "../t3f/animation.h"

#define DEMO_SPRITE_STATES 10

#define DEMO_SPRITE_STATE_STAND_RIGHT 0
#define DEMO_SPRITE_STATE_WALK_RIGHT  1
#define DEMO_SPRITE_STATE_JUMP_RIGHT  2
#define DEMO_SPRITE_STATE_FALL_RIGHT  3
#define DEMO_SPRITE_STATE_DUCK_RIGHT  4
#define DEMO_SPRITE_STATE_STAND_LEFT  5
#define DEMO_SPRITE_STATE_WALK_LEFT   6
#define DEMO_SPRITE_STATE_JUMP_LEFT   7
#define DEMO_SPRITE_STATE_FALL_LEFT   8
#define DEMO_SPRITE_STATE_DUCK_LEFT   9

#define DEMO_CONTROLLER_UP            0
#define DEMO_CONTROLLER_DOWN          1
#define DEMO_CONTROLLER_LEFT          2
#define DEMO_CONTROLLER_RIGHT         3

/* Allegro stuff */
ALLEGRO_FONT * demo_font = NULL;

/* OCD Animation Studio stuff */
T3F_TILESET * demo_tileset = NULL;
T3F_TILEMAP * demo_tilemap = NULL;
T3F_COLLISION_TILEMAP * demo_collision_tilemap = NULL;
T3F_TILEMAP_CAMERA demo_camera;
T3F_ANIMATION * sprite_ani[10] = {NULL};
T3F_ANIMATION * platform_ani = NULL;
T3F_CONTROLLER * controller = NULL;
unsigned long demo_tick = 0;
bool demo_quit = false;
float target_z = 100.0;

typedef struct
{
	
	float x, y, angle;
	T3F_COLLISION_OBJECT * cp;
	T3F_COLLISION_OBJECT * sticky;
	bool floor;
	
} SPRITE;

typedef struct
{
	float x, y;
	float vx, vy;
	T3F_COLLISION_OBJECT * cp;
	T3F_COLLISION_OBJECT * sticky;
	bool floor;
	int state;
	int frame;
} PLAYER;

SPRITE platform;
PLAYER player;
bool collision = false;
bool stepmode = false;

void demo_logic(void)
{
	int skipy = 0;
	
	if(stepmode)
	{
		if(t3f_key[ALLEGRO_KEY_ENTER])
		{
			t3f_key[ALLEGRO_KEY_ENTER] = 0;
		}
		else
		{
			return;
		}
	}
	
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	if(t3f_key[ALLEGRO_KEY_F1])
	{
		stepmode = true;
	}
	if(t3f_key[ALLEGRO_KEY_1])
	{
		target_z = demo_tilemap->layer[0]->z;
	}
	else if(t3f_key[ALLEGRO_KEY_2])
	{
		target_z = demo_tilemap->layer[1]->z;
	}
	else if(t3f_key[ALLEGRO_KEY_3])
	{
		target_z = demo_tilemap->layer[2]->z;
	}
	else if(t3f_key[ALLEGRO_KEY_4])
	{
		target_z = demo_tilemap->layer[3]->z;
	}
	else if(t3f_key[ALLEGRO_KEY_5])
	{
		target_z = demo_tilemap->layer[4]->z;
	}
	t3f_read_controller(controller);
	if(t3f_key[ALLEGRO_KEY_LEFT])
	{
		demo_camera.x -= 2.0;
	}
	if(t3f_key[ALLEGRO_KEY_RIGHT])
	{
		demo_camera.x += 2.0;
	}
	if(t3f_key[ALLEGRO_KEY_UP])
	{
		demo_camera.y -= 2.0;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN])
	{
		demo_camera.y += 2.0;
	}
	if(demo_camera.z > target_z)
	{
		demo_camera.z -= 10.0;
		if(demo_camera.z < target_z)
		{
			demo_camera.z = target_z;
		}
	}
	else if(demo_camera.z < target_z)
	{
		demo_camera.z += 10.0;
		if(demo_camera.z > target_z)
		{
			demo_camera.z = target_z;
		}
	}
	platform.angle += 0.05;
	platform.x = 320 + cos(platform.angle) * 48.0;
	t3f_move_collision_object_x(platform.cp, platform.x);
	platform.y = 240 + sin(platform.angle) * 48.0;
	t3f_move_collision_object_y(platform.cp, platform.y);
	if(platform.cp->vy < 0.0 && platform.cp->oy + platform.cp->map.top.point[0].y > player.cp->y + player.cp->map.bottom.point[0].y && t3f_check_object_collision(platform.cp, player.cp))
	{
		player.sticky = platform.cp;
		player.y = t3f_get_object_top_y(player.cp, platform.cp);
		t3f_move_collision_object_y(player.cp, player.y);
		skipy = 1;
		player.floor = false;
	}
	
	/* add gravity */
	if(!player.floor && !player.sticky)
	{
		if(controller->state[DEMO_CONTROLLER_UP].held && player.vy < 0.0)
		{
			player.vy += 0.35;
		}
		else
		{
			player.vy += 0.5;
		}
		if(player.vy > 10.0)
		{
			player.vy = 10.0;
		}
		if(player.vy < 0)
		{
			if(player.state < 5)
			{
				player.state = DEMO_SPRITE_STATE_JUMP_RIGHT;
			}
			else
			{
				player.state = DEMO_SPRITE_STATE_JUMP_LEFT;
			}
		}
		else
		{
			if(player.state < 5)
			{
				player.state = DEMO_SPRITE_STATE_FALL_RIGHT;
			}
			else
			{
				player.state = DEMO_SPRITE_STATE_FALL_LEFT;
			}
		}
	}
	else
	{
		if(controller->state[DEMO_CONTROLLER_DOWN].held)
		{
			if(player.state < 5)
			{
				player.state = DEMO_SPRITE_STATE_DUCK_RIGHT;
			}
			else
			{
				player.state = DEMO_SPRITE_STATE_DUCK_LEFT;
			}
		}
		else
		{
			if(player.state == DEMO_SPRITE_STATE_DUCK_RIGHT)
			{
				player.state = DEMO_SPRITE_STATE_STAND_RIGHT;
			}
			else if(player.state == DEMO_SPRITE_STATE_DUCK_LEFT)
			{
				player.state = DEMO_SPRITE_STATE_STAND_LEFT;
			}
		}
	}
	
	if(controller->state[DEMO_CONTROLLER_LEFT].held && !controller->state[DEMO_CONTROLLER_RIGHT].held)
	{
		player.vx -= 0.5;
		if(player.vx < -3.0)
		{
			player.vx = -3.0;
		}
		if((player.floor || player.sticky) && player.vx < 0.0 && player.state != DEMO_SPRITE_STATE_WALK_LEFT)
		{
			player.state = DEMO_SPRITE_STATE_WALK_LEFT;
			player.frame = 0;
		}
		else
		{
			if(player.state == DEMO_SPRITE_STATE_JUMP_RIGHT)
			{
				player.state = DEMO_SPRITE_STATE_JUMP_LEFT;
			}
			else if(player.state == DEMO_SPRITE_STATE_FALL_RIGHT)
			{
				player.state = DEMO_SPRITE_STATE_FALL_LEFT;
			}
		}
	}
	else if(controller->state[DEMO_CONTROLLER_RIGHT].held && !controller->state[DEMO_CONTROLLER_LEFT].held)
	{
		player.vx += 0.5;
		if(player.vx > 3.0)
		{
			player.vx = 3.0;
		}
		if((player.floor || player.sticky) && player.vx > 0.0 && player.state != DEMO_SPRITE_STATE_WALK_RIGHT)
		{
			player.state = DEMO_SPRITE_STATE_WALK_RIGHT;
			player.frame = 0;
		}
		else
		{
			if(player.state == DEMO_SPRITE_STATE_JUMP_LEFT)
			{
				player.state = DEMO_SPRITE_STATE_JUMP_RIGHT;
			}
			else if(player.state == DEMO_SPRITE_STATE_FALL_LEFT)
			{
				player.state = DEMO_SPRITE_STATE_FALL_RIGHT;
			}
		}
	}
	else if(player.floor || player.sticky)
	{
		if(player.vx > 0.0)
		{
			player.vx -= 0.25;
			if(player.vx <= 0.0)
			{
				player.vx = 0.0;
				if(player.state != DEMO_SPRITE_STATE_DUCK_RIGHT)
				{
					player.state = DEMO_SPRITE_STATE_STAND_RIGHT;
				}
			}
		}
		else if(player.vx < 0.0)
		{
			player.vx += 0.25;
			if(player.vx >= 0.0)
			{
				player.vx = 0.0;
				if(player.state != DEMO_SPRITE_STATE_DUCK_LEFT)
				{
					player.state = DEMO_SPRITE_STATE_STAND_LEFT;
				}
			}
		}
	}
	player.x += player.vx;
	if(player.sticky)
	{
		player.x += player.sticky->vx;
	}
	t3f_move_collision_object_x(player.cp, player.x);
	if(!t3f_get_collision_tilemap_flag(demo_collision_tilemap, player.cp->x + player.cp->map.bottom.point[0].x, player.cp->y + player.cp->map.bottom.point[0].y, T3F_COLLISION_FLAG_SLOPE_TOP))
	{
		if(t3f_check_tilemap_collision_left(player.cp, demo_collision_tilemap))
		{
			player.x = t3f_get_tilemap_collision_x(player.cp, demo_collision_tilemap);
			player.vx = 0.0;
			t3f_move_collision_object_x(player.cp, player.x);
		}
		else if(t3f_check_tilemap_collision_right(player.cp, demo_collision_tilemap))
		{
			player.x = t3f_get_tilemap_collision_x(player.cp, demo_collision_tilemap);
			player.vx = 0.0;
			t3f_move_collision_object_x(player.cp, player.x);
		}
	}
	if(player.sticky && (player.cp->x + player.cp->map.left.point[0].x > player.sticky->x + player.sticky->map.right.point[0].x || player.cp->x + player.cp->map.right.point[0].x < player.sticky->x + player.sticky->map.left.point[0].x))
	{
		player.sticky = NULL;
		player.floor = false;
		player.vy = 0.0;
	}
	if(player.floor)
	{
		player.y = t3f_get_tilemap_walk_position(player.cp, demo_collision_tilemap, T3F_COLLISION_FLAG_SOLID_TOP);
		t3f_move_collision_object_y(player.cp, player.y);
	}
	
	if(controller->state[DEMO_CONTROLLER_UP].pressed && (player.floor || player.sticky))
	{
		player.vy = -10.0;
		player.sticky = NULL;
		player.floor = false;
	}
	if(player.sticky && !skipy)
	{
		player.y += player.sticky->vy;
	}
	if(!player.floor)
	{
		player.y += player.vy;
	}
	if((player.sticky && !skipy) || !player.floor)
	{
		t3f_move_collision_object_y(player.cp, player.y);
		
		/* collide with platform */
		if(player.cp->vy > 0.0 && player.cp->oy + player.cp->map.bottom.point[0].y < platform.cp->y + platform.cp->map.top.point[0].y && t3f_check_object_collision(player.cp, platform.cp))
		{
			player.y = t3f_get_object_top_y(player.cp, platform.cp);
			t3f_move_collision_object_y(player.cp, player.y);
			player.sticky = platform.cp;
			player.floor = false;
			if(player.state < 5)
			{
				player.state = DEMO_SPRITE_STATE_STAND_RIGHT;
			}
			else
			{
				player.state = DEMO_SPRITE_STATE_STAND_LEFT;
			}
		}
		
		if(t3f_check_tilemap_collision_bottom(player.cp, demo_collision_tilemap))
		{
			printf("land\n");
			player.y = t3f_get_tilemap_collision_y(player.cp, demo_collision_tilemap);
			t3f_move_collision_object_y(player.cp, player.y);
			player.floor = true;
			player.vy = 0.0;
			player.sticky = NULL;
			if(player.state < 5)
			{
				player.state = DEMO_SPRITE_STATE_STAND_RIGHT;
			}
			else
			{
				player.state = DEMO_SPRITE_STATE_STAND_LEFT;
			}
		}
		else if(t3f_check_tilemap_collision_top(player.cp, demo_collision_tilemap))
		{
			player.y = t3f_get_tilemap_collision_y(player.cp, demo_collision_tilemap);
			player.vy = 0.0;
			t3f_move_collision_object_y(player.cp, player.y);
		}
	}
	if(player.floor)
	{
		bool nf = false;
		bool af = false;
		int i;
		for(i = 0; i < player.cp->map.bottom.points; i++)
		{
			if(t3f_get_collision_tilemap_flag(demo_collision_tilemap, player.cp->x + player.cp->map.bottom.point[i].x, player.cp->y + player.cp->map.bottom.point[i].y + 1.0, T3F_COLLISION_FLAG_SOLID_TOP | T3F_COLLISION_FLAG_SLOPE_TOP) == (T3F_COLLISION_FLAG_SOLID_TOP | T3F_COLLISION_FLAG_SLOPE_TOP))
			{
				nf = true;
			}
			
			/* if any bottom point is touching a floor */
			if(t3f_check_collision_tilemap_flag(demo_collision_tilemap, player.cp->x + player.cp->map.bottom.point[i].x, player.cp->y + player.cp->map.bottom.point[i].y + 1.0, T3F_COLLISION_FLAG_SOLID_TOP, 0))
			{
				af = true;
			}
		}
//		if(!nf || (!af && t3f_get_collision_tilemap_flag(demo_collision_tilemap, player.cp->x + player.cp->map.bottom.point[0].x, player.cp->y + player.cp->map.bottom.point[0].y + 1.0, T3F_COLLISION_FLAG_SOLID_TOP) != (T3F_COLLISION_FLAG_SOLID_TOP)))
		if(!af || (nf && t3f_get_collision_tilemap_flag(demo_collision_tilemap, player.cp->x + player.cp->map.bottom.point[0].x, player.cp->y + player.cp->map.bottom.point[0].y + 1.0, T3F_COLLISION_FLAG_SOLID_TOP) != (T3F_COLLISION_FLAG_SOLID_TOP)))
		{
			player.vy = 0.0;
			player.floor = false;
			if(!af)
			{
				printf("af\n");
			}
			else
			{
				printf("nf\n");
			}
		}
	}
	player.frame++;
	
	if(t3f_check_object_collision(platform.cp, player.cp))
	{
		collision = true;
	}
	else
	{
		collision = false;
	}
	demo_camera.x = player.x + 16.0 - 320.0;
	demo_camera.y = player.y + 16.0 - 240.0;
	demo_tick++;
}

void demo_render(void)
{
	int i;
	t3f_select_camera(&demo_camera);
	al_clear_to_color(al_map_rgb(0, 0, 64));
	if(demo_tilemap && demo_tilemap->tileset)
	{
		for(i = 0; i < 3; i++)
		{
			t3f_render(demo_tilemap, i, demo_tick, &demo_camera);
		}
	}
	draw_animation(platform_ani, demo_tick, platform.x - demo_camera.x, platform.y - demo_camera.y, -demo_camera.z, 0);
	if(collision)
	{
		t3f_set_blender(al_map_rgba(255, 255, 255, 64));
	}
	draw_animation(sprite_ani[player.state], player.frame, player.x - demo_camera.x, player.y - demo_camera.y, -demo_camera.z, 0);
	if(collision)
	{
		t3f_unset_blender();
	}
	if(demo_tilemap && demo_tilemap->tileset)
	{
		for(i = 3; i < demo_tilemap->layers; i++)
		{
			t3f_render(demo_tilemap, i, demo_tick, &demo_camera);
		}
	}
//	al_font_textprintf(demo_font, 0, 0, "Camera Pos - (%f, %f, %f)", demo_camera.x, demo_camera.y, demo_camera.z);
	al_draw_textf(demo_font, 0, 0, 0, "CT - (%d, %d)", t3f_get_collision_tile_x(demo_collision_tilemap, player.x), t3f_get_collision_tile_y(demo_collision_tilemap, player.y));
	al_flip_display();
}


int demo_initialize(int argc, char * argv[])
{
	if(!t3f_initialize(640, 480, 60.0, "Paintball Party 2 Tech Demo 2", demo_logic, demo_render, T3F_DEFAULT | T3F_USE_MOUSE | T3F_USE_JOYSTICK))
	{
		return 0;
	}
	demo_font = al_load_bitmap_font("demo_font.png");
	if(!demo_font)
	{
		printf("Failed to load font!\n");
		return 0;
	}
	
	t3f_3d_set_projection(&demo_camera.projection, 1.0, 1.0, 320.0, 240.0, 640, 640);
	t3f_3d_select_projection(&demo_camera.projection);
	
	demo_camera.x = 0.0;
	demo_camera.y = 0.0;
	demo_camera.z = 0.0;
	demo_camera.vx = 0;
	demo_camera.vy = 0;
	demo_camera.vw = al_get_display_width() / 2;
	demo_camera.vh = al_get_display_height() / 2;
	demo_camera.flags = 0;
	t3f_3d_set_projection(&demo_camera.projection, 0.5, 0.5, 320.0, 240.0, 640, 640);
	t3f_3d_select_projection(&demo_camera.projection);
	
	demo_tileset = t3f_load_tileset("export.ots");
	if(!demo_tileset)
	{
		return 0;
	}
	demo_tilemap = t3f_load_tilemap("export.otm");
	if(!demo_tilemap)
	{
		return 0;
	}
	demo_tilemap->tileset = demo_tileset;
	
	demo_collision_tilemap = t3f_load_collision_tilemap("export.ocm");
	if(!demo_collision_tilemap)
	{
		return 0;
	}
	
	sprite_ani[DEMO_SPRITE_STATE_STAND_RIGHT] = load_animation("mario_sr.ani");
	sprite_ani[DEMO_SPRITE_STATE_WALK_RIGHT] = load_animation("mario_wr.ani");
	sprite_ani[DEMO_SPRITE_STATE_JUMP_RIGHT] = load_animation("mario_jr.ani");
	sprite_ani[DEMO_SPRITE_STATE_FALL_RIGHT] = load_animation("mario_fr.ani");
	sprite_ani[DEMO_SPRITE_STATE_DUCK_RIGHT] = load_animation("mario_dr.ani");
	sprite_ani[DEMO_SPRITE_STATE_STAND_LEFT] = load_animation("mario_sl.ani");
	sprite_ani[DEMO_SPRITE_STATE_WALK_LEFT] = load_animation("mario_wl.ani");
	sprite_ani[DEMO_SPRITE_STATE_JUMP_LEFT] = load_animation("mario_jl.ani");
	sprite_ani[DEMO_SPRITE_STATE_FALL_LEFT] = load_animation("mario_fl.ani");
	sprite_ani[DEMO_SPRITE_STATE_DUCK_LEFT] = load_animation("mario_dl.ani");
	if(!sprite_ani)
	{
		return 0;
	}
	platform_ani = load_animation("platform.ani");
	if(!sprite_ani)
	{
		return 0;
	}
	
	platform.angle = 0.0;
	platform.x = 320 + cos(platform.angle) * 48.0;
	platform.y = 240 + sin(platform.angle) * 48.0;
	platform.cp = t3f_create_collision_object(0, 0, al_get_bitmap_width(platform_ani->bitmap[0]) * 2, al_get_bitmap_height(platform_ani->bitmap[0]) * 2, 32, 32, 0);
	platform.sticky = NULL;
/*	player.x = 0.0;
	player.y = 0.0;
	player.cp = t3f_create_collision_object(0, 0, 32, 32, 32, 32, 0);
	player.sticky = NULL;
	player.floor = 0; */
	
	player.cp = t3f_create_collision_object(5, 0, 22, 56, 32, 32, 0);
	player.x = 64.0;
	player.y = 64.0;
	player.vx = 0.0;
	player.vy = 0.0;
	player.sticky = NULL;
	player.floor = false;
	player.state = DEMO_SPRITE_STATE_STAND_RIGHT;
	player.frame = 0;
	
	controller = t3f_create_controller(4);
	if(!controller)
	{
		return 0;
	}
	al_clear_to_color(al_map_rgb(255, 0, 0));
	al_flip_display();
	t3f_bind_controller(controller, DEMO_CONTROLLER_UP);
	al_clear_to_color(al_map_rgb(0, 255, 0));
	al_flip_display();
	t3f_bind_controller(controller, DEMO_CONTROLLER_DOWN);
	al_clear_to_color(al_map_rgb(0, 0, 255));
	al_flip_display();
	t3f_bind_controller(controller, DEMO_CONTROLLER_LEFT);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	t3f_bind_controller(controller, DEMO_CONTROLLER_RIGHT);
	
	return 1;
}

int main(int argc, char * argv[])
{
	if(!demo_initialize(argc, argv))
	{
		printf("Failed to initialize!\n");
		return 1;
	}
	t3f_play_music("demo_music.ogg", 0.0, 0.0);
	t3f_run();
	t3f_stop_music();

	return 0;
}
END_OF_MAIN()
