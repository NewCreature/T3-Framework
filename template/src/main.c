#include "t3f/t3f.h"

int app_state = 0;

/* main logic routine */
void app_logic(void)
{
	switch(app_state)
	{
		default:
		{
			/* insert logic here, as your project grows you can add more states
			 * to deal with various parts of your app (logo, title screen, in-
			 * game, etc.) */
			break;
		}
	}
}

/* main rendering routine */
void app_render(void)
{
	switch(app_state)
	{
		default:
		{
			/* insert rendering code here, see app_logic() for more info */
			break;
		}
	}
}

/* initialize our app, load graphics, etc. */
bool app_initialize(int argc, char * argv[])
{
	/* initialize T3F */
	if(!t3f_initialize("My App", 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	return true;
}

int main(int argc, char * argv[])
{
	if(app_initialize(argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	return 0;
}
