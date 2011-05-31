#include "t3f/t3f.h"
#include <allegro5/allegro_native_dialog.h>

ALLEGRO_FILECHOOSER * le_file_selector = NULL;

void le_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
}

void le_render(void)
{
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.25, 1.0));
}

bool le_initialize(void)
{
	if(!t3f_initialize("T3F Loop Editor", 640, 480, 60.0, le_logic, le_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_SOUND))
	{
		return false;
	}
	return true;
}

int main(int argc, char * argv[])
{
	if(!le_initialize())
	{
		return -1;
	}
	return 0;
}
