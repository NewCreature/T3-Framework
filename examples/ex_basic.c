#include "t3f/t3f.h"

ALLEGRO_BITMAP * bitmap = NULL;
ALLEGRO_FONT * font = NULL;
float x = 0.0;
float y = 0.0;
char text[1024] = {0};
int text_pos = 0;

void logic(void * data)
{
	if(t3f_key_held(ALLEGRO_KEY_ESCAPE))
	{
		t3f_exit();
	}
	else if(t3f_key_held(ALLEGRO_KEY_LEFT))
	{
		x -= 0.5;
	}
	else if(t3f_key_held(ALLEGRO_KEY_RIGHT))
	{
		x += 0.5;
	}
	else if(t3f_key_held(ALLEGRO_KEY_UP))
	{
		y -= 0.5;
	}
	else if(t3f_key_held(ALLEGRO_KEY_DOWN))
	{
		y += 0.5;
	}
	else
	{
		int k = t3f_get_char(0);
		switch(k)
		{
			case '\b':
			{
				if(text_pos > 0)
				{
					text_pos--;
					text[text_pos] = '\0';
				}
				break;
			}
			case '\r':
			case '\t':
			case '\0':
			{
				break;
			}
			default:
			{
				text[text_pos] = k;
				text_pos++;
			}
		}
	}
}

void render(void * data)
{
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_bitmap(bitmap, x, y, 0);
	
	/* draw red, partially transparent text */
	al_draw_text(font, al_map_rgba_f(1.0, 0.0, 0.0, 0.75), 0, 448, 0, text);
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_basic", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD, NULL))
	{
		return 1;
	}
	bitmap = al_load_bitmap("data/bitmap.png");
	if(!bitmap)
	{
		return 1;
	}
	font = al_load_bitmap_font("data/font.png");
	if(!font)
	{
		return 1;
	}
	t3f_run();
	return 0;
}
