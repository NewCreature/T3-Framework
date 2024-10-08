#include "t3f/t3f.h"

ALLEGRO_BITMAP * bitmap = NULL;

void logic(void * data)
{
}

void render(void * data)
{
	int i;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch_active(i))
		{
			al_draw_bitmap(bitmap, t3f_get_touch_x(i) - al_get_bitmap_width(bitmap) / 2, t3f_get_touch_y(i) - al_get_bitmap_height(bitmap) / 2, 0);
		}
	}
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_touch", 640, 480, 60.0, logic, render, T3F_USE_MOUSE | T3F_USE_TOUCH, NULL))
	{
		return 1;
	}
	bitmap = al_load_bitmap("data/0.png");
	if(!bitmap)
	{
		return 1;
	}
	t3f_run();
	return 0;
}
