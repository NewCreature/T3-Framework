#include "t3f/t3f.h"
#include "t3f/tilemap.h"
#include "t3f/collision.h"

ALLEGRO_BITMAP * bitmap = NULL;
ALLEGRO_FONT * font = NULL;

T3F_TILESET * tileset = NULL;;
T3F_TILEMAP * tilemap = NULL;;

typedef struct
{
	
	float x, y, z;
	
} CAMERA;

CAMERA camera;

int tick = 0;

void logic(void * data)
{
	int mx, my, mz;
	
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	t3f_get_mouse_mickeys(&mx, &my, &mz);
	if(t3f_mouse_button[0])
	{
		camera.x += mx;
		camera.y += my;
	}
	camera.z += mz * 16;
	tick++;
}

void render(void * data)
{
	int i;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	for(i = 0; i < tilemap->layers; i++)
	{
		t3f_render_tilemap(tilemap, tileset, i, tick, camera.x, camera.y, camera.z, t3f_color_white);
	}
	al_draw_textf(font, t3f_color_white, 0, 0, 0, "Camera Position = (%d, %d, %d)", (int)camera.x, (int)camera.y, (int)camera.z);
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_tilemap", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE, NULL))
	{
		return 1;
	}
	font = al_load_bitmap_font("data/font.png");
	if(!font)
	{
		return 1;
	}
	tileset = t3f_load_tileset("data/tileset.tts");
	if(!tileset)
	{
		return 1;
	}
	tilemap = t3f_load_tilemap("data/tilemap.ttm");
	if(!tilemap)
	{
		return 1;
	}
	camera.x = 0;
	camera.y = 0;
	camera.z = 0;
	t3f_run();
	return 0;
}
