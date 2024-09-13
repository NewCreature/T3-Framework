#include "t3f/t3f.h"
#include "t3f/draw.h"

typedef struct
{
	
	float x, y, z;
	
} OBJECT;

T3F_BITMAP * bitmap = NULL;
float camera_x = 0.0;
float camera_z = 0.0;
OBJECT object[20];

void logic(void * data)
{
	if(t3f_key_held(ALLEGRO_KEY_ESCAPE))
	{
		t3f_exit();
	}
	camera_x = t3f_get_mouse_x();
	camera_z = t3f_get_mouse_y() * 2 - 480;
}

void render(void * data)
{
	int i;
	float alpha;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < 20; i++)
	{
		alpha = ((object[i].z + camera_z) + 400.0) / 800.0;
		t3f_draw_bitmap(bitmap, al_map_rgba_f(1.0, 1.0, 1.0, alpha), object[i].x - camera_x, object[i].y, object[i].z + camera_z, 0);
	}
	al_hold_bitmap_drawing(false);
}

static int qsort_objects(const void * e1, const void * e2)
{
    OBJECT * thing1 = (OBJECT *)e1;
    OBJECT * thing2 = (OBJECT *)e2;

    if(thing1->z < thing2->z)
	{
        return 1;
    }
    if(thing1->z > thing2->z)
    {
        return -1;
    }
    return 0;
}

int main(int argc, char * argv[])
{
	int i;
	
	if(!t3f_initialize("ex_3d", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE, NULL))
	{
		return 1;
	}
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	bitmap = t3f_load_bitmap("data/bitmap.png", 0, false);
	if(!bitmap)
	{
		return 1;
	}
	for(i = 0; i < 20; i++)
	{
		object[i].x = rand() % 640;
		object[i].y = rand() % 480;
		object[i].z = rand() % 800 - 400;
	}
	qsort(object, 20, sizeof(OBJECT), qsort_objects);
	t3f_run();
	return 0;
}
