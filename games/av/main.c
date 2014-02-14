#include "includes.h"

THEME av_theme;
GAME  av_game;
ALLEGRO_FONT * av_font = NULL;
ALLEGRO_FONT * av_font_small = NULL;
bool paused = false;
int keys[8] = {0};
T3F_ATLAS * av_atlas = NULL;

void av_logic(void * data)
{
	if(!paused)
	{
		// Do all input processing and game logic here. Refer to the
		// comments in IGame.h for details on the format of the input
		// parameters and the requirements for the output.
		// ...
		keys[0] = t3f_key[ALLEGRO_KEY_LEFT];
		keys[1] = t3f_key[ALLEGRO_KEY_RIGHT];
		keys[3] = t3f_key[ALLEGRO_KEY_DOWN];
		keys[4] = t3f_key[ALLEGRO_KEY_Z];
		keys[5] = t3f_key[ALLEGRO_KEY_X];
		read_controller(&av_game.controller, keys);
		if(!do_game(&av_game))
		{
			return;
		}
	}
}

void show_title(void)
{
	float y = 48.0;
	
	al_draw_filled_rectangle(32, 32, 640 - 32 - 1, 480 - 32 - 1, al_map_rgba_f(0.0, 0.0, 0.0, 0.75));
	al_draw_rectangle(32, 32, 640 - 32 - 1, 480 - 32 - 1, al_map_rgba_f(1.0, 1.0, 0.0, 1.0), 2.0);
	al_hold_bitmap_drawing(true);
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "Controls");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "--------");
	y += 32;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "Left/Right: Move pill");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "Z/X:        Rotate left/right");
	y += 48;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "Objective");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "---------");
	y += 32;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "Stack the pills up to create");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "matches of 4 of the same color");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "vertically or horizontally.");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "Eliminate all the viruses to");
	y += 16;
	al_draw_text(av_font, t3f_color_white, 48, y, 0, "move on to the next level.");
	y += 144;
	al_draw_text(av_font, al_map_rgb_f(0.0, 1.0, 0.0), 320, y, ALLEGRO_ALIGN_CENTRE, "Press ROTATE to begin.");
	al_hold_bitmap_drawing(false);
}

void show_game_over(void)
{
	float y = 192;
	
	al_draw_filled_rectangle(128, 176, 640 - 128 - 1, 480 - 176 - 1, al_map_rgba_f(0.0, 0.0, 0.0, 0.75));
	al_draw_rectangle(128, 176, 640 - 128 - 1, 480 - 176 - 1, al_map_rgba_f(1.0, 1.0, 0.0, 1.0), 2.0);
	al_hold_bitmap_drawing(true);
	al_draw_text(av_font, al_map_rgb_f(1.0, 0.0, 0.0), 320, y, ALLEGRO_ALIGN_CENTRE, "Game Over");
	y += 32;
	al_draw_textf(av_font, t3f_color_white, 320, y, ALLEGRO_ALIGN_CENTRE, "Final Score: %d", av_game.score);
	y += 32;
	al_draw_text(av_font, al_map_rgb_f(0.0, 1.0, 0.0), 320, y, ALLEGRO_ALIGN_CENTRE, "Press ROTATE to");
	y += 16;
	al_draw_text(av_font, al_map_rgb_f(0.0, 1.0, 0.0), 320, y, ALLEGRO_ALIGN_CENTRE, "play again.");
	al_hold_bitmap_drawing(false);
}

