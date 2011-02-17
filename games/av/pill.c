#include "includes.h"

int pill_color(PILL * pp, int xpos, int ypos)
{
    if(pp->data[ypos][xpos] == 0)
    {
        return 0;
    }
    else
    {
        if(pp->data[ypos][xpos] > 0 && pp->data[ypos][xpos] < 6)
        {
            return 1;
        }
        if(pp->data[ypos][xpos] > 5 && pp->data[ypos][xpos] < 11)
        {
            return 2;
        }
        if(pp->data[ypos][xpos] > 10 && pp->data[ypos][xpos] < 16)
        {
            return 3;
        }
    }
    return 0;
}

int pill_top(int color)
{
    return (color - 1) * 5 + 1 + 2;
}

int pill_bottom(int color)
{
    return (color - 1) * 5 + 1 + 4;
}

int pill_left(int color)
{
    return (color - 1) * 5 + 1 + 1;
}

int pill_right(int color)
{
    return (color - 1) * 5 + 1 + 3;
}

int pick_pill(PILL * pp, int color, int xpos, int ypos)
{
    if(color == 0)
    {
        return 0;
    }
    else
    {
        if(xpos == 0 && ypos == 0)
        {
            return 0;
        }
        else
        {
            if(ypos == 1 && xpos == 0)
            {
                return (color - 1) * 5 + 1 + 1;
            }
            if(ypos == 0 && xpos == 1)
            {
                return (color - 1) * 5 + 1 + 2;
            }
            if(ypos == 1 && xpos == 1)
            {
                if(pp->data[1][0])
                {
                    return (color - 1) * 5 + 1 + 3;
                }
                else
                {
                    return (color - 1) * 5 + 1 + 4;
                }
            }
        }
    }
    return 0;
}

