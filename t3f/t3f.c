#include <allegro5/allegro5.h>

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <stdio.h>
#include <string.h>

#include "t3f.h"

/* display data */
int t3f_virtual_display_width = 0;
int t3f_virtual_display_height = 0;
int t3f_display_offset_x = 0;
int t3f_display_offset_y = 0;
int t3f_display_width = 0;
int t3f_display_height = 0;
float t3f_mouse_scale_x = 1.0;
float t3f_mouse_scale_y = 1.0;

/* keyboard data */
bool t3f_key[ALLEGRO_KEY_MAX] = {false};
char t3f_key_buffer[T3F_KEY_BUFFER_MAX] = {0};
int t3f_key_buffer_keys = 0;

/* mouse data */
int t3f_mouse_x = 0;
int t3f_mouse_y = 0;
int t3f_mouse_z = 0;
int t3f_mouse_dx = 0;
int t3f_mouse_dy = 0;
int t3f_mouse_dz = 0;
bool t3f_mouse_button[16] = {0};
bool t3f_mouse_hidden = false;

/* joystick data */
ALLEGRO_JOYSTICK * t3f_joystick[T3F_MAX_JOYSTICKS] = {NULL};
ALLEGRO_JOYSTICK_STATE t3f_joystick_state[T3F_MAX_JOYSTICKS];

ALLEGRO_TRANSFORM t3f_base_transform;
ALLEGRO_TRANSFORM t3f_current_transform;

/* blender data */
ALLEGRO_STATE t3f_state_stack[T3F_MAX_STACK];
int t3f_state_stack_size = 0;

bool t3f_quit = false;
int t3f_requested_flags = 0;
int t3f_flags = 0;

void (*t3f_logic_proc)() = NULL;
void (*t3f_render_proc)() = NULL;

ALLEGRO_DISPLAY * t3f_display = NULL;
ALLEGRO_TIMER * t3f_timer = NULL;
ALLEGRO_EVENT_QUEUE * t3f_queue = NULL;
char t3f_window_title[1024] = {0};

ALLEGRO_CONFIG * t3f_config = NULL;
ALLEGRO_PATH * t3f_data_path = NULL;
ALLEGRO_PATH * t3f_config_path = NULL;
static char t3f_config_filename[1024] = {0};
static char t3f_return_filename[1024] = {0};

/* view data */
/* internal state of the 3d engine */
static T3F_VIEW * t3f_current_view = NULL;
T3F_VIEW * t3f_default_view = NULL;

/* colors */
ALLEGRO_COLOR t3f_color_white;
ALLEGRO_COLOR t3f_color_black;

/* internal variables */
static bool t3f_need_redraw = false;
static void (*t3f_event_handler_proc)(ALLEGRO_EVENT * event) = NULL;

/* used to locate resources */
#ifdef T3F_PACKAGE_NAME
	static const char * t3f_package_name = T3F_PACKAGE_NAME;
#else
	static const char * t3f_package_name = NULL;
#endif

static bool t3f_is_path_present(ALLEGRO_PATH * pp)
{
	return al_filename_exists(al_path_cstr(pp, '/'));
}

/* creates directory structure that leads to 'final' */
void t3f_setup_directories(ALLEGRO_PATH * final)
{
	ALLEGRO_PATH * working_path[16] = {NULL};
	int working_paths = 0;
	const char * cpath = NULL;
	int i;
	
	/* find first directory that exists */
	working_path[0] = al_clone_path(final);
	working_paths = 1;
	while(!t3f_is_path_present(working_path[working_paths - 1]) && working_paths < 16)
	{
		working_path[working_paths] = al_clone_path(working_path[working_paths - 1]);
		al_drop_path_tail(working_path[working_paths]);
		working_paths++;
	}
	
	/* iterate through working_path[] and make each directory */
	for(i = working_paths - 1; i >= 0; i--)
	{
		cpath = al_path_cstr(working_path[i], '/');
//		printf("make directory: %s\n", cpath);
		al_make_directory(cpath);
	}
	for(i = 0; i < working_paths; i++)
	{
		al_destroy_path(working_path[i]);
	}
}

