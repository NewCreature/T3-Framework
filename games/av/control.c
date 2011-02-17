#include "includes.h"

void read_controller(CONTROLLER * cp, int keys[8])
{
	/* read left button */
	if(keys[0] && cp->was_left < 1)
	{
		cp->left = 1;
		cp->is_left++;
		cp->was_left++;
	}
	else if(keys[0] && cp->was_left >= 1 && cp->was_left < REPEAT_DELAY)
	{
		cp->left = 0;
		cp->is_left++;
		cp->was_left++;
	}
	else
	{
		if(cp->was_left % REPEAT_RATE == 0)
		{
			cp->left = 1;
		}
		else
		{
			cp->left = 0;
		}
		cp->is_left++;
		cp->was_left++;
	}
	if(!keys[0])
	{
		cp->left = 0;
		cp->is_left = 0;
		cp->was_left = 0;
	}

	/* read right button */
	if(keys[1] && cp->was_right < 1)
	{
		cp->right = 1;
		cp->is_right++;
		cp->was_right++;
	}
	else if(keys[1] && cp->was_right >= 1 && cp->was_right < REPEAT_DELAY)
	{
		cp->right = 0;
		cp->is_right++;
		cp->was_right++;
	}
	else
	{
		if(cp->was_right % REPEAT_RATE == 0)
		{
			cp->right = 1;
		}
		else
		{
			cp->right = 0;
		}
		cp->is_right++;
		cp->was_right++;
	}
	if(!keys[1])
	{
		cp->right = 0;
		cp->is_right = 0;
		cp->was_right = 0;
	}

	/* read drop button */
	if(keys[3])
	{
		if(cp->was_drop % DROP_SPEED == 0 && !cp->hdrop)
		{
			cp->drop = 1;
		}
		else
		{
			cp->drop = 0;
		}
		cp->was_drop++;
	}
	else
	{
		cp->drop = 0;
		cp->was_drop = 0;
		cp->hdrop = 0;
	}

	/* read rotate left button */
	if(keys[4] && !cp->was_rleft)
	{
		cp->rleft = 1;
		cp->was_rleft = 1;
	}
	else
	{
		cp->rleft = 0;
	}
	if(!keys[4])
	{
		cp->was_rleft = 0;
	}

	/* read rorate right button */
	if(keys[5] && !cp->was_rright)
	{
		cp->rright = 1;
		cp->was_rright = 1;
	}
	else
	{
		cp->rright = 0;
	}
	if(!keys[5])
	{
		cp->was_rright = 0;
	}
}
