#ifndef STUDIO_H
#define STUDIO_H

#include <allegro5/allegro5.h>
#include <allegro5/a5_font.h>
#include "wfsel.h"

#define MAPPER_STATE_TILESET 0
#define MAPPER_STATE_TILEMAP 1

/* Allegro stuff */
extern ALLEGRO_DISPLAY * studio_display;
extern ALLEGRO_TIMER * studio_timer;
extern ALLEGRO_EVENT_QUEUE * studio_queue;
extern ALLEGRO_FONT * studio_font;

/* OCD Animation Studio stuff */
extern T3F_ANIMATION * studio_animation;
extern unsigned long studio_tick;
extern bool studio_quit;
extern char studio_filename[1024];
extern char * studio_return_filename;
extern NCDFS_FILTER_LIST * studio_filter_animation_files;
extern NCDFS_FILTER_LIST * studio_filter_image_files;

extern int studio_current_frame;
extern int studio_current_bitmap;
extern int studio_state;
extern float studio_scale;
extern ALLEGRO_COLOR studio_left_color;
extern ALLEGRO_COLOR studio_right_color;
extern ALLEGRO_COLOR * studio_color;
extern bool studio_left_held;
extern bool studio_right_held;
extern int studio_mouse_x;
extern int studio_mouse_y;

#endif