/* this gets Allegro ready */
int t3f_initialize(const char * name, int w, int h, double fps, void (*logic_proc)(), void (*render_proc)(), int flags)
{
	int i;
	ALLEGRO_PATH * temp_path = NULL;
	
	/* initialize Allegro */
	if(!al_init())
	{
		printf("Allegro failed to initialize!\n");
		return 0;
	}
	al_set_app_name(name);
	#ifdef T3F_COMPANY
		al_set_org_name(T3F_COMPANY);
	#endif
	
	/* set up application path */
	t3f_config_path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
	t3f_data_path = al_get_standard_path(ALLEGRO_USER_DATA_PATH);
	t3f_setup_directories(t3f_config_path);
	t3f_setup_directories(t3f_data_path);
	
	/* set up configuration file */
	temp_path = al_clone_path(t3f_config_path);
	al_set_path_filename(temp_path, "settings.ini");
	strcpy(t3f_config_filename, al_path_cstr(temp_path, '/'));
	al_destroy_path(temp_path);
	t3f_config = al_load_config_file(t3f_config_filename);
	if(!t3f_config)
	{
		t3f_config = al_create_config();
	}
	
	if(!al_init_image_addon())
	{
		printf("Failed to initialize Image I/O module!\n");
		return 0;
	}
	al_init_font_addon();
	if(flags & T3F_USE_SOUND)
	{
		if(!al_install_audio())
		{
			printf("Could not init sound!\n");
		}
		else if(!al_reserve_samples(16))
		{
			printf("Could not set up voice and mixer.\n");
		}
		else
		{
			t3f_flags |= T3F_USE_SOUND;
		}
		al_init_acodec_addon();
	}
	if(flags & T3F_USE_KEYBOARD)
	{
		if(!al_install_keyboard())
		{
			printf("Failed to initialize keyboard!\n");
		}
		else
		{
			t3f_flags |= T3F_USE_KEYBOARD;
		}
	}
	if(flags & T3F_USE_MOUSE)
	{
		if(!al_install_mouse())
		{
			printf("Failed to initialize mouse!\n");
		}
		else
		{
			t3f_flags |= T3F_USE_MOUSE;
		}
	}
	if(flags & T3F_USE_JOYSTICK)
	{
		if(!al_install_joystick())
		{
			printf("Failed to initialize joystick!\n");
		}
		else
		{
			t3f_flags |= T3F_USE_JOYSTICK;
		}
	}
	al_init_primitives_addon();
	
	/* if we are using console (for a server, for instance) don't create display */
	strcpy(t3f_window_title, name);
	if(flags & T3F_USE_CONSOLE)
	{
		t3f_flags |= T3F_USE_CONSOLE;
	}
	else
	{
		if(!t3f_set_gfx_mode(w, h, flags))
		{
			return 0;
		}
	}
	
	t3f_timer = al_create_timer(1.000 / fps);
	if(!t3f_timer)
	{
		printf("Failed to create timer!\n");
		return 0;
	}
	
	t3f_queue = al_create_event_queue();
	if(!t3f_queue)
	{
		printf("Failed to create event queue!\n");
		return 0;
	}
	
	if(t3f_flags & T3F_USE_KEYBOARD)
	{
		al_register_event_source(t3f_queue, al_get_keyboard_event_source());
	}
	if(t3f_flags & T3F_USE_MOUSE)
	{
		al_register_event_source(t3f_queue, al_get_mouse_event_source());
	}
	if(t3f_flags & T3F_USE_JOYSTICK)
	{
		al_register_event_source(t3f_queue, al_get_joystick_event_source());
		for(i = 0; i < al_get_num_joysticks(); i++)
		{
			t3f_joystick[i] = al_get_joystick(i);
		}
	}
	if(!(t3f_flags & T3F_USE_CONSOLE))
	{
		al_register_event_source(t3f_queue, al_get_display_event_source(t3f_display));
	}
	al_register_event_source(t3f_queue, al_get_timer_event_source(t3f_timer));
	
	t3f_default_view = t3f_create_view(0, 0, w, h, w / 2, h / 2);
	if(!t3f_default_view)
	{
		return 0;
	}
	t3f_select_view(t3f_default_view);
	t3f_color_white = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
	t3f_color_black = al_map_rgba_f(0.0, 0.0, 0.0, 1.0);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	
	t3f_logic_proc = logic_proc;
	t3f_render_proc = render_proc;
	
	return 1;
}

/* function to ease the burden of having resources located in different places
 * on different platforms, changes to the directory where it finds the specified
 * resource */
bool t3f_locate_resource(const char * filename)
{
	ALLEGRO_PATH * path;
	ALLEGRO_PATH * file_path;
	bool found = false;
	
	/* if we are already in the correct directory */
	if(al_filename_exists(filename))
	{
		return true;
	}
	
	/* look in resources path */
	file_path = al_create_path(filename);
	if(!file_path)
	{
		return false;
	}
	path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	if(path)
	{
		al_join_paths(path, file_path);
		if(al_filename_exists(al_path_cstr(path, '/')))
		{
			found = true;
		}
//		printf("%s\n", al_path_cstr(path, '/'));
		al_destroy_path(path);
	}
	al_destroy_path(file_path);
	if(found)
	{
		path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
		al_change_directory(al_path_cstr(path, '/'));
		al_destroy_path(path);
		return true;
	}

	/* look in "/usr/share" if a package name is defined */
	if(t3f_package_name)
	{
		file_path = al_create_path(filename);
		if(!file_path)
		{
			return false;
		}
		path = al_create_path("/usr/share/");
		if(path)
		{
			al_append_path_component(path, t3f_package_name);
			al_join_paths(path, file_path);
			if(al_filename_exists(al_path_cstr(path, '/')))
			{
				al_change_directory(al_path_cstr(path, '/'));
				found = true;
			}
//			printf("%s\n", al_path_cstr(path, '/'));
			al_destroy_path(path);
		}
		al_destroy_path(file_path);
	}
	if(found)
	{
		path = al_create_path("/usr/share/");
		if(path)
		{
			al_append_path_component(path, t3f_package_name);
			al_change_directory(al_path_cstr(path, '/'));
			al_destroy_path(path);
			return true;
		}
	}
	return false;
}