void av_render(void * data)
{
	int y = 314;
	
	/* this reduces the amount of texture switching so we can draw more efficiently */
	al_hold_bitmap_drawing(true);
	
	/* draw the background */
	al_draw_bitmap(av_theme.backdrop, 0, 0, 0);
	
	/* draw the game board */
	if(!paused && av_game.state != GAME_STATE_LEVEL)
	{
		draw_game(&av_game, &av_theme);
	}
	
	/* draw the game status */
	al_draw_textf(av_font_small, al_map_rgb(0, 0, 0), 440, y, ALLEGRO_ALIGN_CENTRE, "SCORE");
	y += 12;
	al_draw_textf(av_font_small, al_map_rgb(0, 0, 0), 440, y, ALLEGRO_ALIGN_CENTRE, "%d", av_game.score);
	y += 24;
	al_draw_textf(av_font_small, al_map_rgb(0, 0, 0), 440, y, ALLEGRO_ALIGN_CENTRE, "LEVEL");
	y += 12;
	al_draw_textf(av_font_small, al_map_rgb(0, 0, 0), 440, y, ALLEGRO_ALIGN_CENTRE, "%d", av_game.level);
	y += 24;
	al_draw_textf(av_font_small, al_map_rgb(0, 0, 0), 440, y, ALLEGRO_ALIGN_CENTRE, "HIGH");
	y += 12;
	al_draw_textf(av_font_small, al_map_rgb(0, 0, 0), 440, y, ALLEGRO_ALIGN_CENTRE, "%d", av_game.high_score);
	
	/* draw end level text if we are finished */
	if(av_game.state == GAME_STATE_LEVEL && (av_game.timer / 15) % 2 == 0)
	{
		al_draw_textf(av_font, al_map_rgb(0, 0, 0), 320 + 2, 232 + 2, ALLEGRO_ALIGN_CENTRE, "LEVEL COMPLETE");
		al_draw_textf(av_font, al_map_rgb(255, 255, 255), 320, 232, ALLEGRO_ALIGN_CENTRE, "LEVEL COMPLETE");
	}
	
	/* if you used al_hold_bitmap_drawing(true), you must call this if you want
	 * all of your stuff to be drawn */
	al_hold_bitmap_drawing(false);
	
	if(av_game.state == GAME_STATE_TITLE)
	{
		show_title();
	}
	else if(av_game.state == GAME_STATE_OVER)
	{
		show_game_over();
	}
}

ALLEGRO_FONT * av_load_font(const char * fn)
{
	ALLEGRO_FONT * fp;
	ALLEGRO_BITMAP * bp;
	int ranges[] = {32, 126};
	
	bp = al_load_bitmap(fn);
	if(!bp)
	{
		return NULL;
	}
	al_convert_mask_to_alpha(bp, al_map_rgb(255, 0, 255));
	fp = al_grab_font_from_bitmap(bp, 1, ranges);
	al_destroy_bitmap(bp);
	return fp;
}

ALLEGRO_BITMAP * av_load_bitmap(const char * fn)
{
	ALLEGRO_BITMAP * bp;
	
	bp = al_load_bitmap(fn);
	if(!bp)
	{
		return NULL;
	}
	al_convert_mask_to_alpha(bp, al_map_rgb(255, 0, 255));
	return bp;
}

