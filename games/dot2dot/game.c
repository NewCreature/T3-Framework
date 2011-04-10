#include <math.h>
#include "t3f.h"
#include "sound.h"
#include "main.h"
#include "game.h"

/* initialize player (done once per turn and at new level) */
void dot_game_drop_player(int type)
{
	int i;
	
	/* initialize player */
	dot_game.player.x = 240 - 8;
	dot_game.player.y = 240 - 8;
	dot_game.player.z = 0.0;
	dot_game.player.a = ALLEGRO_PI / 2.0;
	dot_game.player.type = type;
	dot_game.player.active = true;
	dot_game.player.timer = 0;
	dot_game.combo = 0;
	t3f_play_sample(dot_sample[DOT_SAMPLE_START], 1.0, 0.0, 1.0);
	
	/* initialize game state */
	dot_game.state = DOT_GAME_STATE_START;
	dot_game.state_tick = 0;
	
	/* clear the area where the player is so we don't get cheap deaths */
	for(i = 0; i < DOT_GAME_MAX_BALLS; i++)
	{
		if(dot_game.ball[i].active)
		{
			if(t3f_distance(dot_game.ball[i].x + 8.0, dot_game.ball[i].y + 8.0, 240.0, 240.0) < 64)
			{
				dot_game.ball[i].a = atan2(dot_game.ball[i].y + 8.0 - 240.0, dot_game.ball[i].x + 8.0 - 240.0);
				dot_game.ball[i].vx = cos(dot_game.ball[i].a) * dot_game.ball[i].s;
				dot_game.ball[i].vy = sin(dot_game.ball[i].a) * dot_game.ball[i].s;
			}
		}
	}
}

/* function to set up a new level */
void dot_game_setup_level(int level)
{
	int i, j;
	int col = 0;
	int level_balls[DOT_GAME_MAX_LEVELS] =
	{
		32, 40, 48, 56, 64, 72, 80, 88, 96, 104
	};
	
	dot_game.level = level < 10 ? level : 10;
	
	/* initialize balls */
	for(i = 0; i < level_balls[dot_game.level]; i++)
	{
		dot_game.ball[i].x = rand() % (480 - 16);
		dot_game.ball[i].y = rand() % (480 - 16);
		dot_game.ball[i].z = 0;
		dot_game.ball[i].a = t3f_drand() * ALLEGRO_PI * 2.0;
		dot_game.ball[i].s = t3f_drand() * 0.75 + 0.25;
		dot_game.ball[i].vx = cos(dot_game.ball[i].a) * dot_game.ball[i].s;
		dot_game.ball[i].vy = sin(dot_game.ball[i].a) * dot_game.ball[i].s;
		dot_game.ball[i].type = col;
		col++;
		if(col > 5)
		{
			col = 0;
		}
		dot_game.ball[i].active = true;
	}
	
	/* add black balls */
	for(j = i; j < i + dot_game.level + 3; j++)
	{
		dot_game.ball[j].x = rand() % (480 - 16);
		dot_game.ball[j].y = rand() % (480 - 16);
		dot_game.ball[j].z = 0;
		dot_game.ball[j].a = t3f_drand() * ALLEGRO_PI * 2.0;
		dot_game.ball[j].s = t3f_drand() * 0.75 + 0.25;
		dot_game.ball[j].vx = cos(dot_game.ball[j].a) * dot_game.ball[j].s;
		dot_game.ball[j].vy = sin(dot_game.ball[j].a) * dot_game.ball[j].s;
		dot_game.ball[j].type = 6;
		dot_game.ball[j].active = true;
	}
	
	/* drop the player with a random color */
	dot_game_drop_player(rand() % 6);
	
	dot_game.timer = 0;
}

/* start the game from level 0 */
void dot_game_initialize(void)
{
	al_hide_mouse_cursor(t3f_display);
	dot_game_setup_level(0);
	dot_game.score = 0;
	dot_game.combo = 0;
	dot_game.lives = 3;
	dot_state = DOT_STATE_GAME;
}

/* finish the game */
void dot_game_exit(void)
{
	char buf[256] = {0};
	
	al_show_mouse_cursor(t3f_display);
	
	/* save high score */
	sprintf(buf, "%d", dot_game.high_score);
	al_set_config_value(t3f_config, "Game Data", "High Score", buf);
	
	/* go back to intro */
	dot_state = DOT_STATE_INTRO;
}

/* function to add points to the score
 * used when combo timer reaches 0, level is completed, or player loses */
void dot_game_accumulate_score(void)
{
	dot_game.score += dot_game.ascore * dot_game.combo;
	if(dot_game.score > dot_game.high_score)
	{
		dot_game.high_score = dot_game.score;
	}
}

