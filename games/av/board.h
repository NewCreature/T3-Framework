#ifndef BOARD_H
#define BOARD_H

#include "includes.h"

void generate_board_layout(GAME * gp);
void clear_board(GAME * gp);
void board_add_virus(GAME * gp);
void generate_board(GAME * gp, int vcount);
int find_runs(GAME * gp);
void delete_runs(GAME * gp);
void blocks_fall(GAME * gp);
int board_cleared(BOARD * bp);
void update_board(GAME * gp);

#endif