static void t3f_get_base_transform(void)
{
	float r, vr;
	if(t3f_flags & T3F_FORCE_ASPECT)
	{
		r = (float)al_get_display_height(t3f_display) / (float)al_get_display_width(t3f_display);
		vr = (float)t3f_virtual_display_height / (float)t3f_virtual_display_width;
		/* need to adjust y */
		if(r > vr)
		{
			t3f_display_offset_x = 0;
			t3f_display_width = al_get_display_width(t3f_display);
			t3f_display_offset_y = ((float)al_get_display_height(t3f_display) - (float)al_get_display_width(t3f_display) * vr) / 2.0;
			t3f_display_height = (float)al_get_display_width(t3f_display) * vr;
		}
		else
		{
			t3f_display_offset_x = ((float)al_get_display_width(t3f_display) - (float)al_get_display_height(t3f_display) / vr) / 2.0;
			t3f_display_width = (float)al_get_display_height(t3f_display) / vr;
			t3f_display_offset_y = 0;
			t3f_display_height = al_get_display_height(t3f_display);
		}
		al_build_transform(&t3f_base_transform, t3f_display_offset_x, t3f_display_offset_y, (float)t3f_display_width / (float)t3f_virtual_display_width, (float)t3f_display_height / (float)t3f_virtual_display_height, 0.0);
	}
	else
	{
		al_build_transform(&t3f_base_transform, 0.0, 0.0, (float)al_get_display_width(t3f_display) / (float)t3f_virtual_display_width, (float)al_get_display_height(t3f_display) / (float)t3f_virtual_display_height, 0.0);
	}
}

int t3f_set_gfx_mode(int w, int h, int flags)
{
	const char * cvalue = NULL;
	const char * cvalue2 = NULL;
	char val[128] = {0};
	int dflags = 0;
	int dw, dh;
	if(t3f_display)
	{
		/* if we are switching from window to full screen, create new display */
		if(flags & T3F_USE_FULLSCREEN)
		{
			al_toggle_display_flag(t3f_display, ALLEGRO_FULLSCREEN_WINDOW, true);
		}
		else
		{
			al_toggle_display_flag(t3f_display, ALLEGRO_FULLSCREEN_WINDOW, false);
			t3f_flags ^= T3F_USE_FULLSCREEN;
			al_resize_display(t3f_display, w, h);
		}
		sprintf(val, "%d", w);
		al_set_config_value(t3f_config, "T3F", "display_width", val);
		sprintf(val, "%d", h);
		al_set_config_value(t3f_config, "T3F", "display_height", val);
		t3f_get_base_transform();
		t3f_mouse_scale_x = (float)t3f_virtual_display_width / (float)t3f_display_width;
		t3f_mouse_scale_y = (float)t3f_virtual_display_height / (float)t3f_display_height;
		al_set_window_title(t3f_display, t3f_window_title);
	}
	
	/* first time creating display */
	else
	{
		/* if we are using console (for a server, for instance) don't create display */
		if(flags & T3F_USE_CONSOLE)
		{
			t3f_flags |= T3F_USE_CONSOLE;
		}
		else
		{
			cvalue = al_get_config_value(t3f_config, "T3F", "force_fullscreen");
			cvalue2 = al_get_config_value(t3f_config, "T3F", "force_window");
			if((flags & T3F_USE_FULLSCREEN || (cvalue && !strcmp(cvalue, "true"))) && !(cvalue2 && !strcmp(cvalue2, "true")))
			{
				dflags |= ALLEGRO_FULLSCREEN;
				t3f_flags |= T3F_USE_FULLSCREEN;
			}
			if(flags & T3F_RESIZABLE)
			{
				dflags |= ALLEGRO_RESIZABLE;
				t3f_flags |= T3F_RESIZABLE;
				t3f_flags |= (flags & T3F_FORCE_ASPECT);
			}
			al_set_new_display_flags(dflags);
			cvalue = al_get_config_value(t3f_config, "T3F", "display_width");
			cvalue2 = al_get_config_value(t3f_config, "T3F", "display_height");
			if(cvalue && cvalue2)
			{
				dw = atoi(cvalue);
				dh = atoi(cvalue2);
			}
			else
			{
				dw = w;
				dh = h;
			}
			t3f_display = al_create_display(dw, dh);
			if(!t3f_display)
			{
				printf("Failed to create display!\n");
				return 0;
			}
			t3f_virtual_display_width = w;
			t3f_virtual_display_height = h;
			t3f_get_base_transform();
			t3f_mouse_scale_x = (float)t3f_virtual_display_width / (float)t3f_display_width;
			t3f_mouse_scale_y = (float)t3f_virtual_display_height / (float)t3f_display_height;
			al_set_window_title(t3f_display, t3f_window_title);
		}
	}
	return 1;
}

/* set the clipping rectangle, taking the current transformation into account,
 * used in conjunction with the view system you will pass virtual screen 
 * coordinates */
