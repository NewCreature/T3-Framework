#include <allegro.h>

int done = 0;
BITMAP * buffer;
BITMAP * map;
int virus = 16;
BITMAP * vpic[3];
PALETTE pal;
int mx, my;
char filename[1024] = {0};

int main(void)
{
	allegro_init();
	install_keyboard();
	install_mouse();
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	buffer = create_bitmap(640, 480);
	vpic[0] = load_pcx("antivirus.dat#virus_red.pcx", pal);
	vpic[1] = load_pcx("antivirus.dat#virus_yellow.pcx", NULL);
	vpic[2] = load_pcx("antivirus.dat#virus_blue.pcx", NULL);
	map = create_bitmap(8, 16);
	clear_bitmap(map);
	set_palette(pal);
	set_mouse_sprite(NULL);
	while(!done)
	{
		if(key[KEY_ESC])
		{
			done = 1;
		}
		if(key[KEY_1])
		{
			virus = 16;
		}
		else if(key[KEY_2])
		{
			virus = 17;
		}
		else if(key[KEY_3])
		{
			virus = 18;
		}
		if(key[KEY_F2])
		{
			if(file_select_ex("Save:", filename, "pcx", 1024, 320, 240))
			{
				save_pcx(filename, map, pal);
			}
			clear_keybuf();
		}
		if(key[KEY_F3])
		{
			if(file_select_ex("Load:", filename, "pcx", 1024, 320, 240))
			{
				destroy_bitmap(map);
				map = load_pcx(filename, NULL);
			}
			clear_keybuf();
		}
		mx = mouse_x;
		my = mouse_y;
		if(mouse_b & 1 && mx / 16 < 8 && my / 16 < 16)
		{
			map->line[my / 16][mx / 16] = virus;
		}
		else if(mouse_b & 2 && mx / 16 < 8 && my / 16 < 16)
		{
			map->line[my / 16][mx / 16] = 0;
		}
		clear_bitmap(buffer);
		for(int i = 0; i < 16; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				if(map->line[i][j])
				{
					draw_sprite(buffer, vpic[map->line[i][j] - 16], j * 16, i * 16);
				}
			}
		}
		draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
		blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
	}
	return 0;
}
END_OF_MAIN()
