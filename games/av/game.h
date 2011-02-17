#ifndef GAME_H
#define GAME_H

#include "includes.h"

#define GAME_STATE_INTRO   0
#define GAME_STATE_PLAY    1
#define GAME_STATE_FIND    2
#define GAME_STATE_MATCHES 3
#define GAME_STATE_KILL    4
#define GAME_STATE_FALL    5
#define GAME_STATE_OVER    6
#define GAME_STATE_LEVEL   7
#define GAME_STATE_TITLE   8

void initialize_game(GAME * gp, int param);
int do_game(GAME * gp);
void draw_game(GAME * gp, THEME * tp);

#endif