void t3f_set_clipping_rectangle(int x, int y, int w, int h)
{
	float tx, ty;
	float twx, twy;
	float ox, oy;
	
	/* convert virtual screen coordinates to real display coordinates */
	al_transform_coordinates(&t3f_current_transform, &ox, &oy);
	if(w != 0 && h != 0)
	{
		tx = x;
		ty = y;
		twx = w;
		twy = h;
	}
	else
	{
		tx = 0;
		ty = 0;
		twx = t3f_virtual_display_width;
		twy = t3f_virtual_display_height;
	}
	al_transform_coordinates(&t3f_current_transform, &tx, &ty);
	al_transform_coordinates(&t3f_current_transform, &twx, &twy);
	al_set_clipping_rectangle(tx, ty, twx - ox, twy - oy);
}

void t3f_set_event_handler(void (*proc)(ALLEGRO_EVENT * event))
{
	t3f_event_handler_proc = proc;
}

void t3f_exit(void)
{
	al_save_config_file(t3f_config_filename, t3f_config);
	t3f_quit = true;
}

float t3f_distance(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

#define T3F_RS_SCALE (1.0 / (1.0 + RAND_MAX))
double t3f_drand(void)
{
	double d;
	do
	{
		d = (((rand () * T3F_RS_SCALE) + rand ()) * T3F_RS_SCALE + rand ()) * T3F_RS_SCALE;
	} while (d >= 1); /* Round off */
	return d;
}

void t3f_clear_keys(void)
{
	t3f_key_buffer_keys = 0;
}

bool t3f_add_key(char key)
{
	if(t3f_key_buffer_keys < T3F_KEY_BUFFER_MAX)
	{
		t3f_key_buffer[t3f_key_buffer_keys] = key;
		t3f_key_buffer_keys++;
		return true;
	}
	return false;
}

char t3f_read_key(int flags)
{
	char rkey = 0;
	if(t3f_key_buffer_keys > 0)
	{
		t3f_key_buffer_keys--;
		rkey = t3f_key_buffer[t3f_key_buffer_keys];
		if(flags & T3F_KEY_BUFFER_FORCE_LOWER)
		{
			if(rkey >= 'A' && rkey <= 'Z')
			{
				rkey += 'a' - 'A';
			}
		}
		else if(flags & T3F_KEY_BUFFER_FORCE_UPPER)
		{
			if(rkey >= 'a' && rkey <= 'z')
			{
				rkey -= 'a' - 'A';
			}
		}
	}
	return rkey;
}

void t3f_get_mouse_mickeys(int * x, int * y, int * z)
{
	if(x)
	{
		*x = t3f_mouse_dx - t3f_mouse_x;
		t3f_mouse_dx = t3f_mouse_x;
	}
	if(y)
	{
		*y = t3f_mouse_dy - t3f_mouse_y;
		t3f_mouse_dy = t3f_mouse_y;
	}
	if(z)
	{
		*z = t3f_mouse_dz - t3f_mouse_z;
		t3f_mouse_dz = t3f_mouse_z;
	}
}

void t3f_set_mouse_xy(float x, float y)
{
	al_set_mouse_xy(t3f_display, t3f_mouse_x / t3f_mouse_scale_x, t3f_mouse_y / t3f_mouse_scale_y);
}

bool t3f_push_state(int flags)
{
	if(t3f_state_stack_size < T3F_MAX_STACK)
	{
		al_store_state(&t3f_state_stack[t3f_state_stack_size], flags);
		t3f_state_stack_size++;
		return true;
	}
	return false;
}

bool t3f_pop_state(void)
{
	if(t3f_state_stack_size > 0)
	{
		al_restore_state(&t3f_state_stack[t3f_state_stack_size - 1]);
		t3f_state_stack_size--;
		return true;
	}
	return false;
}

int t3f_get_joystick_number(ALLEGRO_JOYSTICK * jp)
{
	int i;
	
	for(i = 0; i < al_get_num_joysticks(); i++)
	{
		if(jp == t3f_joystick[i] && t3f_joystick[i] != NULL)
		{
			return i;
		}
	}
	return -1;
}

float t3f_fread_float(ALLEGRO_FILE * fp)
{
	char buffer[256] = {0};
	int l;
	
	l = al_fgetc(fp);
	al_fread(fp, buffer, l);
	buffer[l] = '\0';
	return atof(buffer);
//	float f;
//	al_fread(fp, &f, sizeof(float));
//	return f;
}

int t3f_fwrite_float(ALLEGRO_FILE * fp, float f)
{
	char buffer[256] = {0};
	int l;
	
	sprintf(buffer, "%f", f);
	l = strlen(buffer);
	al_fputc(fp, l);
	al_fwrite(fp, buffer, l);
//	al_fwrite(fp, &f, sizeof(float));
	return 1;
}

static void t3f_convert_grey_to_alpha(ALLEGRO_BITMAP * bitmap)
{
	int x, y;
	unsigned char ir, ig, ib, ia;
	ALLEGRO_COLOR pixel;
	ALLEGRO_STATE old_state;

	if(!al_lock_bitmap(bitmap, al_get_bitmap_format(bitmap), 0))
	{
		return;
	}

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(bitmap);

	for(y = 0; y < al_get_bitmap_height(bitmap); y++)
	{
		for(x = 0; x < al_get_bitmap_width(bitmap); x++)
		{
			pixel = al_get_pixel(bitmap, x, y);
			al_unmap_rgba(pixel, &ir, &ig, &ib, &ia);
			if(ir == 255 && ig == 0 && ib == 255)
			{
				pixel = al_map_rgba(0, 0, 0, 0);
				al_put_pixel(x, y, pixel);
			}
			else if(ia > 0 && !(ir == 255 && ig == 255 && ib == 0))
			{
				pixel = al_map_rgba(ir, ir, ir, ir);
				al_put_pixel(x, y, pixel);
			}
		}
	}

	al_restore_state(&old_state);
	al_unlock_bitmap(bitmap);
}

ALLEGRO_FONT * t3f_load_font(const char * fn)
{
	ALLEGRO_BITMAP * fimage;
	ALLEGRO_FONT * fp;
	ALLEGRO_STATE old_state;
	int ranges[] = {32, 126};
	
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	fimage = al_load_bitmap(fn);
	if(!fimage)
	{
		return NULL;
	}
	t3f_convert_grey_to_alpha(fimage);
	al_restore_state(&old_state);
	fp = al_grab_font_from_bitmap(fimage, 1, ranges);
	al_destroy_bitmap(fimage);
	return fp;
}

ALLEGRO_FILE * t3f_open_file(ALLEGRO_PATH * pp, const char * fn, const char * m)
{
	ALLEGRO_PATH * tpath = al_clone_path(pp);
	al_set_path_filename(tpath, fn);
	return al_fopen(al_path_cstr(tpath, '/'), m);
}

unsigned long t3f_checksum_file(const char * fn)
{
	ALLEGRO_FILE * fp;
	unsigned long sum = 0;
	
	fp = al_fopen(fn, "rb");
	while(!al_feof(fp))
	{
		sum += al_fgetc(fp);
	}
	al_fclose(fp);
	return sum;
}

bool t3f_copy_file(const char * src, const char * dest)
{
	ALLEGRO_FILE * fsrc;
	ALLEGRO_FILE * fdest;
	char c;
	
	fsrc = al_fopen(src, "rb");
	if(!fsrc)
	{
		return false;
	}
	fdest = al_fopen(dest, "wb");
	if(!fdest)
	{
		al_fclose(fsrc);
		return false;
	}
	while(!al_feof(fsrc))
	{
		c = al_fgetc(fsrc);
		al_fputc(fdest, c);
	}
	al_fclose(fdest);
	al_fclose(fsrc);
	return true;
}

void t3f_event_handler(ALLEGRO_EVENT * event)
{
	switch(event->type)
	{
		
		/* user pressed close button */
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			t3f_exit();
			break;
		}
		
		/* window was resized */
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			char val[8] = {0};
			al_acknowledge_resize(t3f_display);
			t3f_get_base_transform();
			al_set_clipping_rectangle(0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display));
			al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
			al_flip_display();
			al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
			t3f_select_view(t3f_current_view);
			t3f_mouse_scale_x = (float)t3f_virtual_display_width / (float)t3f_display_width;
			t3f_mouse_scale_y = (float)t3f_virtual_display_height / (float)t3f_display_height;
			sprintf(val, "%d", al_get_display_width(t3f_display));
			al_set_config_value(t3f_config, "T3F", "display_width", val);
			sprintf(val, "%d", al_get_display_height(t3f_display));
			al_set_config_value(t3f_config, "T3F", "display_height", val);
			break;
		}
		
		/* key was pressed or repeated */
		case ALLEGRO_EVENT_KEY_DOWN:
		{
			t3f_key[event->keyboard.keycode] = 1;
			break;
		}
		
		/* key was released */
		case ALLEGRO_EVENT_KEY_UP:
		{
			t3f_key[event->keyboard.keycode] = 0;
			break;
		}
		
		/* a character was entered */
		case ALLEGRO_EVENT_KEY_CHAR:
		{
			if(event->keyboard.unichar != -1)
			{
				t3f_add_key(event->keyboard.unichar);
			}
			if(event->keyboard.repeat)
			{
				t3f_key[event->keyboard.keycode] = 1;
			}
			break;
		}
		
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			t3f_mouse_button[event->mouse.button - 1] = 1;
			t3f_mouse_x = (float)(event->mouse.x - t3f_display_offset_x) * t3f_mouse_scale_x;
			t3f_mouse_y = (float)(event->mouse.y - t3f_display_offset_y) * t3f_mouse_scale_y;
			t3f_mouse_z = event->mouse.z;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		{
			t3f_mouse_button[event->mouse.button - 1] = 0;
			t3f_mouse_x = (float)(event->mouse.x - t3f_display_offset_x) * t3f_mouse_scale_x;
			t3f_mouse_y = (float)(event->mouse.y - t3f_display_offset_y) * t3f_mouse_scale_y;
			t3f_mouse_z = event->mouse.z;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			t3f_mouse_x = (float)(event->mouse.x - t3f_display_offset_x) * t3f_mouse_scale_x;
			t3f_mouse_y = (float)(event->mouse.y - t3f_display_offset_y) * t3f_mouse_scale_y;
			t3f_mouse_z = event->mouse.z;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_WARPED:
		{
			t3f_mouse_x = (float)(event->mouse.x - t3f_display_offset_x) * t3f_mouse_scale_x;
			t3f_mouse_y = (float)(event->mouse.y - t3f_display_offset_y) * t3f_mouse_scale_y;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
		{
			t3f_mouse_hidden = true;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
		{
			t3f_mouse_hidden = false;
			break;
		}
		
		case ALLEGRO_EVENT_JOYSTICK_AXIS:
		case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
		case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
		{
			int jn = t3f_get_joystick_number(event->joystick.id);
			if(jn >= 0)
			{
				al_get_joystick_state(event->joystick.id, &t3f_joystick_state[jn]);
			}
			break;
		}
		
		/* this keeps your program running */
		case ALLEGRO_EVENT_TIMER:
		{
			t3f_logic_proc();
			t3f_need_redraw = true;
			break;
		}
	}	
}

/* called when it's time to render */
void t3f_render(void)
{
	al_copy_transform(&t3f_current_transform, &t3f_base_transform);
	al_use_transform(&t3f_current_transform); // <-- apply additional transformations to t3f_current_transform
	t3f_render_proc();
	al_flip_display();
	t3f_need_redraw = false;
}

/* this function is where it's at
   somewhere in your logic code you need to set t3f_quit = true to exit */
void t3f_run(void)
{
	ALLEGRO_EVENT event;
	
	al_start_timer(t3f_timer);
	while(!t3f_quit)
	{
		al_wait_for_event(t3f_queue, &event);
		if(t3f_event_handler_proc)
		{
			t3f_event_handler_proc(&event);
		}
		else
		{
			t3f_event_handler(&event);
		}
       	/* draw after we have run all the logic */
		if(!(t3f_flags & T3F_USE_CONSOLE) && t3f_need_redraw && al_event_queue_is_empty(t3f_queue))
		{
			t3f_render();
		}
	}
}

const char * t3f_get_filename(ALLEGRO_PATH * path, const char * fn)
{
	ALLEGRO_PATH * temp_path = al_clone_path(path);
	if(!temp_path)
	{
		return NULL;
	}
	al_set_path_filename(temp_path, fn);
	strcpy(t3f_return_filename, al_path_cstr(temp_path, '/'));
	al_destroy_path(temp_path);
	return t3f_return_filename;
}

T3F_VIEW * t3f_create_view(float ox, float oy, float w, float h, float vpx, float vpy)
{
	T3F_VIEW * vp = malloc(sizeof(T3F_VIEW));
	if(!vp)
	{
		return NULL;
	}
	vp->offset_x = ox;
	vp->offset_y = oy;
	vp->width = w;
	vp->height = h;
	vp->vp_x = vpx;
	vp->vp_y = vpy;
	return vp;
}

void t3f_destroy_view(T3F_VIEW * vp)
{
	free(vp);
}

void t3f_store_state(T3F_VIEW * sp)
{
	memcpy(sp, t3f_current_view, sizeof(T3F_VIEW));
}

void t3f_restore_state(T3F_VIEW * sp)
{
	memcpy(t3f_current_view, sp, sizeof(T3F_VIEW));
}

void t3f_select_view(T3F_VIEW * sp)
{
	float sx, sy;
	float dsx, dsy;
	float ox, oy;
	
	t3f_current_view = sp;
	sx = t3f_current_view->width / (float)t3f_virtual_display_width;
	sy = t3f_current_view->height / (float)t3f_virtual_display_height;
	dsx = (float)al_get_display_width(t3f_display) / t3f_virtual_display_width;
	dsy = (float)al_get_display_height(t3f_display) / t3f_virtual_display_height;
	ox = t3f_current_view->offset_x * dsx;
	oy = t3f_current_view->offset_y * dsy;
	
	/* apply additional transformations */
	al_copy_transform(&t3f_current_transform, &t3f_base_transform);
	al_scale_transform(&t3f_current_transform, sx, sy);
	al_translate_transform(&t3f_current_transform, t3f_current_view->offset_x * dsx, t3f_current_view->offset_y * dsy);
	al_use_transform(&t3f_current_transform);
	
	/* convert virtual screen coordinates to real display coordinates */
	t3f_set_clipping_rectangle(0, 0, 0, 0);
}

/* get the x coordinate of the pixel at the given (x, z) coordinate
   takes current projection state into account */
float t3f_project_x(float x, float z)
{
	float rx;
	
//	if(z + t3f_current_view->width > 0)
	if(z + t3f_virtual_display_width > 0)
	{
//		rx = (((x - t3f_current_view->vp_x) * t3f_current_view->width) / (z + t3f_current_view->width) + t3f_current_view->vp_x);
		rx = (((x - t3f_current_view->vp_x) * t3f_virtual_display_width) / (z + t3f_virtual_display_width) + t3f_current_view->vp_x);
		return rx;
	}
	else
	{
		return -65536;
	}
}

/* get the y coordinate of the pixel at the given (y, z) coordinate
   takes current projection state into account */
float t3f_project_y(float y, float z)
{
	float ry;
	
//	if(z + t3f_current_view->height > 0)
	if(z + t3f_virtual_display_width > 0)
	{
//		ry = (((y - t3f_current_view->vp_y) * t3f_current_view->width) / (z + t3f_current_view->width) + t3f_current_view->vp_y);
		ry = (((y - t3f_current_view->vp_y) * t3f_virtual_display_width) / (z + t3f_virtual_display_width) + t3f_current_view->vp_y);
		return ry;
	}
	else
	{
		return -65536;
	}
}

/* create an empty atlas of the specified type and size */
T3F_ATLAS * t3f_create_atlas(int type, int w, int h)
{
	T3F_ATLAS * ap;
	
	ap = malloc(sizeof(T3F_ATLAS));
	if(!ap)
	{
		return NULL;
	}
	ap->bitmap = al_create_bitmap(w, h);
	if(!ap->bitmap)
	{
		free(ap);
		return NULL;
	}
	ap->type = type;
	ap->x = 1; // start at 1 so we get consistency with filtered bitmaps
	ap->y = 1;
	ap->line_height = 0;
	
	return ap;
}

/* destroy the atlas */
void t3f_destroy_atlas(T3F_ATLAS * ap)
{
	al_destroy_bitmap(ap->bitmap);
	free(ap);
}

/* fix for when you have exceeded the size of the sprite sheet */
ALLEGRO_BITMAP * t3f_add_bitmap_to_atlas(T3F_ATLAS * ap, ALLEGRO_BITMAP * bp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * retbp = NULL;
	ALLEGRO_TRANSFORM identity_transform;
	
	if(ap->y >= al_get_bitmap_height(ap->bitmap))
	{
		return NULL;
	}
	
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(ap->bitmap);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_identity_transform(&identity_transform);
	al_use_transform(&identity_transform);
	switch(ap->type)
	{
		case T3F_ATLAS_TILES:
		{
			
			/* go to next line if it doesn't fit */
			if(ap->x + al_get_bitmap_width(bp) + 2 >= al_get_bitmap_width(ap->bitmap))
			{
				ap->x = 1; // start at 1 so we get consistency with filtered bitmaps
				ap->y += ap->line_height;
				
				/* if it still doesn't fit, fail */
				if(ap->y  + al_get_bitmap_height(bp) + 2 >= al_get_bitmap_height(ap->bitmap))
				{
					al_restore_state(&old_state);
					return NULL;
				}
			}
			
//			retbp = _t3f_add_bitmap_to_region(t3f_ss_working_bitmap, bp, t3f_ss_working_pos_x + 1, t3f_ss_working_pos_y + 1);
			/* need to extend edges of tiles so they don't have soft edges */
			al_draw_bitmap(bp, ap->x, ap->y, 0);
			al_draw_bitmap(bp, ap->x + 2, ap->y, 0);
			al_draw_bitmap(bp, ap->x, ap->y + 2, 0);
			al_draw_bitmap(bp, ap->x + 2, ap->y + 2, 0);
			al_draw_bitmap(bp, ap->x + 1, ap->y, 0);
			al_draw_bitmap(bp, ap->x + 1, ap->y + 2, 0);
			al_draw_bitmap(bp, ap->x, ap->y + 1, 0);
			al_draw_bitmap(bp, ap->x + 2, ap->y + 1, 0);
			al_draw_bitmap(bp, ap->x + 1, ap->y + 1, 0);
			
			retbp = al_create_sub_bitmap(ap->bitmap, ap->x + 1, ap->y + 1, al_get_bitmap_width(bp), al_get_bitmap_height(bp));
			
			ap->x += al_get_bitmap_width(bp) + 2;
			if(al_get_bitmap_height(bp) + 2 > ap->line_height)
			{
				ap->line_height = al_get_bitmap_height(bp) + 2;
			}
			break;
		}
		case T3F_ATLAS_SPRITES:
		{
			/* go to next line if it doesn't fit */
			if(ap->x + al_get_bitmap_width(bp) + 1 >= al_get_bitmap_width(ap->bitmap))
			{
				ap->x = 0;
				ap->y += ap->line_height;
				
				/* if it still doesn't fit, fail */
				if(ap->y + al_get_bitmap_height(bp) + 1 >= al_get_bitmap_height(ap->bitmap))
				{
					al_restore_state(&old_state);
					return NULL;
				}
			}
//			retbp = _t3f_add_bitmap_to_region(t3f_ss_working_bitmap, bp, t3f_ss_working_pos_x, t3f_ss_working_pos_y);
			al_draw_bitmap(bp, ap->x, ap->y, 0);
			
			retbp = al_create_sub_bitmap(ap->bitmap, ap->x, ap->y, al_get_bitmap_width(bp), al_get_bitmap_height(bp));
			
			ap->x += al_get_bitmap_width(bp) + 1;
			if(al_get_bitmap_height(bp) + 1 > ap->line_height)
			{
				ap->line_height = al_get_bitmap_height(bp) + 1;
			}
			break;
		}
	}
	al_restore_state(&old_state);
		
	return retbp;
}

void t3f_draw_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float x, float y, float z, int flags)
{
	/* upper left and bottom right points in 3d */
	float obj_x[2], obj_y[2], obj_z[2];
	
	/* upper left and bottom right points in 2d */
	float screen_w, screen_h;

	obj_x[0] = t3f_project_x(x, z);
	obj_x[1] = t3f_project_x(x + al_get_bitmap_width(bp), z);
	obj_y[0] = t3f_project_y(y, z);
	obj_y[1] = t3f_project_y(y + al_get_bitmap_height(bp), z);
	obj_z[0] = z + t3f_current_view->width;
	obj_z[1] = z + t3f_virtual_display_width;
	
	/* clip sprites at z = 0 */
	if(obj_z[0] > 0)
	{
		screen_w = obj_x[1] - obj_x[0];
		screen_h = obj_y[1] - obj_y[0];
		al_draw_tinted_scaled_bitmap(bp, color, 0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), obj_x[0], obj_y[0], screen_w, screen_h, flags);
	}
}

