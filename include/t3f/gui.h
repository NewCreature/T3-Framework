#ifndef T3F_GUI_H
#define T3F_GUI_H

#ifdef __cplusplus
   extern "C" {
#endif

#include "t3f.h"

#define T3F_GUI_MAX_ELEMENTS    128

/* GUI element types */
#define T3F_GUI_ELEMENT_TEXT      0 // text element
#define T3F_GUI_ELEMENT_IMAGE     1 // image element
#define T3F_GUI_ELEMENT_ANIMATION 2 // animation element

/* GUI_element flags */
#define T3F_GUI_ELEMENT_STATIC    (1 << 0) // do not animate on hover
#define T3F_GUI_ELEMENT_CENTER_H  (1 << 1)
#define T3F_GUI_ELEMENT_CENTER_V  (1 << 2)
#define T3F_GUI_ELEMENT_SHADOW    (1 << 3) // element has shadow
#define T3F_GUI_ELEMENT_AUTOHIDE  (1 << 4) // element is hidden unless mouse pointer is close
#define T3F_GUI_ELEMENT_COPY      (1 << 5) // element maintains its own copy of the data
#define T3F_GUI_ELEMENT_ON_TOUCH  (1 << 6) // active upon touch
#define T3F_GUI_ELEMENT_OWN       (1 << 7)
#define T3F_GUI_ELEMENT_CENTRE T3F_GUI_ELEMENT_CENTER_H
#define T3F_GUI_ELEMENT_CENTER T3F_GUI_ELEMENT_CENTER_H

/* GUI flags */
#define T3F_GUI_DISABLED          (1 << 0) // GUI is disabled
#define T3F_GUI_NO_MOUSE          (1 << 1)
#define T3F_GUI_NO_TOUCH          (1 << 2)
#define T3F_GUI_USER_FLAG         (1 << 3)

typedef struct
{

	int type;
	const void * data;
  void * allocated_data;
	void * resource; // for bitmaps, fonts, etc.
	ALLEGRO_COLOR color;
  ALLEGRO_COLOR inactive_color;
  ALLEGRO_COLOR active_color;
	int flags;
	int (*proc)(void *, int, void *);
	char * description;
	int description_flags;

	int ox, oy;
	int d1, d2, d3, d4;
  float sx, sy;
  float hx, hy;

} T3F_GUI_ELEMENT;

typedef struct
{

	T3F_GUI_ELEMENT element[T3F_GUI_MAX_ELEMENTS];
	int elements;
	int flags;

	int ox, oy;

	float hover_y;
	int hover_element;
  int font_margin_top;
  int font_margin_bottom;
  int font_margin_left;
  int font_margin_right;
	int tick;

} T3F_GUI;

typedef struct
{

	void(*get_element_edges)(T3F_GUI * pp, int i, int * left, int * top, int * right, int * bottom);
	void(*render_element)(T3F_GUI * pp, int i, bool hover, int flags);

} T3F_GUI_DRIVER;

void t3f_set_gui_driver(T3F_GUI_DRIVER * dp);
void t3f_set_gui_shadow_color(ALLEGRO_COLOR color);
T3F_GUI * t3f_create_gui(int ox, int oy);
void t3f_destroy_gui(T3F_GUI * pp);

T3F_GUI_ELEMENT * t3f_add_gui_image_element(T3F_GUI * pp, int (*proc)(void *, int, void *), T3F_BITMAP * bitmap, ALLEGRO_COLOR color, int ox, int oy, int flags);
T3F_GUI_ELEMENT * t3f_add_gui_animation_element(T3F_GUI * pp, int (*proc)(void *, int, void *), T3F_ANIMATION * animation, ALLEGRO_COLOR color, int ox, int oy, int flags);
T3F_GUI_ELEMENT * t3f_add_gui_text_element(T3F_GUI * pp, int (*proc)(void *, int, void *), T3F_FONT * font, ALLEGRO_COLOR color, const char * text, int ox, int oy, int flags);
int t3f_describe_last_gui_element(T3F_GUI * pp, char * text, int flags);
void t3f_center_gui(T3F_GUI * pp, float oy, float my);
void t3f_set_gui_shadow(T3F_GUI * pp, float x, float y);
void t3f_set_gui_hover_lift(T3F_GUI * pp, float x, float y);
void t3f_set_gui_element_interaction_colors(T3F_GUI * pp, ALLEGRO_COLOR inactive_color, ALLEGRO_COLOR active_color);
int t3f_get_gui_width(T3F_GUI * pp);
int t3f_get_gui_height(T3F_GUI * pp, float * top);

bool t3f_select_hover_gui_element(T3F_GUI * pp, float x, float y);
void t3f_select_previous_gui_element(T3F_GUI * pp);
void t3f_select_next_gui_element(T3F_GUI * pp);
void t3f_activate_selected_gui_element(T3F_GUI * pp, void * data);
void t3f_reset_gui_input(T3F_GUI * pp);
bool t3f_process_gui(T3F_GUI * pp, int flags, void * data);
void t3f_render_gui(T3F_GUI * pp, int flags);

#ifdef __cplusplus
	}
#endif

#endif
