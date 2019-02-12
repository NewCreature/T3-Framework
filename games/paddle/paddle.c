#include "t3f/t3f.h"
#include "t3f/music.h"
#include "t3f/gui.h"
#include "t3f/collision.h"
#include "t3f/rng.h"
#include "paddle.h"

/* program data (defines in "example.h") */
ALLEGRO_BITMAP * paddle_bitmap[EXAMPLE_MAX_BITMAPS] = {NULL};
ALLEGRO_FONT * paddle_font[EXAMPLE_MAX_FONTS] = {NULL};
ALLEGRO_SAMPLE * paddle_sample[EXAMPLE_MAX_SAMPLES] = {NULL};
T3F_GUI * paddle_menu = NULL;
T3F_RNG_STATE rng_state;

/* program state
   used to tell if we are playing, in the menu, etc. (defines in "example.h") */
int paddle_state = EXAMPLE_STATE_TITLE; // start out at title screen

/* in-game data */
PADDLE paddle[2];
BALL ball;
int score[2] = {0};

/* CPU predicts where it thinks the ball will end up so it can try to move there */
float paddle_ai_predict_y(void)
{
	float ox, oy, ovy;
	float dy = 0.0;

	ox = ball.x;
	oy = ball.y;
	ovy = ball.vy;
	while(ball.x < paddle[1].x - 16)
	{
		ball.x += ball.vx;
		ball.y += ball.vy;
		if(ball.y < 0.0)
		{
			ball.y = 0.0;
			ball.vy = -ball.vy;
		}
		else if(ball.y >= 480.0 - 16.0)
		{
			ball.y = 480.0 - 16.0 - 1.0;
			ball.vy = -ball.vy;
		}
	}
	dy = ball.y;
	ball.x = ox;
	ball.y = oy;
	ball.vy = ovy;
	return dy;
}

void paddle_init_ball(float dir)
{
	float angle = t3f_drand(&rng_state) * ALLEGRO_PI;
	ball.x = 320.0 - 16.0 / 2.0;
	ball.y = 240.0 - 16.0 / 2.0;
	t3f_move_collision_object_xy(ball.object, ball.x, ball.y);
	ball.vx = cos(angle) * EXAMPLE_START_SPEED * dir;
	ball.vy = sin(angle) * EXAMPLE_START_SPEED * dir;
	if(ball.vx > 0.0 && dir < 0.0)
	{
		ball.vx = -ball.vx;
	}
	else if(ball.vx < 0.0 && dir > 0.0)
	{
		ball.vx = -ball.vx;
	}
	ball.active = true;

	/* handle AI */
	if(ball.vx > 0)
	{
		paddle[1].dy = paddle_ai_predict_y() - 32 + 8.0;
		paddle[1].dvy = t3f_drand(&rng_state) * 6.0 - 3.0;
	}
}

void paddle_game_init(void)
{
	al_hide_mouse_cursor(t3f_display);

	/* place paddles */
	paddle[0].x = 32.0;
	paddle[0].y = 240.0 - 64.0 / 2.0;
	paddle[0].object = t3f_create_collision_object(0, 0, 16, 64, 32, 32, 0);
	t3f_move_collision_object_xy(paddle[0].object, paddle[0].x, paddle[0].y);
	paddle[0].active = true;
	paddle[1].x = 640.0 - 32.0 - 16.0;
	paddle[1].y = 240.0 - 64.0 / 2.0;
	paddle[1].object = t3f_create_collision_object(0, 0, 16, 64, 32, 32, 0);
	t3f_move_collision_object_xy(paddle[1].object, paddle[1].x, paddle[1].y);
	paddle[1].active = true;

	/* place ball */
	ball.object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
	paddle_init_ball(1.0);

	/* reset scores */
	score[0] = 0;
	score[1] = 0;

	t3f_srand(&rng_state, time(0));

	paddle_state = EXAMPLE_STATE_GAME;
	t3f_play_music("data/music/game.xm");
}

