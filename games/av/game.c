#include "includes.h"

/* prepares a game for play */
void initialize_game(GAME * gp, int param)
{
	srand(time(0));
    gp->pill.x = BOARD_RX / 2;
    gp->pill.y = 0;
    gp->pill.data[0][0] = 0;
    gp->pill.data[0][1] = 0;
    gp->pill.data[1][0] = rand() % 3 + 1;
    gp->pill.data[1][1] = rand() % 3 + 1;
    gp->pill.active = 0;
    clear_board(gp);
    if(!param)
    {
		gp->state = GAME_STATE_TITLE;
	}
	else
	{
		gp->pill.active = 1;
		t3f_play_music("data/music/bgm.xm");
		gp->state = GAME_STATE_INTRO;
	}
    gp->timer = 0;
    gp->score = 0;
    gp->combo = 0;
    gp->preview_vangle = 0.005;
}

void exit_game(GAME * gp)
{
	char buf[256] = {0};
	
	t3f_stop_music();
	
	/* save high score */
	sprintf(buf, "%d", gp->high_score);
	al_set_config_value(t3f_config, "Game Data", "High Score", buf);
	
	gp->state = GAME_STATE_OVER;
}

/* do game logic */
int do_game(GAME * gp)
{
	int i, j;
	
	if(gp->over)
	{
		return 0;
	}
	gp->level_time++;
	if(gp->state != GAME_STATE_INTRO && gp->state != GAME_STATE_LEVEL && gp->state != GAME_STATE_OVER)
	{
		if(gp->level_time % 2000 == 0)
		{
			if(gp->ticker > 12)
			{
				gp->timer -= 2;
				gp->ticker -= 2;
				gp->level_time = 0;
				al_play_sample(gp->tp->sound[THEME_SOUND_LEVEL], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else if(gp->ticker > 7)
			{
				gp->timer -= 1;
				gp->ticker -= 1;
				gp->level_time = 0;
				al_play_sample(gp->tp->sound[THEME_SOUND_LEVEL], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
		}
	}
	gp->preview_angle += cos((double)(gp->frame) / 100.0) * 0.005;
	if(gp->preview_angle >= ALLEGRO_PI * 2)
	{
		gp->preview_angle -= ALLEGRO_PI * 2;
	}
    switch(gp->state)
    {

		/* the "title" screen */
		case GAME_STATE_TITLE:
		{
			if(gp->controller.rleft || gp->controller.rright)
			{
				gp->pill.active = 1;
				t3f_play_music("data/music/bgm.xm");
				gp->state = GAME_STATE_INTRO;
			}
			break;
		}

        /* do this first */
        case GAME_STATE_INTRO:
        {
            gp->timer++;
	        if(gp->timer == 1)
	        {
		        generate_board_layout(gp);
	            clear_board(gp);
	            generate_pill(gp);
				generate_pill(gp);
	            gp->pill.active = 0;
	        }
	        else if(gp->timer % 5 == 0)
	        {
		        board_add_virus(gp);
	        }
	        if(gp->timer >= gp->level * 4 * 5 + 4)
	        {
	            gp->state = GAME_STATE_PLAY;
	            switch(gp->difficulty)
            	{
		            case 0:
	            	{
	            		gp->ticker = 70;
		            	break;
	            	}
	            	case 1:
	            	{
	            		gp->ticker = 36;
            			break;
        			}
        			case 2:
        			{
	            		gp->ticker = 26;
            			break;
        			}
    			}
            	gp->level_time = 0;
	        }
            gp->preview_angle = 0;
            gp->frame = 0;
            break;
        }

        /* normal play */
        case GAME_STATE_PLAY:
        {
            if(!gp->pill.active)
            {
                generate_pill(gp);
                gp->timer = 0;
                if(gp->controller.was_left > REPEAT_DELAY)
                {
	                gp->controller.was_left = REPEAT_DELAY;
                }
                else if(gp->controller.was_right > REPEAT_DELAY)
                {
	                gp->controller.was_right = REPEAT_DELAY;
                }
            }
            else
            {
                if(gp->controller.left)
                {
                    move_pill(gp, -1, 0);
                }
                if(gp->controller.right)
                {
                    move_pill(gp, 1, 0);
                }
                if(gp->controller.drop)
                {
                    if(!move_pill(gp, 0, 1))
                    {
	                    return 0;
                    }
                    gp->timer = 0;
                }
                if(gp->controller.rleft)
                {
                    lrotate_pill(gp);
                }
                if(gp->controller.rright)
                {
                    rrotate_pill(gp);
                }
            }
            gp->timer++;
            if(gp->timer % gp->ticker == 0)
            {
				if(!move_pill(gp, 0, 1))
				{
					return 0;
				}
            }
            break;
        }

        /* search for matches */
        case GAME_STATE_FIND:
        {
	        gp->timer++;
	        if(gp->timer >= 50)
	        {
            	if(!find_runs(gp))
            	{
	            	gp->combo = 0;
					if(gp->board.data[1][3] || gp->board.data[1][4])
					{
						exit_game(gp);
						return 0;
					}
					for(i = 0; i < BOARD_RX; i++)
					{
						if(gp->board.data[0][i])
						{
							exit_game(gp);
							return 0;
						}
					}
            	}
        	}
            break;
        }

        /* do whatever when matches found */
        case GAME_STATE_MATCHES:
        {
	        if(gp->timer == 0)
	        {
            	gp->pill.active = 0;
            	delete_runs(gp);
                update_board(gp);
	        }
	        gp->timer += 8;
	        if(gp->timer >= 256)
	        {
			    for(i = 0; i < BOARD_RY; i++)
    			{
        			for(j = 0; j < BOARD_RX; j++)
        			{
            			if(gp->board.match_data[i][j])
            			{
                			gp->board.match_data[i][j] = 0; /* make sure it's not counted next time */
            			}
        			}
    			}
            	if(board_cleared(&gp->board))
            	{
	            	gp->state = GAME_STATE_LEVEL;
	            	gp->timer = 0;
	            	t3f_play_music("data/music/level_complete.xm");
            	}
            	else
            	{
                	gp->state = GAME_STATE_FALL;
                	gp->timer = 29;
            	}
        	}
            break;
        }

        /* when viruses killed do this */
        case GAME_STATE_KILL:
        {
	        gp->timer += 4;
	        if(gp->timer >= 256)
	        {
		        gp->state = GAME_STATE_PLAY;
	        }
            break;
        }

        /* whenever a pill breaks off and needs to fall */
        case GAME_STATE_FALL:
        {
	        gp->timer++;
	        if(gp->timer % 30 == 0)
	        {
            	blocks_fall(gp);
        	}
            break;
        }
        
        case GAME_STATE_LEVEL:
        {
	        gp->timer++;
	        if(gp->controller.rleft || gp->controller.rright)
	        {
                gp->state = GAME_STATE_INTRO;
                gp->timer = 0;
               	gp->level++;
            	clear_board(gp);
            	gp->pill.active = 0;
               	if(gp->level > 20)
               	{
	               	gp->over = 1;
	               	gp->level = 20;
	               	t3f_stop_music();
               	}
               	else
               	{
               		t3f_play_music("data/music/bgm.xm");
        		}
	        }
	        break;
        }
        case GAME_STATE_OVER:
        {
			if(gp->controller.rleft || gp->controller.rright)
			{
				initialize_game(gp, 1);
			}
        	break;
        }
    }
    gp->frame++;
    return 1;
}

int get_aid_pos(GAME * gp)
{
	int i;
	
	for(i = gp->pill.y >= 0 ? gp->pill.y : 0; i < BOARD_RY - 1; i++)
	{
		if((gp->board.data[i][gp->pill.x] && gp->pill.data[0][0]) ||
		   (gp->board.data[i][gp->pill.x + 1] && gp->pill.data[0][1]) ||
		   (gp->board.data[i + 1][gp->pill.x] && gp->pill.data[1][0]) ||
		   (gp->board.data[i + 1][gp->pill.x + 1] && gp->pill.data[1][1]))
		{
			return i - 1;
		}
	}
	return BOARD_RY - 2;
}

/* update the display */
void draw_game(GAME * gp, THEME * tp)
{
    int i, j;
    int py = get_aid_pos(gp);

    /* draw the board */
    for(i = 0; i < BOARD_RY; i++)
    {
        for(j = 0; j < BOARD_RX; j++)
        {
		    if(gp->state == GAME_STATE_MATCHES)
    		{
				if(gp->board.match_data[i][j])
            	{
	            	if(gp->board.match_data[i][j] >= 16)
	            	{
		                t3f_draw_animation(tp->virus[(int)gp->board.match_data[i][j] - 16], al_map_rgba(255, 255, 255, 255 - gp->timer), gp->frame, j * 16 + gp->x, i * 16 + gp->y, 0.0, 0);
	            	}
	            	else
	            	{
		                t3f_draw_bitmap(tp->block[(int)gp->board.match_data[i][j]], al_map_rgba(255, 255, 255, 255 - gp->timer), j * 16 + gp->x, i * 16 + gp->y, 0.0, 0);
		            }
            	}
            	else if(gp->board.data[i][j])
            	{
	            	if(gp->board.data[i][j] >= 16)
	            	{
		                t3f_draw_animation(tp->virus[(int)gp->board.data[i][j] - 16], t3f_color_white, gp->frame, j * 16 + gp->x, i * 16 + gp->y, 0.0, 0);
	            	}
	            	else
	            	{
	                	al_draw_bitmap(tp->block[(int)gp->board.data[i][j]]->bitmap, j * 16 + gp->x, i * 16 + gp->y, 0);
                	}
            	}
        	}
        	else
        	{
            	if(gp->board.data[i][j])
            	{
	            	if(gp->board.data[i][j] >= 16)
	            	{
		                t3f_draw_animation(tp->virus[(int)gp->board.data[i][j] - 16], t3f_color_white, gp->frame, j * 16 + gp->x, i * 16 + gp->y, 0.0, 0);
	            	}
	            	else
	            	{
	                	al_draw_bitmap(tp->block[(int)gp->board.data[i][j]]->bitmap, j * 16 + gp->x, i * 16 + gp->y, 0);
            		}
	            }
        	}
        }
    }

    /* draw the pill */
    if(gp->pill.active)
    {
		if(gp->aid && py != gp->pill.y)
		{
	        for(i = 0; i < 2; i++)
        	{
	            for(j = 0; j < 2; j++)
            	{
	                if(gp->pill.data[i][j])
                	{
	                   	al_draw_tinted_bitmap(tp->block[(int)gp->pill.data[i][j]]->bitmap, al_map_rgba_f(1.0, 1.0, 1.0, 0.5), (gp->pill.x + j) * 16 + gp->x, (get_aid_pos(gp) + i) * 16 + gp->y, 0);
                	}
            	}
        	}
		}
        for(i = 0; i < 2; i++)
        {
            for(j = 0; j < 2; j++)
            {
                if(gp->pill.data[i][j])
                {
                   	al_draw_bitmap(tp->block[(int)gp->pill.data[i][j]]->bitmap, (gp->pill.x + j) * 16 + gp->x, (gp->pill.y + i) * 16 + gp->y + 1, 0);
                }
            }
        }
    }
    
    /* draw preview */
	if(gp->level <= 20)
	{
		al_draw_scaled_rotated_bitmap(tp->block[(int)gp->next_pill.data[1][0]]->bitmap, 16, 8, 408 + 16 * 2, 168 + 8 * 2, 2, 2, gp->preview_angle, 0);
		al_draw_scaled_rotated_bitmap(tp->block[(int)gp->next_pill.data[1][1]]->bitmap, 0, 8, 408 + 16 * 2, 168 + 8 * 2, 2, 2, gp->preview_angle, 0);
	}
}
