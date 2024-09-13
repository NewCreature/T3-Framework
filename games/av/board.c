#include "includes.h"

void generate_board_layout(GAME * gp)
{
	int rn;
	ALLEGRO_STATE old_state;
	
	rn = rand() % 3;
	rn = 1;
	if(gp->board.layout)
	{
		t3f_destroy_bitmap(gp->board.layout);
	}
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	switch(rn)
	{
		case 0:
		{
			gp->board.layout = t3f_load_bitmap("data/graphics/layout1.pcx", 0, false);
			break;
		}
		case 1:
		{
			gp->board.layout = t3f_load_bitmap("data/graphics/layout2.pcx", 0, false);
			break;
		}
		case 2:
		{
			gp->board.layout = t3f_load_bitmap("data/graphics/layout3.pcx", 0, false);
			break;
		}
	}
	al_convert_mask_to_alpha(gp->board.layout->bitmap, al_map_rgb(255, 0, 255));
	al_restore_state(&old_state);
}

void clear_board(GAME * gp)
{
    int i, j;

    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            gp->board.data[i][j] = 0;
            gp->board.color_data[i][j] = 0;
            gp->board.match_data[i][j] = 0;
        }
    }
}

typedef struct
{
	
	int x, y, r;
	
} BOARD_POSITION;

void board_add_virus(GAME * gp)
{
	int i, j;
    BOARD_POSITION position[BOARD_RX * BOARD_RY];
    int pcount = 0;
    int rn;
    int top;
    ALLEGRO_COLOR pixel;
    unsigned char r, g, b, a;
    int new_type = 0;
    
	if(gp->level < 3)
	{
		top = 9;
	}
	else if(gp->level < 5)
	{
		top = 8;
	}
	else if(gp->level < 8)
	{
		top = 7;
	}
	else if(gp->level < 11)
	{
		top = 6;
	}
	else if(gp->level < 14)
	{
		top = 5;
	}
	else
	{
		top = 3;
	}
	for(i = 1; i < BOARD_RY - top; i++)
	{
		for(j = 0; j < BOARD_RX; j++)
		{
			pixel = al_get_pixel(gp->board.layout->bitmap, j, i + top - 1);
			al_unmap_rgba(pixel, &r, &g, &b, &a);
			if(gp->board.data[i + top][j] == 0 && a)
			{
				position[pcount].x = j;
				position[pcount].y = i + top;
				position[pcount].r = r;
				pcount++;
			}
		}
	}
	if(pcount > 0)
	{
		rn = rand() % pcount;
		switch(position[rn].r)
		{
			case 129:
			{
				new_type = 18;
				break;
			}
			case 137:
			{
				new_type = 17;
				break;
			}
			case 145:
			{
				new_type = 16;
				break;
			}
		}
		gp->board.data[position[rn].y][position[rn].x] = new_type;
	}
}

int board_cleared(BOARD * bp)
{
    int i, j;
    int cleared = 1;

    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            if(bp->data[i][j] == 16 || bp->data[i][j] == 17 || bp->data[i][j] == 18)
            {
                cleared = 0;
            }
        }
    }
    return cleared;
}

int block_color(char data[BOARD_RY][BOARD_RX], int xpos, int ypos)
{
    if(data[ypos][xpos] == 0)
    {
        return 0;
    }
    else
    {
        if(data[ypos][xpos] > 0 && data[ypos][xpos] < 6)
        {
            return 1;
        }
        if(data[ypos][xpos] > 5 && data[ypos][xpos] < 11)
        {
            return 2;
        }
        if(data[ypos][xpos] > 10 && data[ypos][xpos] < 16)
        {
            return 3;
        }
        if(data[ypos][xpos] == 16)
        {
            return 1;
        }
        if(data[ypos][xpos] == 17)
        {
            return 2;
        }
        if(data[ypos][xpos] == 18)
        {
            return 3;
        }
    }
    return 0;
}

/* use the board data to generate color matching data */
void generate_color_map(BOARD * bp)
{
    int i, j;

    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            bp->color_data[i][j] = block_color(bp->data, j, i);
        }
    }
}

