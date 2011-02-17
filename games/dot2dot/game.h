#ifndef DOT_GAME_H
#define DOT_GAME_H

typedef struct
{
	
	float x, y, z, a, s;
	float vx, vy;
	int type;
	int timer;
	bool active;
	
} DOT_BALL;

#define DOT_GAME_MAX_LEVELS   10
#define DOT_GAME_COMBO_TIME  120

#define DOT_GAME_MAX_BALLS   256
#define DOT_GAME_STATE_START   0
#define DOT_GAME_STATE_PLAY    1

typedef struct
{
	
	int state;
	int state_tick;
	int timer;
	
	/* player data */
	int lives;
	int level;
	int ascore, score, high_score;
	int combo;
	
	DOT_BALL ball[DOT_GAME_MAX_BALLS];
	DOT_BALL player;
	
} DOT_GAME;

void dot_game_initialize(void);
void dot_game_logic(void);
void dot_game_render_hud(void);
void dot_game_render(void);

#endif