void t3f_draw_rotated_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float cx, float cy, float x, float y, float z, float angle, int flags)
{
	/* upper left and bottom right points in 3d */
	float obj_x[2], obj_y[2], obj_z[2], obj_cx, obj_cy;
	float rx, ry;
	
	/* upper left and bottom right points in 2d */
	float screen_w, screen_h;

	obj_x[0] = t3f_project_x(x - cx, z);
	obj_x[1] = t3f_project_x(x + cx, z);
	obj_y[0] = t3f_project_y(y - cy, z);
	obj_y[1] = t3f_project_y(y + cy, z);
	obj_z[0] = z + t3f_current_view->width;
	obj_z[1] = z + t3f_virtual_display_width;
	obj_cx = t3f_project_x(x, z);
	obj_cy = t3f_project_y(y, z);
	
	/* clip sprites at z = 0 */
	if(obj_z[0] > 0)
	{
		screen_w = obj_x[1] - obj_x[0];
		screen_h = obj_y[1] - obj_y[0];
		rx = screen_w / al_get_bitmap_width(bp);
		ry = screen_h / al_get_bitmap_height(bp);
		al_draw_tinted_scaled_rotated_bitmap(bp, color, cx, cy, obj_cx, obj_cy, rx, ry, angle, flags);
	}
}