/* rotate a pill left */
void lrotate_pill(GAME * gp)
{
	/* see if pill is horizontally aligned */
    if(gp->pill.data[1][0] != 0)
    {
	    
	    /* regular rotate, no adjustment */
	    if(gp->pill.y < 0 || (!gp->board.data[gp->pill.y][gp->pill.x + 1]))
	    {
	        gp->pill.data[0][1] = pill_top(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][1] = pill_bottom(pill_color(&gp->pill, 0, 1));
        	gp->pill.data[1][0] = 0;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    }
	    
	    /* bump against something above */
	    else if(gp->pill.y < BOARD_RY - 2 && gp->board.data[gp->pill.y][gp->pill.x + 1] && !gp->board.data[gp->pill.y + 2][gp->pill.x]&& !gp->board.data[gp->pill.y + 2][gp->pill.x + 1])
	    {
	        gp->pill.data[0][1] = pill_top(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][1] = pill_bottom(pill_color(&gp->pill, 0, 1));
        	gp->pill.data[1][0] = 0;
        	gp->pill.y += 1;
        	gp->timer = 0;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    }
	}
    /* otherwise it's vertical */
    else
    {
	    /* bump against left wall */
	    if(gp->pill.x < 0 && !gp->board.data[gp->pill.y + 1][gp->pill.x + 1])
	    {
	        gp->pill.data[1][0] = pill_left(pill_color(&gp->pill, 1, 0));
        	gp->pill.data[1][1] = pill_right(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[0][1] = 0;
        	gp->pill.x += 1;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    }
	    
	    /* regular rotate, no adjustment */
		else if(gp->pill.y < 0 || (!gp->board.data[gp->pill.y][gp->pill.x] && !gp->board.data[gp->pill.y + 1][gp->pill.x]))
		{
	        gp->pill.data[1][0] = pill_left(pill_color(&gp->pill, 1, 0));
        	gp->pill.data[1][1] = pill_right(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[0][1] = 0;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		
		/* bump against something on the left */
		else if(gp->pill.x < BOARD_RX - 2 && !gp->board.data[gp->pill.y + 1][gp->pill.x + 2])
		{
	        gp->pill.data[1][0] = pill_left(pill_color(&gp->pill, 1, 0));
        	gp->pill.data[1][1] = pill_right(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[0][1] = 0;
        	gp->pill.x += 1;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
	}
}

/* rotate a pill right */
void rrotate_pill(GAME * gp)
{
	/* see if pill is horizontally aligned */
    if(gp->pill.data[1][0] != 0)
    {
	    
	    /* regular rotate, no adjustment */
	    if(gp->pill.y < 0 || (!gp->board.data[gp->pill.y][gp->pill.x] && !gp->board.data[gp->pill.y][gp->pill.x + 1]))
	    {
	        gp->pill.data[0][1] = pill_top(pill_color(&gp->pill, 0, 1));
        	gp->pill.data[1][1] = pill_bottom(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][0] = 0;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    }
	    
	    /* bump against something above */
	    else if(gp->pill.y < BOARD_RY - 2 && (gp->board.data[gp->pill.y][gp->pill.x] || gp->board.data[gp->pill.y][gp->pill.x + 1]) && !gp->board.data[gp->pill.y + 2][gp->pill.x + 1] && !gp->board.data[gp->pill.y + 2][gp->pill.x])
	    {
	        gp->pill.data[0][1] = pill_top(pill_color(&gp->pill, 0, 1));
        	gp->pill.data[1][1] = pill_bottom(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][0] = 0;
        	gp->pill.y += 1;
        	gp->timer = 0;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    }
	}
    /* otherwise it's vertical */
    else
    {
	    /* bump against left wall */
	    if(gp->pill.x < 0 && !gp->board.data[gp->pill.y + 1][gp->pill.x + 1])
	    {
	        gp->pill.data[1][0] = pill_left(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][1] = pill_right(pill_color(&gp->pill, 1, 0));
        	gp->pill.data[0][1] = 0;
        	gp->pill.x += 1;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	    }
	    
	    /* regular rotate, no adjustment */
		else if(gp->pill.y < 0 || (!gp->board.data[gp->pill.y + 1][gp->pill.x]))
		{
	        gp->pill.data[1][0] = pill_left(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][1] = pill_right(pill_color(&gp->pill, 1, 0));
        	gp->pill.data[0][1] = 0;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		
		/* bump against something on the left */
		else if(gp->pill.x < BOARD_RX - 2 && !gp->board.data[gp->pill.y + 1][gp->pill.x + 2])
		{
	        gp->pill.data[1][0] = pill_left(pill_color(&gp->pill, 1, 1));
        	gp->pill.data[1][1] = pill_right(pill_color(&gp->pill, 1, 0));
        	gp->pill.data[0][1] = 0;
        	gp->pill.x += 1;
			al_play_sample(gp->tp->sound[THEME_SOUND_ROTATE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
	}
}

/* place pill on the board */
void place_pill(GAME * gp)
{
    int i, j;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            if(gp->pill.data[i][j])
            {
                gp->board.data[gp->pill.y + i][gp->pill.x + j] = gp->pill.data[i][j];
            }
        }
    }
    gp->pill.active = 0;
}

void generate_pill(GAME * gp)
{
	gp->pill.data[0][0] = gp->next_pill.data[0][0];
	gp->pill.data[0][1] = gp->next_pill.data[0][1];
	gp->pill.data[1][0] = gp->next_pill.data[1][0];
	gp->pill.data[1][1] = gp->next_pill.data[1][1];
    gp->pill.x = BOARD_RX / 2 - 1;
    gp->pill.y = 0;
    gp->next_pill.data[0][0] = 0;
    gp->next_pill.data[0][1] = 0;
    gp->next_pill.data[1][0] = pill_left(rand() % 3 + 1);
    gp->next_pill.data[1][1] = pill_right(rand() % 3 + 1);
    gp->pill.active = 1;
}

/* move pill */
int move_pill(GAME * gp, int dx, int dy)
{
    int ox = gp->pill.x;
    int oy = gp->pill.y;
    int i, j;

    /* moving left */
    if(dx < 0)
    {
        /* make sure we're not at the edge */
        if(gp->pill.x > -1)
        {
            /* move the pill */
            gp->pill.x += dx;

            /* if we hit something, move back */
            if(gp->pill.data[1][0] && gp->pill.x == -1)
            {
                gp->pill.x = ox;
            }
            else
            {
	            if(gp->pill.y < 0)
	            {
                    for(j = 0; j < 2; j++)
                   	{
                        if(gp->pill.data[1][j] && gp->board.data[gp->pill.y + 1][gp->pill.x + j])
                       	{
                            gp->pill.x = ox;
                       	}
                   	}
	            }
	            else
	            {
                	for(i = 0; i < 2; i++)
                	{
	                    for(j = 0; j < 2; j++)
                    	{
	                        if(gp->pill.data[i][j] && gp->board.data[gp->pill.y + i][gp->pill.x + j])
                        	{
	                            gp->pill.x = ox;
                        	}
                    	}
                	}
            	}
            }
        }
        if(gp->pill.x != ox)
        {
			al_play_sample(gp->tp->sound[THEME_SOUND_MOVE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
        else
        {
	        if(gp->controller.is_left < REPEAT_DELAY)
	        {
	        	gp->controller.was_left = 0;
        	}
        	else
        	{
	        	gp->controller.was_left = REPEAT_DELAY;
        	}
        }
    }

    /* moving right */
    if(dx > 0)
    {
        /* make sure we're not at the edge */
        if(gp->pill.x < BOARD_RX - 2)
        {
            /* move the pill */
            gp->pill.x += dx;

			if(gp->pill.y < 0)
			{
                for(j = 0; j < 2; j++)
               	{
                    if(gp->pill.data[1][j] && gp->board.data[gp->pill.y + 1][gp->pill.x + j])
                   	{
                        gp->pill.x = ox;
                   	}
               	}
			}
			else
			{
				for(i = 0; i < 2; i++)
            	{
	                for(j = 0; j < 2; j++)
                	{
	                    if(gp->pill.data[i][j] && gp->board.data[gp->pill.y + i][gp->pill.x + j])
                    	{
	                        gp->pill.x = ox;
                    	}
                	}
            	}
        	}
        }
        if(gp->pill.x != ox)
        {
			al_play_sample(gp->tp->sound[THEME_SOUND_MOVE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
        else
        {
	        if(gp->controller.is_right < REPEAT_DELAY)
	        {
	        	gp->controller.was_right = 0;
        	}
        	else
        	{
	        	gp->controller.was_right = REPEAT_DELAY;
        	}
        }
    }

    /* moving down */
    if(dy > 0)
    {
        /* make sure we're not at the edge */
        if(gp->pill.y < BOARD_RY - 2)
        {
            /* move the pill */
            gp->pill.y += dy;

            for(i = 0; i < 2; i++)
            {
                for(j = 0; j < 2; j++)
                {
                    if(gp->pill.data[i][j] && gp->board.data[gp->pill.y + i][gp->pill.x + j])
                    {
                        gp->pill.y = oy;
			            if(gp->pill.y < 0 && gp->pill.data[0][1])
			            {
				            gp->state = GAME_STATE_OVER;
	            			return 0;
            			}
                        place_pill(gp);
	                    al_play_sample(gp->tp->sound[THEME_SOUND_LAND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        gp->state = GAME_STATE_FIND;
                        gp->controller.hdrop = 1;
                        gp->timer = 0;
                        break;
                    }
                }
            }
        }

        /* if we are at edge, update board and make new pill */
        else
        {
            place_pill(gp);
			al_play_sample(gp->tp->sound[THEME_SOUND_LAND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            gp->state = GAME_STATE_FIND;
			gp->controller.hdrop = 1;
			gp->timer = 0;
        }
    }
    return 1;
}
