#include "t3f/t3f.h"
#include "t3f/music.h"
#include "t3f/gui.h"
#include <allegro5/allegro_ttf.h>
#include "main.h"
#include "intro.h"
#include "game.h"

T3F_BITMAP * dot_bitmap[DOT_MAX_BITMAPS] = {NULL};
ALLEGRO_SAMPLE * dot_sample[DOT_MAX_SAMPLES] = {NULL};
ALLEGRO_FONT * dot_font = NULL;
T3F_ATLAS * dot_atlas = NULL;

/* program state */
int dot_state = DOT_STATE_INTRO;
int dot_tick = 0;

/* game data */
DOT_GAME dot_game;

void dot_shadow_text(ALLEGRO_FONT * font, ALLEGRO_COLOR color, ALLEGRO_COLOR shadow, float x, float y, float sx, float sy, int flags, const char * text)
{
	al_draw_text(font, shadow, x + sx, y + sy, flags, text);
	al_draw_text(font, color, x, y, flags, text);
}

void dot_logic(void * data)
{
	switch(dot_state)
	{
		case DOT_STATE_INTRO:
		{
			dot_intro_logic();
			break;
		}
		case DOT_STATE_GAME:
		{
			dot_game_logic();
			break;
		}
	}
}

void dot_render(void * data)
{
	switch(dot_state)
	{
		case DOT_STATE_INTRO:
		{
			dot_intro_render();
			break;
		}
		case DOT_STATE_GAME:
		{
			dot_game_render();
			break;
		}
	}
}

bool dot_initialize(int argc, char * argv[])
{
	const char * val;
	int i;

	/* initialize framework */
	if(!t3f_initialize("Dot to Dot Sweep", 640, 480, 60.0, dot_logic, dot_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_SOUND, NULL))
	{
		return false;
	}

	/* load images */
	dot_bitmap[DOT_BITMAP_BALL_RED] = t3f_load_bitmap("data/graphics/ball_red.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_RED])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_RED);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_GREEN] = t3f_load_bitmap("data/graphics/ball_green.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_GREEN])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_GREEN);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_BLUE] = t3f_load_bitmap("data/graphics/ball_blue.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_BLUE])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_BLUE);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_PURPLE] = t3f_load_bitmap("data/graphics/ball_purple.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_PURPLE])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_PURPLE);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_YELLOW] = t3f_load_bitmap("data/graphics/ball_yellow.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_YELLOW])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_YELLOW);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_ORANGE] = t3f_load_bitmap("data/graphics/ball_orange.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_ORANGE])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_ORANGE);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_BLACK] = t3f_load_bitmap("data/graphics/ball_black.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_BLACK])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_BLACK);
		return false;
	}
	dot_bitmap[DOT_BITMAP_BALL_EYES] = t3f_load_bitmap("data/graphics/ball_eyes.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_BALL_EYES])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_BALL_EYES);
		return false;
	}
	dot_bitmap[DOT_BITMAP_HUD] = t3f_load_bitmap("data/graphics/hud.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!dot_bitmap[DOT_BITMAP_HUD])
	{
		printf("Failed to load image %d!\n", DOT_BITMAP_HUD);
		return false;
	}

	/* load sounds */
	dot_sample[DOT_SAMPLE_START] = al_load_sample("data/sounds/sfx0005.wav");
	if(!dot_sample[DOT_SAMPLE_START])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_START);
		return false;
	}
	dot_sample[DOT_SAMPLE_GRAB] = al_load_sample("data/sounds/sfx0000.wav");
	if(!dot_sample[DOT_SAMPLE_GRAB])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_GRAB);
		return false;
	}
	dot_sample[DOT_SAMPLE_LOSE] = al_load_sample("data/sounds/sfx0001.wav");
	if(!dot_sample[DOT_SAMPLE_LOSE])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_LOSE);
		return false;
	}
	dot_sample[DOT_SAMPLE_GO] = al_load_sample("data/sounds/sfx0004.wav");
	if(!dot_sample[DOT_SAMPLE_GO])
	{
		printf("Failed to load sound %d!\n", DOT_SAMPLE_GO);
		return false;
	}

	dot_font = al_load_font("data/fonts/kongtext.ttf", 16, 0);
	if(!dot_font)
	{
		printf("Failed to load font!\n");
		return false;
	}

	/* build atlas */
	dot_atlas = t3f_create_atlas(1024, 1024);
	if(dot_atlas)
	{
		for(i = 0; i < DOT_MAX_BITMAPS; i++)
		{
			if(dot_bitmap[i])
			{
				t3f_add_bitmap_to_atlas(dot_atlas, &dot_bitmap[i]->bitmap, T3F_ATLAS_SPRITE);
			}
		}
	}

	/* load high score */
	val = al_get_config_value(t3f_config, "Game Data", "High Score");
	if(val)
	{
		dot_game.high_score = atoi(val);
	}
	else
	{
		dot_game.high_score = 0;
	}
	return true;
}

int main(int argc, char * argv[])
{
	if(!dot_initialize(argc, argv))
	{
		printf("Failed to initialize Dot to Dot Sweep\n");
		return -1;
	}
	t3f_run();
	return 0;
}