void paddle_game_exit(void)
{
	t3f_destroy_collision_object(paddle[0].object);
	t3f_destroy_collision_object(paddle[1].object);
	t3f_destroy_collision_object(ball.object);
	al_show_mouse_cursor(t3f_display);
	t3f_play_music("data/music/menu.xm");
	paddle_state = EXAMPLE_STATE_TITLE;
}

/* logic routines, passed to T^3 Framework */
void paddle_logic(void * data)
{
	/* logic switch, use different logic for each state */
	switch(paddle_state)
	{

		case EXAMPLE_STATE_TITLE:
		{
			t3f_process_gui(paddle_menu, data);
			break;
		}

		case EXAMPLE_STATE_GAME:
		{

			/* return to menu if Escape pressed */
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				paddle_game_exit();
			}

			/* store old paddle positions */
			paddle[0].oy = paddle[0].y;
			paddle[1].oy = paddle[1].y;

			/* move paddle */
			paddle[0].y = t3f_mouse_y - 32;
			if(paddle[0].y < 0)
			{
				paddle[0].y = 0;
			}
			if(paddle[0].y > 480.0 - 64.0)
			{
				paddle[0].y = 480.0 - 64.0;
			}
			t3f_move_collision_object_xy(paddle[0].object, paddle[0].x, paddle[0].y);

			/* move CPU paddle */
			if(ball.vx > 0.0)
			{
				if(ball.x > paddle[1].x - 24)
				{
					paddle[1].y += paddle[1].dvy;
				}
				else
				{
					if(paddle[1].y < paddle[1].dy - 2.0)
					{
						paddle[1].y += 2.0;
						if(paddle[1].y > 480.0 - 64.0)
						{
							paddle[1].y = 480.0 - 64.0;
						}
					}
					else if(paddle[1].y > paddle[1].dy + 2.0)
					{
						paddle[1].y -= 2.0;
						if(paddle[1].y < 0.0)
						{
							paddle[1].y = 0.0;
						}
					}
				}
				/* correct paddle position so it doesn't go past edge */
				if(paddle[1].y > 480.0 - 64.0)
				{
					paddle[1].y = 480.0 - 64.0;
				}
				if(paddle[1].y < 0.0)
				{
					paddle[1].y = 0.0;
				}
			}
			t3f_move_collision_object_xy(paddle[1].object, paddle[1].x, paddle[1].y);

			/* move ball */
			ball.x += ball.vx;
			t3f_move_collision_object_xy(ball.object, ball.x, ball.y);
			if(ball.vx < 0 && t3f_check_object_collision(paddle[0].object, ball.object))
			{
				al_play_sample(paddle_sample[EXAMPLE_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				ball.x = paddle[0].x + 16.0;
				ball.vx = -ball.vx;
				ball.vx += EXAMPLE_SPEED_INCREASE;
				ball.vy -= (paddle[0].oy - paddle[0].y) / 4.0;
				if(ball.vy < -3.0)
				{
					ball.vy = -3.0;
				}
				if(ball.vy > 3.0)
				{
					ball.vy = 3.0;
				}
				paddle[1].dy = paddle_ai_predict_y() - 32 + 8.0;
				paddle[1].dvy = t3f_drand(&rng_state) * 6.0 - 3.0;
			}
			else if(ball.vx > 0 && t3f_check_object_collision(paddle[1].object, ball.object))
			{
				al_play_sample(paddle_sample[EXAMPLE_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				ball.x = paddle[1].x - 16.0 - 1.0;
				ball.vx = -ball.vx;
				ball.vx -= EXAMPLE_SPEED_INCREASE;
				ball.vy -= (paddle[1].oy - paddle[1].y) / 4.0;
				if(ball.vy < -3.0)
				{
					ball.vy = -3.0;
				}
				if(ball.vy > 3.0)
				{
					ball.vy = 3.0;
				}
			}
			if(ball.x < -16.0)
			{
				al_play_sample(paddle_sample[EXAMPLE_SAMPLE_SCORE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				score[1]++;
				if(score[1] >= 5)
				{
					t3f_stop_music();
					paddle_state = EXAMPLE_STATE_GAME_OVER;
				}
				paddle_init_ball(1.0);
			}
			else if(ball.x > 640.0)
			{
				al_play_sample(paddle_sample[EXAMPLE_SAMPLE_SCORE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				score[0]++;
				if(score[0] >= 5)
				{
					t3f_stop_music();
					paddle_state = EXAMPLE_STATE_GAME_OVER;
				}
				paddle_init_ball(-1.0);
			}
			ball.y += ball.vy;
			if(ball.y < 0.0)
			{
				ball.y = 0.0;
				ball.vy = -ball.vy;
			}
			else if(ball.y > 480.0 - 16.0)
			{
				ball.y = 480.0 - 16.0;
				ball.vy = -ball.vy;
			}
			break;
		}

		case EXAMPLE_STATE_GAME_OVER:
		{

			/* return to menu if Escape pressed */
			if(t3f_mouse_button[0] || t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				paddle_game_exit();
			}

			break;
		}
	}
}

/* render routines, passed to T^3 Framework */
void paddle_render(void * data)
{
	/* render switch, render graphics according to which state we are in */
	switch(paddle_state)
	{

		case EXAMPLE_STATE_TITLE:
		{

			/* draw background */
			al_draw_bitmap(paddle_bitmap[EXAMPLE_BITMAP_BG], 0.0, 0.0, 0);

			/* center logo */
			al_draw_bitmap(paddle_bitmap[EXAMPLE_BITMAP_LOGO], al_get_display_width(t3f_display) / 2 - al_get_bitmap_width(paddle_bitmap[EXAMPLE_BITMAP_LOGO]) / 2, 32.0, 0);

			/* draw menu */
			t3f_render_gui(paddle_menu);

			break;
		}

		case EXAMPLE_STATE_GAME:
		{
			int i;

			/* draw background */
			al_draw_bitmap(paddle_bitmap[EXAMPLE_BITMAP_BG], 0.0, 0.0, 0);

			/* draw game objects */
			for(i = 0; i < 2; i++)
			{
				if(paddle[i].active)
				{
					al_draw_bitmap(paddle_bitmap[EXAMPLE_BITMAP_PADDLE], paddle[i].x, paddle[i].y, 0);
				}
				if(ball.active)
				{
					al_draw_bitmap(paddle_bitmap[EXAMPLE_BITMAP_BALL], ball.x, ball.y, 0);
				}
			}

			/* draw scores */
			al_draw_textf(paddle_font[EXAMPLE_FONT_GAME], al_map_rgba(0, 0, 0, 255), 10.0, 0.0, 0, "Player 1: %d", score[0]);
			al_draw_textf(paddle_font[EXAMPLE_FONT_GAME], al_map_rgba(0, 0, 0, 255), 540.0, 0.0, 0, "Player 2: %d", score[1]);

			break;
		}

		case EXAMPLE_STATE_GAME_OVER:
		{

			/* draw background */
			al_draw_bitmap(paddle_bitmap[EXAMPLE_BITMAP_BG], 0.0, 0.0, 0);

			/* draw results */
			al_draw_filled_circle(640.0 * t3f_drand(&rng_state), 480.0 * t3f_drand(&rng_state), 10.0 + 32.0 * t3f_drand(&rng_state), al_map_rgba(0, 0, 192, 128));
			al_draw_filled_rectangle(220.0, 192.0, 420.0, 280.0, al_map_rgba(0, 192, 0, 128));
			al_draw_rectangle(220.0, 192.0, 420.0, 280.0, al_map_rgba(0, 0, 0, 255), 2.0);
			al_draw_textf(paddle_font[EXAMPLE_FONT_MENU], al_map_rgba(0, 0, 0, 255), 320.0, 200.0, ALLEGRO_ALIGN_CENTRE, "Player %d Wins!", score[0] > score[1] ? 1 : 2);
			al_draw_textf(paddle_font[EXAMPLE_FONT_GAME], al_map_rgba(0, 0, 0, 255), 320.0, 240.0, ALLEGRO_ALIGN_CENTRE, "Click to continue...");

			break;
		}
	}
}

int paddle_menu_play_proc(void * d, int i, void * p)
{
	paddle_game_init();
	return 1;
}

int paddle_menu_quit_proc(void * d, int i, void * p)
{
	t3f_exit();
	return 1;
}

bool paddle_initialize(void)
{

	/* initialize the T^3 Framework
	   - create 640x480 display
	   - logic runs at 60.0 FPS, typical game refresh rate
	   - name the program "Paper Paddle" (displayed as window title when in windowed mode)
	   - use paddle_logic() and paddle_render() to handle logic and rendering
	   - use keyboard (T3F_USE_KEYBOARD), mouse (T3F_USE_MOUSE), sound (T3F_USE_SOUND), and fullscreen mode (T3F_USE_FULLSCREEN) */
	if(!t3f_initialize("Paper Paddle", 640, 480, 60.0, paddle_logic, paddle_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_SOUND, NULL))
	{
		printf("Failed to initialize T^3 Framework!\n");
		return false;
	}

	/* load bitmaps */
	paddle_bitmap[EXAMPLE_BITMAP_BG] = al_load_bitmap("data/graphics/bg.png");
	if(!paddle_bitmap[EXAMPLE_BITMAP_BG])
	{
		printf("Failed to load image!\n");
		return false;
	}
	paddle_bitmap[EXAMPLE_BITMAP_LOGO] = al_load_bitmap("data/graphics/logo.png");
	if(!paddle_bitmap[EXAMPLE_BITMAP_LOGO])
	{
		printf("Failed to load image!\n");
		return false;
	}
	paddle_bitmap[EXAMPLE_BITMAP_PADDLE] = al_load_bitmap("data/graphics/paddle.png");
	if(!paddle_bitmap[EXAMPLE_BITMAP_PADDLE])
	{
		printf("Failed to load image!\n");
		return false;
	}
	paddle_bitmap[EXAMPLE_BITMAP_BALL] = al_load_bitmap("data/graphics/ball.png");
	if(!paddle_bitmap[EXAMPLE_BITMAP_BALL])
	{
		printf("Failed to load image!\n");
		return false;
	}

	/* load fonts */
	paddle_font[EXAMPLE_FONT_MENU] = al_load_bitmap_font("data/fonts/title_font.png");
	if(!paddle_font[EXAMPLE_FONT_MENU])
	{
		printf("Failed to load font!\n");
		return false;
	}
	paddle_font[EXAMPLE_FONT_GAME] = al_load_bitmap_font("data/fonts/game_font.png");
	if(!paddle_font[EXAMPLE_FONT_GAME])
	{
		printf("Failed to load font!\n");
		return false;
	}

	/* load sound samples */
	paddle_sample[EXAMPLE_SAMPLE_HIT] = al_load_sample("data/sounds/hit.ogg");
	if(!paddle_sample[EXAMPLE_SAMPLE_HIT])
	{
		printf("Failed to load sample!\n");
		return false;
	}
	paddle_sample[EXAMPLE_SAMPLE_SCORE] = al_load_sample("data/sounds/score.ogg");
	if(!paddle_sample[EXAMPLE_SAMPLE_SCORE])
	{
		printf("Failed to load sample!\n");
		return false;
	}

	/* create GUI */
	t3f_set_gui_driver(NULL);
	paddle_menu = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(paddle_menu, paddle_menu_play_proc, "Play", (void **)&paddle_font[EXAMPLE_FONT_MENU], 320, 240, t3f_color_black, T3F_GUI_ELEMENT_CENTRE);
	t3f_add_gui_text_element(paddle_menu, paddle_menu_quit_proc, "Quit", (void **)&paddle_font[EXAMPLE_FONT_MENU], 320, 270, t3f_color_black, T3F_GUI_ELEMENT_CENTRE);

	return true;
}

int main(int argc, char * argv[])
{

	if(!paddle_initialize())
	{
		return 1;
	}

	/* play the menu music
	   - setting both loop points to 0.0 makes the entire song loop
	   - if your song has loop points, pass them here (paramaters are read as seconds) */
	t3f_play_music("data/music/menu.xm");

	/* run the T^3 Framework
	   - exits when you call t3f_exit() somewhere in your logic routine
	   - clicking the close button in windowed mode calls t3f_exit() */
	t3f_run();

	/* stop the music before exiting */
	t3f_stop_music();

	return 0;
}
