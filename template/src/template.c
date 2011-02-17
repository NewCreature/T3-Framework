/* template.c
   This file contains a template which you can use to begin a new project.
   All T3F modules are included by default but unused modules can safely be
   removed.
   
   Instructions:
   
   Copy the "template" folder to where you want to store your new project and
   rename the directory to whatever you want.
   
   Run "make -f makefile.linux (or the makefile appropriate for your platform)
   from the "src" directory to build. */

#include "t3f/t3f.h"

int state = 0;

void logic(void)
{
	switch(state)
	{
		default:
		{
			break;
		}
	}
}

void render(void)
{
	switch(state)
	{
		default:
		{
			break;
		}
	}
}

bool initialize(void)
{
	if(!t3f_initialize("My Game", 640, 480, 60.0, logic, render, T3F_DEFAULT))
	{
		printf("Failed to initialize T^3 Framework!\n");
		return false;
	}
	return true;
}

void shutdown(void)
{
}

int main(int argc, char * argv[])
{
	if(!initialize())
	{
		return 1;
	}
	t3f_run();
	shutdown();
	return 0;
}