/* return the number of colored balls left */
int dot_game_move_balls(void)
{
	float speed = 1.0;
	int colored = 0;
	int i;
	
	/* set the speed of the balls */
	if(dot_game.state == DOT_GAME_STATE_START)
	{
		speed = 0.25;
	}
	else
	{
		speed = 1.0 + (float)dot_game.timer / 3600.0;
		if(speed > 3.0)
		{
			speed = 3.0;
		}
	}
	
	/* move the balls */
	for(i = 0; i < DOT_GAME_MAX_BALLS; i++)
	{
		if(dot_game.ball[i].active)
		{
			dot_game.ball[i].x += dot_game.ball[i].vx * speed;
			if(dot_game.ball[i].x < 0.0)
			{
				dot_game.ball[i].vx = -dot_game.ball[i].vx;
			}
			if(dot_game.ball[i].x >= 480.0 - 16.0)
			{
				dot_game.ball[i].vx = -dot_game.ball[i].vx;
			}
			dot_game.ball[i].y += dot_game.ball[i].vy * speed;
			if(dot_game.ball[i].y < 0.0)
			{
				dot_game.ball[i].vy = -dot_game.ball[i].vy;
			}
			if(dot_game.ball[i].y >= 480.0 - 16.0)
			{
				dot_game.ball[i].vy = -dot_game.ball[i].vy;
			}
			if(dot_game.ball[i].type != DOT_BITMAP_BALL_BLACK)
			{
				colored++;
			}
		}
	}
	return colored;
}

/* figure out the direction to rotate to move the angle from a1 to a2 */
static float get_angle_dir(float a1, float a2)
{
	float distance = fabs(a1 - a2);
	if(distance < ALLEGRO_PI)
	{
		if(a1 < a2)
		{
			return -1.0;
		}
		else
		{
			return 1.0;
		}
	}
	if(a1 < a2)
	{
		return 1.0;
	}
	else
	{
		return -1.0;
	}
}

/* handle player movement */
void dot_game_move_player(void)
{
	int i, j;
	float ox, oy, target_angle;
	
	if(dot_game.player.active)
	{
		ox = dot_game.player.x;
		oy = dot_game.player.y;
		dot_game.player.x = t3f_mouse_x - 8.0;
		dot_game.player.y = t3f_mouse_y - 8.0;
		
		/* prevent player from moving past the edge */
		if(dot_game.player.x < 0.0)
		{
			dot_game.player.x = 0.0;
		}
		if(dot_game.player.x >= 480.0 - 16.0)
		{
			dot_game.player.x = 480.0 - 16.0 - 1.0;
		}
		if(dot_game.player.y < 0.0)
		{
			dot_game.player.y = 0.0;
		}
		if(dot_game.player.y >= 480.0 - 16.0)
		{
			dot_game.player.y = 480.0 - 16.0 - 1.0;
		}
		
		/* if the player has moved, change the angle of the character */
		if((int)ox != (int)dot_game.player.x || (int)oy != (int)dot_game.player.y)
		{
			target_angle = atan2(oy - dot_game.player.y, ox - dot_game.player.x);
			if(fabs(target_angle - dot_game.player.a) > ALLEGRO_PI / 16.0)
			{
				dot_game.player.a += (ALLEGRO_PI / 16.0) * get_angle_dir(target_angle, dot_game.player.a);
				if(dot_game.player.a > ALLEGRO_PI)
				{
					dot_game.player.a -= ALLEGRO_PI * 2.0;
				}
				if(dot_game.player.a < -ALLEGRO_PI)
				{
					dot_game.player.a += ALLEGRO_PI * 2.0;
				}
			}
		}
		
		/* handle combo timer */
		if(dot_game.combo > 0)
		{
			dot_game.player.timer++;
			if(dot_game.player.timer >= DOT_GAME_COMBO_TIME)
			{
				dot_game_accumulate_score();
				dot_game.player.timer = 0;
				dot_game.combo = 0;
				dot_game.ascore = 0;
			}
		}
		
		/* see if player ball hits any other balls */
		for(i = 0; i < DOT_GAME_MAX_BALLS; i++)
		{
			if(dot_game.ball[i].active)
			{
				if(t3f_distance(dot_game.ball[i].x, dot_game.ball[i].y, dot_game.player.x, dot_game.player.y) < 16.0)
				{
					
					/* hitting the same color gives you points and increases your combo */
					if(dot_game.ball[i].type == dot_game.player.type)
					{
						t3f_play_sample(dot_sample[DOT_SAMPLE_GRAB], 1.0, 0.0, 1.0);
						dot_game.ball[i].active = false;
						dot_game.ascore += 50;
						if(dot_game.player.timer < DOT_GAME_COMBO_TIME)
						{
							dot_game.combo++;
						}
						dot_game.player.timer = 0;
						for(j = 0; j < DOT_GAME_MAX_BALLS; j++)
						{
							if(dot_game.ball[j].active && dot_game.ball[j].type != DOT_BITMAP_BALL_BLACK && dot_game.ball[j].type != dot_game.player.type)
							{
								dot_game.ball[j].type = dot_game.player.type;
								break;
							}
						}
					}
					
					/* hitting other color kills you */
					else
					{
						t3f_play_sample(dot_sample[DOT_SAMPLE_LOSE], 1.0, 0.0, 1.0);
						dot_game_accumulate_score();
						dot_game.lives--;
						
						/* change ball color to match the ball that is hit unless it is black */
						if(dot_game.ball[i].type != DOT_BITMAP_BALL_BLACK)
						{
							if(dot_game.lives > 0)
							{
								dot_game_drop_player(dot_game.ball[i].type);
							}
							else
							{
								dot_game_exit();
							}
						}
						else
						{
							if(dot_game.lives > 0)
							{
								dot_game_drop_player(dot_game.player.type);
							}
							else
							{
								dot_game_exit();
							}
						}
						break;
					}
				}
			}
		}
	}
}