//int block_broken(GAME * gp, int bx, int by)
int block_broken(char data[BOARD_RY][BOARD_RX], int bx, int by)
{
    switch(data[by][bx])
    {
        case 2:
        {
            if(data[by][bx + 1] != 4 && data[by][bx + 1] != 9 && data[by][bx + 1] != 14)
            {
                return 1;
            }
            return 0;
        }
        case 3:
        {
            if(data[by + 1][bx] != 5 && data[by + 1][bx] != 10 && data[by + 1][bx] != 15)
            {
                return 1;
            }
            return 0;
        }
        case 4:
        {
            if(data[by][bx - 1] != 2 && data[by][bx - 1] != 7 && data[by][bx - 1] != 12)
            {
                return 1;
            }
            return 0;
        }
        case 5:
        {
            if(data[by - 1][bx] != 3 && data[by - 1][bx] != 8 && data[by - 1][bx] != 13)
            {
                return 1;
            }
            return 0;
        }
        case 7:
        {
            if(data[by][bx + 1] != 4 && data[by][bx + 1] != 9 && data[by][bx + 1] != 14)
            {
                return 1;
            }
            return 0;
        }
        case 8:
        {
            if(data[by + 1][bx] != 5 && data[by + 1][bx] != 10 && data[by + 1][bx] != 15)
            {
                return 1;
            }
            return 0;
        }
        case 9:
        {
            if(data[by][bx - 1] != 2 && data[by][bx - 1] != 7 && data[by][bx - 1] != 12)
            {
                return 1;
            }
            return 0;
        }
        case 10:
        {
            if(data[by - 1][bx] != 3 && data[by - 1][bx] != 8 && data[by - 1][bx] != 13)
            {
                return 1;
            }
            return 0;
        }
        case 12:
        {
            if(data[by][bx + 1] != 4 && data[by][bx + 1] != 9 && data[by][bx + 1] != 14)
            {
                return 1;
            }
            return 0;
        }
        case 13:
        {
            if(data[by + 1][bx] != 5 && data[by + 1][bx] != 10 && data[by + 1][bx] != 15)
            {
                return 1;
            }
            return 0;
        }
        case 14:
        {
            if(data[by][bx - 1] != 2 && data[by][bx - 1] != 7 && data[by][bx - 1] != 12)
            {
                return 1;
            }
            return 0;
        }
        case 15:
        {
            if(data[by - 1][bx] != 3 && data[by - 1][bx] != 8 && data[by - 1][bx] != 13)
            {
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

/* change broken pills into half-pills */
void update_board(GAME * gp)
{
    int i, j;

    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            if(block_broken(gp->board.data, j, i))
            {
                gp->board.data[i][j] = ((block_color(gp->board.data, j, i)) - 1) * 5 + 1;
            }
            if(block_broken(gp->board.match_data, j, i))
            {
                gp->board.match_data[i][j] = ((block_color(gp->board.match_data, j, i)) - 1) * 5 + 1;
            }
        }
    }
}

int broken_pill(GAME * gp, int bx, int by)
{
    if(gp->board.data[by][bx] == 1 || gp->board.data[by][bx] == 6 || gp->board.data[by][bx] == 11)
    {
        return 1;
    }
    return 0;
}

int left_pill(GAME * gp, int bx, int by)
{
    if(gp->board.data[by][bx] == 2 || gp->board.data[by][bx] == 7 || gp->board.data[by][bx] == 12)
    {
        return 1;
    }
    return 0;
}

int right_pill(GAME * gp, int bx, int by)
{
    if(gp->board.data[by][bx] == 4 || gp->board.data[by][bx] == 9 || gp->board.data[by][bx] == 14)
    {
        return 1;
    }
    return 0;
}

int top_pill(GAME * gp, int bx, int by)
{
    if(gp->board.data[by][bx] == 3 || gp->board.data[by][bx] == 8 || gp->board.data[by][bx] == 13)
    {
        return 1;
    }
    return 0;
}

int bottom_pill(GAME * gp, int bx, int by)
{
    if(gp->board.data[by][bx] == 5 || gp->board.data[by][bx] == 10 || gp->board.data[by][bx] == 15)
    {
        return 1;
    }
    return 0;
}

/* make unsupported blocks fall */
void blocks_fall(GAME * gp)
{
    int i, j;
    int done = 1;

    for(i = BOARD_RY - 1; i > 0; i--)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            /* check broken pills (easy part) */
            if(!gp->board.data[i][j] && broken_pill(gp, j, i - 1))
            {
				gp->board.data[i][j] = gp->board.data[i - 1][j];
				gp->board.data[i - 1][j] = 0;
				done = 0;
            }
            
            /* check unbroken pills */
            else if(j < BOARD_RX - 1 && !gp->board.data[i][j] && !gp->board.data[i][j + 1] && left_pill(gp, j, i - 1) && right_pill(gp, j + 1, i - 1))
            {
				gp->board.data[i][j] = gp->board.data[i - 1][j];
				gp->board.data[i - 1][j] = 0;
				gp->board.data[i][j + 1] = gp->board.data[i - 1][j + 1];
				gp->board.data[i - 1][j + 1] = 0;
				done = 0;
            }
            else if(!gp->board.data[i][j] && bottom_pill(gp, j, i - 1) && top_pill(gp, j, i - 2))
            {
				gp->board.data[i][j] = gp->board.data[i - 1][j];
				gp->board.data[i - 1][j] = gp->board.data[i - 2][j];
				gp->board.data[i - 2][j] = 0;
				done = 0;
            }
        }
    }
    if(done)
    {
        gp->state = GAME_STATE_FIND;
        gp->timer = 0;
    }
    else
    {
		al_play_sample(gp->tp->sound[THEME_SOUND_LAND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
}

/* helper for find_runs() below */
int find_across(GAME * gp, int bx, int by)
{
    int test_color; /* color we're looking for */
    int matched;    /* how many have been matched */
    int i;

    /* get ready to check */
    test_color = gp->board.color_data[by][bx];
    matched = 1;

    /* check from the current position to the edge of the map */
    for(i = bx + 1; i < BOARD_RX; i++)
    {
        /* if we like the color, add it to tally */
        if(gp->board.color_data[by][i] == test_color)
        {
            matched++;
        }

        /* otherwise break from the loop */
        else
        {
            break;
        }
    }

    /* if we have matches mark them (matched tally >= 4) */
    if(matched >= 4)
    {
        for(i = bx; i < bx + matched; i++)
        {
            gp->board.match_data[by][i] = gp->board.data[by][i];
        }
        gp->state = GAME_STATE_MATCHES;
        gp->timer = 0;
        return matched;
    }
    return 0;
}

/* helper for find_runs() below */
int find_down(GAME * gp, int bx, int by)
{
    int test_color; /* color we're looking for */
    int matched;    /* how many have been matched */
    int i;

    /* get ready to check */
    test_color = gp->board.color_data[by][bx];
    matched = 1;

    /* check from the current position to the edge of the map */
    for(i = by + 1; i < BOARD_RY; i++)
    {
        /* if we like the color, add it to tally */
        if(gp->board.color_data[i][bx] == test_color)
        {
            matched++;
        }

        /* otherwise break from the loop */
        else
        {
            break;
        }
    }

    /* if we have matches mark them (matched tally >= 4) */
    if(matched >= 4)
    {
        for(i = by; i < by + matched; i++)
        {
            gp->board.match_data[i][bx] = gp->board.data[i][bx];
        }
        gp->state = GAME_STATE_MATCHES;
        gp->timer = 0;
        return matched;
    }
    return 0;
}

/* find color matches */
int find_runs(GAME * gp)
{
    int i, j, t = 0;

    /* so we know what colors are currently on the board */
    gp->total = 0;
    generate_color_map(&gp->board);
    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            if(gp->board.data[i][j])
            {
                t += find_across(gp, j, i);
                t += find_down(gp, j, i);
                if(gp->state != GAME_STATE_MATCHES)
                {
                    gp->state = GAME_STATE_PLAY;
                }
            }
        }
    }
    return t;
}

/* deletes a single block and drops others appropriately */
void delete_block(BOARD * bp, int bx, int by)
{
    bp->data[by][bx] = 0;
}

/* delete the matches */
void delete_runs(GAME * gp)
{
    int i, j;

	gp->total = 0;
    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
            if(gp->board.match_data[i][j])
            {
	            if(gp->board.data[i][j] >= 16)
	            {
		            gp->total++;
	            }
                delete_block(&gp->board, j, i);
            }
        }
    }
    if(gp->total > 0)
    {
	    gp->combo++;
		al_play_sample(gp->tp->sound[THEME_SOUND_KILL], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    else
    {
		al_play_sample(gp->tp->sound[THEME_SOUND_MATCH], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    switch(gp->difficulty)
    {
	    case 0:
	    {
    		gp->score += gp->total * 100 * gp->combo;
    		break;
		}
	    case 1:
	    {
    		gp->score += gp->total * 200 * gp->combo;
    		break;
		}
	    case 2:
	    {
    		gp->score += gp->total * 400 * gp->combo;
    		break;
		}
	}
	if(gp->score > gp->high_score)
	{
		gp->high_score = gp->score;
	}
}
