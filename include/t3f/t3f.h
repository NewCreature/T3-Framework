#ifndef T3F_H
#define T3F_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <stdio.h>
#include <math.h>

#define T3F_USE_KEYBOARD    1
#define T3F_USE_MOUSE       2
#define T3F_USE_JOYSTICK    4
#define T3F_USE_SOUND       8
#define T3F_USE_FULLSCREEN 16
#define T3F_RESIZABLE      32
#define T3F_FORCE_ASPECT   64
#define T3F_NO_DISPLAY    128
#define T3F_USE_TOUCH     256
#define T3F_USE_OPENGL    512
#define T3F_DEFAULT (T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_JOYSTICK | T3F_USE_TOUCH | T3F_USE_SOUND | T3F_FORCE_ASPECT)

#define T3F_KEY_BUFFER_MAX 256
#define T3F_KEY_BUFFER_FORCE_LOWER 1
#define T3F_KEY_BUFFER_FORCE_UPPER 2

#define T3F_MAX_JOYSTICKS 16

#define T3F_MAX_TOUCHES   64

#define T3F_MAX_STACK     16

#define T3F_ATLAS_TILE     0
#define T3F_ATLAS_SPRITE   1
#define T3F_ATLAS_MAX_BITMAPS 1024
#define T3F_MAX_ATLASES   32

/* structure holds information about a 3D viewport usually used to represent
   one player's screen, split screen games will have multiple viewports */
typedef struct
{
	
	/* offset of viewport */
	float offset_x;
	float offset_y;
	float width;
	float height;
	
	/* vanishing point */
	float vp_x;
	float vp_y;

} T3F_VIEW;

typedef struct
{

	ALLEGRO_BITMAP * page;
	int x, y;
	int width, height;
	int line_height;
	ALLEGRO_BITMAP ** bitmap[T3F_ATLAS_MAX_BITMAPS];
	int bitmap_type[T3F_ATLAS_MAX_BITMAPS];
	int bitmaps;
	
} T3F_ATLAS;

typedef struct
{
	
	bool active; // is this touch active?
	bool released;
	float x, y;
	bool primary;
	
} T3F_TOUCH;

/* include all T3F modules */
#include "sound.h"
#include "music.h"
#include "bitmap.h"
#include "animation.h"
#include "font.h"
#include "collision.h"
#include "controller.h"
#include "gui.h"
//#include "tilemap.h"
#include "vector.h"

extern int t3f_virtual_display_width;
extern int t3f_virtual_display_height;

extern bool t3f_key[ALLEGRO_KEY_MAX];
extern bool t3f_quit;
extern int t3f_flags;

extern int t3f_mouse_x;
extern int t3f_mouse_y;
extern int t3f_mouse_z;
extern int t3f_mouse_dx;
extern int t3f_mouse_dy;
extern int t3f_mouse_dz;
extern bool t3f_mouse_button[16];
extern bool t3f_mouse_hidden;

extern ALLEGRO_JOYSTICK * t3f_joystick[T3F_MAX_JOYSTICKS];
extern ALLEGRO_JOYSTICK_STATE t3f_joystick_state[T3F_MAX_JOYSTICKS];
extern T3F_TOUCH t3f_touch[T3F_MAX_TOUCHES];
extern ALLEGRO_DISPLAY * t3f_display;
extern ALLEGRO_TIMER * t3f_timer;
extern ALLEGRO_EVENT_QUEUE * t3f_queue;
extern ALLEGRO_CONFIG * t3f_config;
extern ALLEGRO_PATH * t3f_data_path;
extern ALLEGRO_PATH * t3f_config_path;

extern ALLEGRO_TRANSFORM t3f_base_transform;
extern ALLEGRO_TRANSFORM t3f_current_transform;

extern T3F_VIEW * t3f_default_view;
extern ALLEGRO_COLOR t3f_color_white;
extern ALLEGRO_COLOR t3f_color_black;

int t3f_initialize(const char * name, int w, int h, double fps, void (*logic_proc)(), void (*render_proc)(), int flags);
bool t3f_locate_resource(char * argv, const char * filename);
int t3f_set_gfx_mode(int w, int h, int flags);
void t3f_set_clipping_rectangle(int x, int y, int w, int h);
void t3f_set_event_handler(void (*proc)(ALLEGRO_EVENT * event));
void t3f_exit(void);
void t3f_event_handler(ALLEGRO_EVENT * event);
void t3f_render(void);
void t3f_run(void);

float t3f_distance(float x1, float y1, float x2, float y2);
double t3f_drand(void);

/* keyboard */
void t3f_clear_keys(void);
bool t3f_add_key(int key);
char t3f_read_key(int flags);

/* mouse */
void t3f_get_mouse_mickeys(int * x, int * y, int * z);
void t3f_set_mouse_xy(float x, float y);

/* touch */
void t3f_clear_touch_data(void);

bool t3f_push_state(int flags);
bool t3f_pop_state(void);

int t3f_get_joystick_number(ALLEGRO_JOYSTICK * jp);
float t3f_fread_float(ALLEGRO_FILE * fp);
int t3f_fwrite_float(ALLEGRO_FILE * fp, float f);

ALLEGRO_FILE * t3f_open_file(ALLEGRO_PATH * pp, const char * fn, const char * m);
unsigned long t3f_checksum_file(const char * fn);
bool t3f_copy_file(const char * src, const char * dest);
void t3f_setup_directories(ALLEGRO_PATH * final);
const char * t3f_get_filename(ALLEGRO_PATH * path, const char * fn);
bool t3f_save_bitmap_f(ALLEGRO_FILE * fp, ALLEGRO_BITMAP * bp);
ALLEGRO_BITMAP * t3f_load_bitmap_f(ALLEGRO_FILE * fp);

/* 3D projection */
T3F_VIEW * t3f_create_view(float ox, float oy, float w, float h, float vpx, float vpy);
void t3f_destroy_view(T3F_VIEW * vp);
void t3f_store_state(T3F_VIEW * sp);
void t3f_restore_state(T3F_VIEW * sp);
void t3f_select_view(T3F_VIEW * sp);
float t3f_project_x(float x, float z);
float t3f_project_y(float y, float z);

/* sprite atlas functions */
T3F_ATLAS * t3f_create_atlas(int w, int h);
void t3f_destroy_atlas(T3F_ATLAS * ap);
ALLEGRO_BITMAP * t3f_add_bitmap_to_atlas(T3F_ATLAS * ap, ALLEGRO_BITMAP ** bp, int type);
bool t3f_rebuild_atlases(void);

/* drawing functions */
void t3f_draw_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float x, float y, float z, int flags);
void t3f_draw_rotated_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float cx, float cy, float x, float y, float z, float angle, int flags);
void t3f_draw_scaled_rotated_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float cx, float cy, float x, float y, float z, float angle, float scale_x, float scale_y, int flags);
void t3f_draw_scaled_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float x, float y, float z, float w, float h, int flags);

#ifdef __cplusplus
   }
#endif

#endif
