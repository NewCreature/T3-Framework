#include "t3f/t3f.h"

void logic(void)
{
}

void render(void)
{
}

int main(int argc, char * argv[])
{
	if(!t3f_initialize("My App", 640, 480, 60.0, logic, render, T3F_DEFAULT))
	{
		printf("Error initializing T3F\n");
		return -1;
	}
	t3f_run();
	return 0;
}
