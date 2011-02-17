#ifndef PILL_H
#define PILL_H

#include "includes.h"

void lrotate_pill(GAME * gp);
void rrotate_pill(GAME * gp);
void place_pill(GAME * gp);
int move_pill(GAME * gp, int dx, int dy);
void generate_pill(GAME * gp);

#endif