void t3f_draw_scaled_rotated_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float cx, float cy, float x, float y, float z, float angle, float scale_x, float scale_y, int flags)
{
	/* upper left and bottom right points in 3d */
	float obj_x[2], obj_y[2], obj_z[2], obj_cx, obj_cy;
	float rx, ry;
	
	/* upper left and bottom right points in 2d */
	float screen_w, screen_h;

	obj_x[0] = t3f_project_x(0, z);
	obj_x[1] = t3f_project_x((float)al_get_bitmap_width(bp) * scale_x, z);
	obj_y[0] = t3f_project_y(0, z);
	obj_y[1] = t3f_project_y((float)al_get_bitmap_height(bp) * scale_y, z);
	obj_z[0] = z + t3f_current_view->width;
	obj_z[1] = z + t3f_virtual_display_width;
	obj_cx = t3f_project_x(x, z);
	obj_cy = t3f_project_y(y, z);
	
	/* clip sprites at z = 0 */
	if(obj_z[0] > 0)
	{
		screen_w = obj_x[1] - obj_x[0];
		screen_h = obj_y[1] - obj_y[0];
		rx = screen_w / al_get_bitmap_width(bp);
		ry = screen_h / al_get_bitmap_height(bp);
		al_draw_tinted_scaled_rotated_bitmap(bp, color, cx, cy, obj_cx, obj_cy, rx, ry, angle, flags);
	}
}

void t3f_draw_scaled_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float x, float y, float z, float w, float h, int flags)
{
	/* upper left and bottom right points in 3d */
	float obj_x[2], obj_y[2], obj_z[2];
	
	/* upper left and bottom right points in 2d */
	float screen_w, screen_h;

	obj_x[0] = t3f_project_x(x, z);
	obj_x[1] = t3f_project_x(x + w, z);
	obj_y[0] = t3f_project_y(y, z);
	obj_y[1] = t3f_project_y(y + h, z);
	obj_z[0] = z + t3f_current_view->width;
	obj_z[1] = z + t3f_virtual_display_width;
	
	/* clip sprites at z = 0 */
	if(obj_z[0] > 0)
	{
		screen_w = obj_x[1] - obj_x[0];
		screen_h = obj_y[1] - obj_y[0];
		al_draw_tinted_scaled_bitmap(bp, color, 0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), obj_x[0], obj_y[0], screen_w, screen_h, flags);
	}
}
