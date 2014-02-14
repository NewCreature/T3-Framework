#include "t3f/t3f.h"

/* structure to hold all of our app-specific data */
typedef struct
{
	
	int state;
	
} APP_INSTANCE;

/* main logic routine */
void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	switch(app->state)
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
void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	switch(app->state)
	{
		default:
		{
			/* insert rendering code here, see app_logic() for more info */
			break;
		}
	}
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	/* initialize T3F */
	if(!t3f_initialize("My App", 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	
	/* You must call this to locate one of your resources if you want your app
	 * to work on all platforms. Your app won't be able to find your resources
	 * if you don't tell it what to locate. I may put a placeholder graphic in
	 * the 'data' folder at some point to help automate this. For the moment,
	 * you must manually call this. (replace "alexlogo.png" with a file that
	 * your project has) */
	if(!t3f_locate_resource("alexlogo.png"))
	{
		printf("Cannot locate resources!\n");
		return false;
	}
	
	app->state = 0;
	return true;
}

int main(int argc, char * argv[])
{
	APP_INSTANCE app;
	
	if(app_initialize(&app, argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	return 0;
}