/* the main game logic function */
void dot_game_logic(void)
{
	int colored = 0;
	
	switch(dot_game.state)
	{
		
		/* balls move slow for a few seconds so player can get ready */
		case DOT_GAME_STATE_START:
		{
			dot_game.state_tick++;
			if(dot_game.state_tick >= 180)
			{
				t3f_play_sample(dot_sample[DOT_SAMPLE_GO], 1.0, 0.0, 1.0);
				dot_game.state = DOT_GAME_STATE_PLAY;
				dot_game.state_tick = 0;
				dot_game.player.active = true;
				al_set_mouse_xy(t3f_display, 240, 240);
				t3f_mouse_x = 240.0;
				t3f_mouse_y = 240.0;
			}
			
			/* handle ball logic */
			colored = dot_game_move_balls();
			break;
		}
		
		/* normal game state */
		default:
		{
			dot_game.timer++;
			
			/* handle ball logic */
			colored = dot_game_move_balls();
			
			/* move player */
			dot_game_move_player();
	
			/* move on to next level */
			if(colored == 0)
			{
				dot_game_accumulate_score();
				if(dot_game.level < 9)
				{
					dot_game_setup_level(dot_game.level + 1);
				}
				else
				{
					dot_game_exit();
				}
			}
			break;
		}
	}
}

/* render the HUD */
void dot_game_render_hud(void)
{
	char buffer[256] = {0};
	ALLEGRO_COLOR shadow = al_map_rgba_f(0.0, 0.0, 0.0, 0.25);
	
	al_draw_bitmap(dot_bitmap[DOT_BITMAP_HUD], 480.0, 0.0, 0);
	sprintf(buffer, "High");
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 8, 2, 2, 0, buffer);
	sprintf(buffer, "  %07d", dot_game.high_score);
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 24, 2, 2, 0, buffer);
	sprintf(buffer, "Score");
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 48, 2, 2, 0, buffer);
	sprintf(buffer, "  %07d", dot_game.score);
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 64, 2, 2, 0, buffer);
	sprintf(buffer, "Combo");
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 88, 2, 2, 0, buffer);
	sprintf(buffer, "      %03d", dot_game.combo);
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 104, 2, 2, 0, buffer);
	sprintf(buffer, "Level");
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 128, 2, 2, 0, buffer);
	sprintf(buffer, "       %02d", dot_game.level);
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 144, 2, 2, 0, buffer);
	sprintf(buffer, "Lives");
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 168, 2, 2, 0, buffer);
	sprintf(buffer, "       %02d", dot_game.lives);
	dot_shadow_text(dot_font, t3f_color_black, shadow, 480 + 8, 184, 2, 2, 0, buffer);
	
	al_draw_scaled_bitmap(dot_bitmap[DOT_BITMAP_BALL_RED + dot_game.player.type], 0, 0, 16, 16, 480 + 16, 336, 128, 128, 0);
	al_draw_scaled_rotated_bitmap(dot_bitmap[DOT_BITMAP_BALL_EYES], 8.0, 8.0, 480.0 + 80.0, 336 + 64 + 16, 8, 8, ALLEGRO_PI / 2.0, 0);
}

/* main game render function */
void dot_game_render(void)
{
	int i;
	ALLEGRO_COLOR player_color = t3f_color_white;
	float rgb = 0.5 - (float)dot_game.level / 24.0;
	
	al_clear_to_color(al_map_rgb_f(rgb, rgb, rgb));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < DOT_GAME_MAX_BALLS; i++)
	{
		if(dot_game.ball[i].active)
		{
			t3f_draw_bitmap(dot_bitmap[DOT_BITMAP_BALL_RED + dot_game.ball[i].type], t3f_color_white, dot_game.ball[i].x, dot_game.ball[i].y, dot_game.ball[i].z, 0);
		}
	}
	if(dot_game.player.active)
	{
		if(dot_game.state == DOT_GAME_STATE_START)
		{
			player_color = al_map_rgba_f(1.0 * 0.5, 1.0 * 0.5, 1.0 * 0.5, 0.5);
		}
		t3f_draw_bitmap(dot_bitmap[DOT_BITMAP_BALL_RED + dot_game.player.type], player_color, dot_game.player.x, dot_game.player.y, dot_game.player.z, 0);
		t3f_draw_rotated_bitmap(dot_bitmap[DOT_BITMAP_BALL_EYES], player_color, 8.0, 8.0, dot_game.player.x + 8.0, dot_game.player.y + 8.0, dot_game.player.z, dot_game.player.a, 0);
	}
	dot_game_render_hud();
	al_hold_bitmap_drawing(false);
}