bool av_initialize(void)
{
	ALLEGRO_STATE old_state;
	const char * val;
	int i;
	
	/* initialize the game with a 640x480 display at 100fps using keyboard and sound */
	if(!t3f_initialize("Anti-Virus", 640, 480, 100.0, av_logic, av_render, T3F_USE_KEYBOARD | T3F_USE_SOUND, NULL))
	{
		return false;
	}
	
	/* setting this blender allows us draw sprites with varying levels of translucency */
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	
	/* load the board tile images */
	av_theme.block[0] = al_create_bitmap(8, 8);
	av_theme.block[1] = av_load_bitmap("data/graphics/pill_red4.pcx");
	av_theme.block[2] = av_load_bitmap("data/graphics/pill_red0.pcx");
	av_theme.block[3] = av_load_bitmap("data/graphics/pill_red2.pcx");
	av_theme.block[4] = av_load_bitmap("data/graphics/pill_red1.pcx");
	av_theme.block[5] = av_load_bitmap("data/graphics/pill_red3.pcx");
	av_theme.block[6] = av_load_bitmap("data/graphics/pill_yellow4.pcx");
	av_theme.block[7] = av_load_bitmap("data/graphics/pill_yellow0.pcx");
	av_theme.block[8] = av_load_bitmap("data/graphics/pill_yellow2.pcx");
	av_theme.block[9] = av_load_bitmap("data/graphics/pill_yellow1.pcx");
	av_theme.block[10] = av_load_bitmap("data/graphics/pill_yellow3.pcx");
	av_theme.block[11] = av_load_bitmap("data/graphics/pill_blue4.pcx");
	av_theme.block[12] = av_load_bitmap("data/graphics/pill_blue0.pcx");
	av_theme.block[13] = av_load_bitmap("data/graphics/pill_blue2.pcx");
	av_theme.block[14] = av_load_bitmap("data/graphics/pill_blue1.pcx");
	av_theme.block[15] = av_load_bitmap("data/graphics/pill_blue3.pcx");
	
	/* make sure they all loaded */
	for(i = 0; i < 16; i++)
	{
		if(!av_theme.block[i])
		{
			printf("Failed to load block images (%d).\n", i);
			return false;
		}
	}
	
	/* load virus animations */
	av_theme.virus[0] = t3f_load_animation("data/graphics/virus_red.ani");
	av_theme.virus[1] = t3f_load_animation("data/graphics/virus_yellow.ani");
	av_theme.virus[2] = t3f_load_animation("data/graphics/virus_blue.ani");
	
	/* make sure they all loaded */
	for(i = 0; i < 3; i++)
	{
		if(!av_theme.virus[i])
		{
			printf("Failed to load virus animations (%d).\n", i);
			return false;
		}
	}
	
	/* load background image */
	av_theme.backdrop = al_load_bitmap("data/graphics/bg.pcx");
	if(!av_theme.backdrop)
	{
		printf("Failed to load backdrop.\n");
		return false;
	}
	
	/* load sounds */
	av_theme.sound[THEME_SOUND_MOVE] = al_load_sample("data/sounds/move.wav");
	av_theme.sound[THEME_SOUND_ROTATE] = al_load_sample("data/sounds/rotate.wav");
	av_theme.sound[THEME_SOUND_LAND] = al_load_sample("data/sounds/land.wav");
	av_theme.sound[THEME_SOUND_MATCH] = al_load_sample("data/sounds/match.wav");
	av_theme.sound[THEME_SOUND_KILL] = al_load_sample("data/sounds/kill.wav");
	av_theme.sound[THEME_SOUND_LEVEL] = al_load_sample("data/sounds/levelup.wav");
	av_theme.sound[THEME_SOUND_PAUSE] = al_load_sample("data/sounds/pause.wav");
	
	/* make sure they all loaded */
	for(i = 0; i < 7; i++)
	{
		if(!av_theme.sound[i])
		{
			printf("Failed to load sounds (%d).\n", i);
			return false;
		}
	}
	
	/* load fonts */
	av_font = al_load_font("data/fonts/kongtext.ttf", 16, 0);
	if(!av_font)
	{
		printf("Failed to load game font.\n");
		return false;
	}
	av_font_small = al_load_font("data/fonts/kongtext.ttf", 12, 0);
	if(!av_font_small)
	{
		printf("Failed to load game font.\n");
		return false;
	}
	
	/* clear block 0 */
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(av_theme.block[0]);
	al_clear_to_color(t3f_color_black);
	al_restore_state(&old_state);
	
	/* create atlas so we can draw more efficiently */
	av_atlas = t3f_create_atlas(1024, 1024);
	if(av_atlas)
	{
		for(i = 0; i < 16; i++)
		{
			t3f_add_bitmap_to_atlas(av_atlas, &av_theme.block[i], T3F_ATLAS_SPRITE);
		}
		for(i = 0; i < 3; i++)
		{
			t3f_add_animation_to_atlas(av_atlas, av_theme.virus[i], T3F_ATLAS_SPRITE);
		}
	}
	
	/* set up game */
	av_game.tp = &av_theme;
	av_game.frame = 0;
	initialize_game(&av_game, 0);
	av_game.x = BOTTLE_X_1P;
	av_game.y = BOTTLE_Y_1P;
//	av_game.level = av_settings[4].value + 1;
	av_game.level = 1;
//	av_game.difficulty = av_settings[3].value;
	av_game.difficulty = 0;
	av_game.board.layout = NULL;
	av_game.aid = true;
	av_game.over = 0;
	
	/* load high score */
	val = al_get_config_value(t3f_config, "Game Data", "High Score");
	if(val)
	{
		av_game.high_score = atoi(val);
	}
	else
	{
		av_game.high_score = 0;
	}
	paused = 0;
	srand(time(0));
	return true;
}

int main(void)
{
	if(!av_initialize())
	{
		return 1;
	}
	t3f_run();
	return 0;
}
