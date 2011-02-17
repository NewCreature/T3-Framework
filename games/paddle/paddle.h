#ifndef T3F_EXAMPLE_H
#define T3F_EXAMPLE_H

#include "t3f/collision.h"

/* defines for bitmaps */
#define EXAMPLE_MAX_BITMAPS  16
#define EXAMPLE_BITMAP_BG     0
#define EXAMPLE_BITMAP_LOGO   1
#define EXAMPLE_BITMAP_BALL   2
#define EXAMPLE_BITMAP_PADDLE 3

/* defines for fonts */
#define EXAMPLE_MAX_FONTS     8
#define EXAMPLE_FONT_MENU     0
#define EXAMPLE_FONT_GAME     1

/* defines for samples (sounds) */
#define EXAMPLE_MAX_SAMPLES   8
#define EXAMPLE_SAMPLE_HIT    0
#define EXAMPLE_SAMPLE_SCORE  1

/* defines for program states */
#define EXAMPLE_STATE_TITLE     0
#define EXAMPLE_STATE_GAME      1
#define EXAMPLE_STATE_GAME_OVER 2

/* defines for gameplay */
#define EXAMPLE_START_SPEED    3.0
#define EXAMPLE_SPEED_INCREASE 0.2

typedef struct
{
	
	float x;
	float y;
	float ox;
	float oy;
	float dy;  // destination for AI paddle
	float dvy; // what the AI is going to do when the ball reaches it
	
	T3F_COLLISION_OBJECT * object;
	
	bool active;
	
} PADDLE;

typedef struct
{
	
	float x;
	float y;
	float vx;
	float vy;
	
	T3F_COLLISION_OBJECT * object;
	
	bool active;
	
} BALL;

#endif
