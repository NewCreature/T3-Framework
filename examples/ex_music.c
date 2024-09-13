#include "t3f/t3f.h"
#include "t3f/music.h"

ALLEGRO_FONT * font = NULL;
bool playing = false;

void logic(void * data)
{
	if(t3f_key_held(ALLEGRO_KEY_ESCAPE))
	{
		t3f_exit();
	}
	if(t3f_key_pressed(ALLEGRO_KEY_SPACE))
	{
		if(playing)
		{
			t3f_pause_music();
			playing = false;
		}
		else
		{
			t3f_resume_music();
			playing = true;
		}
		t3f_use_key_press(ALLEGRO_KEY_SPACE);
	}
}

void render(void * data)
{
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	al_draw_text(font, al_map_rgba_f(1.0, 0.0, 0.0, 0.75), 0, 448, 0, "Playing \"data/music.xm\"");
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_music", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_SOUND, NULL))
	{
		return 1;
	}
	if(!(t3f_flags & T3F_USE_SOUND)) // no sound so exit
	{
		return 1;
	}
	font = al_load_bitmap_font("data/font.png");
	if(!font)
	{
		return 1;
	}
	if(!t3f_play_music("data/music.xm"))
	{
		return 1;
	}
	playing = true;
	t3f_run();
	t3f_stop_music();
	return 0;
}
