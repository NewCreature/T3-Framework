#ifndef TYPES_H
#define TYPES_H

#include "includes.h"

#define BOARD_RX      8
#define BOARD_RY     17
#define BOTTLE_X_1P  144
#define BOTTLE_Y_1P  96

typedef struct
{
	
    char data[BOARD_RY][BOARD_RX];
    char color_data[BOARD_RY][BOARD_RX];
    char match_data[BOARD_RY][BOARD_RX];
    T3F_BITMAP * layout;
    
} BOARD;

typedef struct
{
	
    char data[2][2]; /* pill data */
    int x, y;        /* pill location */
    char active;     /* active flag */
    
} PILL;

#define THEME_BLOCK_TYPES 19
#define THEME_SOUND_TYPES  7

#define THEME_SOUND_MOVE   0
#define THEME_SOUND_ROTATE 1
#define THEME_SOUND_LAND   2
#define THEME_SOUND_MATCH  3
#define THEME_SOUND_KILL   4
#define THEME_SOUND_LEVEL  5
#define THEME_SOUND_PAUSE  6

typedef struct
{
	
    T3F_BITMAP * backdrop;
    T3F_BITMAP * block[THEME_BLOCK_TYPES];
    T3F_ANIMATION * virus[3];
    ALLEGRO_SAMPLE * sound[THEME_SOUND_TYPES];
    
} THEME;

typedef struct
{
	
    int left;
    int right;
    int rleft;
    int rright;
    int drop;

	int is_left;
    int was_left;
    int is_right;
    int was_right;
    int was_rleft;
    int was_rright;
    int was_drop;
    int hdrop;
    
} CONTROLLER;

/* game structure, use one for each player */
typedef struct
{
	
	THEME *    tp;
	
    PILL       pill;
    PILL       next_pill;
    BOARD      board;
    CONTROLLER controller;
	T3F_BITMAP *   preview;
	float      preview_angle;
	float      preview_vangle;
    int        state;
    int        state_frame;
    int        level;
    int        timer;
    int        score;
    int        high_score;
    int        total;
    int        combo;
    int        level_time;
    int        ticker;
    
    int difficulty;
    bool aid;
    int frame;
    bool over;

    int x, y; /* so we know where to draw */
    
} GAME;

#endif
