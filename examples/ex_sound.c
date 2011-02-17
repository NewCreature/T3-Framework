#include "t3f.h"
#include "sound.h"

ALLEGRO_BITMAP * bitmap = NULL;
ALLEGRO_SAMPLE * sample = NULL;
ALLEGRO_SAMPLE * queue_sample = NULL;
bool was_clicked = false;

void logic(void)
{
	float vol, pan;
	
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	if(t3f_key[ALLEGRO_KEY_SPACE])
	{
		t3f_queue_sample(queue_sample);
		t3f_key[ALLEGRO_KEY_SPACE] = 0;
	}
	if(t3f_mouse_button[0])
	{
		if(!was_clicked)
		{
			vol = t3f_get_sound_gain(320, 240, t3f_mouse_x, t3f_mouse_y, 320);
			pan = t3f_get_sound_position(320, 240, t3f_mouse_x, t3f_mouse_y);
			t3f_play_sample(sample, vol, pan, 1.0);
			was_clicked = true;
		}
	}
	else
	{
		was_clicked = false;
	}
	t3f_poll_sound_queue();
}

void render(void)
{
	al_clear_to_color(al_map_rgba_f(1.0, 1.0, 1.0, 1.0));
	al_draw_bitmap(bitmap, 320 - al_get_bitmap_width(bitmap) / 2, 240 - al_get_bitmap_height(bitmap) / 2, 0);
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("ex_sound", 640, 480, 60.0, logic, render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_SOUND))
	{
		return 1;
	}
	if(!(t3f_flags & T3F_USE_SOUND)) // no sound so exit
	{
		return 1;
	}
	bitmap = al_load_bitmap("data/listener.png");
	if(!bitmap)
	{
		return 1;
	}
	sample = al_load_sample("data/sound.ogg");
	if(!sample)
	{
		return 1;
	}
	queue_sample = al_load_sample("data/queue_sound.ogg");
	if(!queue_sample)
	{
		return 1;
	}
	t3f_run();
	return 0;
}
