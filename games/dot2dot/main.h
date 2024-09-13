#ifndef DOT_MAIN_H
#define DOT_MAIN_H

#include "t3f/t3f.h"
#include "game.h"

#define DOT_MAX_BITMAPS      256
#define DOT_BITMAP_BALL_RED    0
#define DOT_BITMAP_BALL_GREEN  1
#define DOT_BITMAP_BALL_BLUE   2
#define DOT_BITMAP_BALL_PURPLE 3
#define DOT_BITMAP_BALL_YELLOW 4
#define DOT_BITMAP_BALL_ORANGE 5
#define DOT_BITMAP_BALL_BLACK  6
#define DOT_BITMAP_BALL_EYES   8
#define DOT_BITMAP_HUD         9

#define DOT_MAX_SAMPLES      256
#define DOT_SAMPLE_START       0
#define DOT_SAMPLE_GRAB        1
#define DOT_SAMPLE_LOSE        2
#define DOT_SAMPLE_GO          3

#define DOT_STATE_INTRO        0
#define DOT_STATE_GAME         1

extern T3F_BITMAP * dot_bitmap[DOT_MAX_BITMAPS];
extern ALLEGRO_SAMPLE * dot_sample[DOT_MAX_SAMPLES];
extern ALLEGRO_FONT * dot_font;
extern T3F_ATLAS * dot_atlas;
extern int dot_state;
extern int dot_tick;
extern DOT_GAME dot_game;

void dot_shadow_text(ALLEGRO_FONT * font, ALLEGRO_COLOR color, ALLEGRO_COLOR shadow, float x, float y, float sx, float sy, int flags, const char * text);

#endif
