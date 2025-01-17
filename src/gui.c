#include "t3f.h"
#include "gui.h"
#include "mouse.h"
#include "touch.h"

T3F_GUI_DRIVER t3f_gui_allegro_driver;
static T3F_GUI_DRIVER * t3f_gui_current_driver = NULL;
static bool t3f_gui_check_hover_y(T3F_GUI * pp, int i, float y);
static float t3f_gui_mouse_x = -100000;
static float t3f_gui_mouse_y = -100000;

static void allegro_get_element_edges(T3F_GUI * pp, int i, int * left, int * top, int * right, int * bottom)
{
	switch(pp->element[i].type)
	{
		case T3F_GUI_ELEMENT_TEXT:
		{
			if(left)
			{
				*left = pp->element[i].ox;
				if(pp->element[i].flags & T3F_GUI_ELEMENT_CENTRE)
				{
					*left = pp->element[i].ox - t3f_get_text_width(*((T3F_FONT **)pp->element[i].resource), pp->element[i].data) / 2;
				}
				else
				{
					*left = pp->element[i].ox;
				}
			}
			if(top)
			{
				*top = pp->element[i].oy;
			}
			if(right)
			{
				if(pp->element[i].flags & T3F_GUI_ELEMENT_CENTRE)
				{
					*right = pp->element[i].ox + t3f_get_text_width(*((T3F_FONT **)pp->element[i].resource), pp->element[i].data) / 2;
				}
				else
				{
					*right = pp->element[i].ox + t3f_get_text_width(*((T3F_FONT **)pp->element[i].resource), pp->element[i].data);
				}
			}
			if(bottom)
			{
				*bottom = pp->element[i].oy + t3f_get_font_line_height(*((T3F_FONT **)pp->element[i].resource));
			}
			break;
		}
		case T3F_GUI_ELEMENT_IMAGE:
		{
			if(left)
			{
				*left = pp->element[i].ox;
			}
			if(top)
			{
				*top = pp->element[i].oy;
			}
			if(right)
			{
				*right = pp->element[i].ox + al_get_bitmap_width(*((ALLEGRO_BITMAP **)pp->element[i].resource));
			}
			if(bottom)
			{
				*bottom = pp->element[i].oy + al_get_bitmap_height(*((ALLEGRO_BITMAP **)pp->element[i].resource));
			}
		}
		break;
	}
}

static void allegro_render_element(T3F_GUI * pp, int i, bool hover, int flags)
{
	ALLEGRO_BITMAP * bitmap = NULL;
	T3F_FONT * font = NULL;
	ALLEGRO_COLOR color;
	int sx, sy;

	if(hover)
	{
		sx = pp->element[i].hx;
		sy = pp->element[i].hy;
		color = pp->element[i].active_color;
	}
	else
	{
		sx = 0;
		sy = 0;
		if(!(pp->element[i].flags & T3F_GUI_ELEMENT_STATIC))
		{
			color = pp->element[i].inactive_color;
		}
		else
		{
			color = pp->element[i].color;
		}
	}

	switch(pp->element[i].type)
	{
		case T3F_GUI_ELEMENT_TEXT:
		{
			font = *((T3F_FONT **)pp->element[i].resource);
			if(pp->element[i].flags & T3F_GUI_ELEMENT_SHADOW)
			{
				if(pp->element[i].flags & T3F_GUI_ELEMENT_CENTRE)
				{
					t3f_draw_text(font, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), pp->ox + pp->element[i].ox + pp->element[i].sx, pp->oy + pp->element[i].oy + pp->element[i].sy, 0, T3F_FONT_ALIGN_CENTER, (char *)pp->element[i].data);
					t3f_draw_text(font, color, pp->ox + pp->element[i].ox + sx, pp->oy + pp->element[i].oy + sy, 0, T3F_FONT_ALIGN_CENTER, (char *)pp->element[i].data);
				}
				else
				{
					t3f_draw_text(font, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), pp->ox + pp->element[i].ox + pp->element[i].sx, pp->oy + pp->element[i].oy + pp->element[i].sy, 0, 0, (char *)pp->element[i].data);
					t3f_draw_text(font, color, pp->ox + pp->element[i].ox + sx, pp->oy + pp->element[i].oy + sy, 0, 0, (char *)pp->element[i].data);
				}
			}
			else
			{
				if(pp->element[i].flags & T3F_GUI_ELEMENT_CENTRE)
				{
					t3f_draw_text(font, color, pp->ox + pp->element[i].ox + sx, pp->oy + pp->element[i].oy + sy, 0, T3F_FONT_ALIGN_CENTER, (char *)pp->element[i].data);
				}
				else
				{
					t3f_draw_text(font, color, pp->ox + pp->element[i].ox + sx, pp->oy + pp->element[i].oy + sy, 0, 0, (char *)pp->element[i].data);
				}
			}
			break;
		}
		case T3F_GUI_ELEMENT_IMAGE:
		{
			bitmap = *((ALLEGRO_BITMAP **)pp->element[i].resource);
			if(pp->element[i].flags & T3F_GUI_ELEMENT_SHADOW)
			{
				if(pp->element[i].flags & T3F_GUI_ELEMENT_CENTRE)
				{
					if(bitmap)
					{
						al_draw_tinted_bitmap(bitmap, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), pp->ox + pp->element[i].ox - al_get_bitmap_width(bitmap) / 2, pp->oy + pp->element[i].oy - al_get_bitmap_height(bitmap) / 2, 0);
						al_draw_bitmap(bitmap, pp->ox + pp->element[i].ox - al_get_bitmap_width(bitmap) / 2 + sx, pp->oy + pp->element[i].oy - al_get_bitmap_height(bitmap) / 2 + sy, 0);
					}
				}
				else
				{
					if(bitmap)
					{
						al_draw_tinted_bitmap(bitmap, al_map_rgba_f(0.0, 0.0, 0.0, 0.5), pp->ox + pp->element[i].ox, pp->oy + pp->element[i].oy, 0);
						al_draw_bitmap(bitmap, pp->ox + pp->element[i].ox + sx, pp->oy + pp->element[i].oy + sy, 0);
					}
				}
			}
			else
			{
				if(pp->element[i].flags & T3F_GUI_ELEMENT_CENTRE)
				{
					if(bitmap)
					{
						al_draw_bitmap(bitmap, pp->ox + pp->element[i].ox - al_get_bitmap_width(bitmap) / 2 + sx, pp->oy + pp->element[i].oy - al_get_bitmap_height(bitmap) / 2 + sy, 0);
					}
				}
				else
				{
					if(bitmap)
					{
						al_draw_bitmap(bitmap, pp->ox + pp->element[i].ox + sx, pp->oy + pp->element[i].oy + sy, 0);
					}
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void t3f_set_gui_driver(T3F_GUI_DRIVER * dp)
{
	if(dp == NULL)
	{
		t3f_gui_allegro_driver.get_element_edges = allegro_get_element_edges;
		t3f_gui_allegro_driver.render_element = allegro_render_element;
		t3f_gui_current_driver = &t3f_gui_allegro_driver;
	}
	else
	{
		t3f_gui_current_driver = dp;
	}
}

T3F_GUI * t3f_create_gui(int ox, int oy)
{
	T3F_GUI * lp;
	lp = al_malloc(sizeof(T3F_GUI));
	if(!lp)
	{
		return NULL;
	}
	lp->elements = 0;
	lp->ox = ox;
	lp->oy = oy;
	lp->hover_element = -1;
	lp->font_margin_top = 0;
	lp->font_margin_bottom = 0;
	lp->font_margin_left = 0;
	lp->font_margin_right = 0;
	return lp;
}

void t3f_destroy_gui(T3F_GUI * pp)
{
	int i;

	for(i = 0; i < pp->elements; i++)
	{
		if(pp->element[i].flags & T3F_GUI_ELEMENT_COPY)
		{
			switch(pp->element[i].type)
			{
				case T3F_GUI_ELEMENT_TEXT:
				{
					al_free(pp->element[i].allocated_data);
					break;
				}
				case T3F_GUI_ELEMENT_IMAGE:
				{
					t3f_destroy_resource(pp->element[i].resource);
					break;
				}
			}
			if(pp->element[i].description)
			{
				al_free(pp->element[i].description);
			}
		}
	}
	al_free(pp);
}

int t3f_add_gui_image_element(T3F_GUI * pp, int (*proc)(void *, int, void *), void ** bp, int ox, int oy, int flags)
{
	memset(&pp->element[pp->elements], 0, sizeof(T3F_GUI_ELEMENT));
	pp->element[pp->elements].type = T3F_GUI_ELEMENT_IMAGE;
	pp->element[pp->elements].proc = proc;
	if(flags & T3F_GUI_ELEMENT_COPY)
	{
		t3f_clone_resource(pp->element[pp->elements].resource, *bp);
	}
	else
	{
		pp->element[pp->elements].resource = bp;
	}
	pp->element[pp->elements].ox = ox;
	pp->element[pp->elements].oy = oy;
	pp->element[pp->elements].flags = flags;
	pp->element[pp->elements].description = NULL;
	pp->element[pp->elements].sx = 2;
	pp->element[pp->elements].sy = 2;
	pp->element[pp->elements].hx = -2;
	pp->element[pp->elements].hy = -2;
	pp->elements++;
	return 1;
}

int t3f_add_gui_text_element(T3F_GUI * pp, int (*proc)(void *, int, void *), const char * text, void ** fp, int ox, int oy, ALLEGRO_COLOR color, int flags)
{
	memset(&pp->element[pp->elements], 0, sizeof(T3F_GUI_ELEMENT));
	pp->element[pp->elements].type = T3F_GUI_ELEMENT_TEXT;
	pp->element[pp->elements].proc = proc;
	if(flags & T3F_GUI_ELEMENT_COPY)
	{
		pp->element[pp->elements].allocated_data = al_malloc(strlen(text) + 1);
		memcpy(pp->element[pp->elements].allocated_data, text, strlen(text) + 1);
		pp->element[pp->elements].data = pp->element[pp->elements].allocated_data;
	}
	else
	{
		pp->element[pp->elements].data = text;
	}
	pp->element[pp->elements].resource = fp;
	pp->element[pp->elements].ox = ox;
	pp->element[pp->elements].oy = oy;
	pp->element[pp->elements].color = color;
	pp->element[pp->elements].inactive_color = color;
	pp->element[pp->elements].active_color = color;
	pp->element[pp->elements].flags = flags;
	pp->element[pp->elements].description = NULL;
	pp->element[pp->elements].sx = 2;
	pp->element[pp->elements].sy = 2;
	pp->element[pp->elements].hx = -2;
	pp->element[pp->elements].hy = -2;
	pp->elements++;
	return 1;
}

int t3f_describe_last_gui_element(T3F_GUI * pp, char * text)
{
	if(pp->elements > 0)
	{
		if(pp->element[pp->elements - 1].flags & T3F_GUI_ELEMENT_COPY)
		{
			pp->element[pp->elements - 1].description = al_malloc(strlen(text) + 1);
			strcpy(pp->element[pp->elements - 1].description, text);
		}
		else
		{
			pp->element[pp->elements - 1].description = text;
		}
		return 1;
	}
	return 0;
}

int t3f_get_gui_width(T3F_GUI * pp)
{
	int i;
	int max_width = 0;
	int width;
	int left, right;

	for(i = 0; i < pp->elements; i++)
	{
		t3f_gui_current_driver->get_element_edges(pp, i, &left, NULL, &right, NULL);
		width = right - left;
		if(width > max_width)
		{
			max_width = width;
		}
	}
	return max_width;
}

int t3f_get_gui_height(T3F_GUI * pp, float * top)
{
	int i;
	float itop = 10000.0;
	float bottom = 0.0;
	int edge_top, edge_bottom;

	for(i = 0; i < pp->elements; i++)
	{
		t3f_gui_current_driver->get_element_edges(pp, i, NULL, &edge_top, NULL, &edge_bottom);
		if(edge_top < itop)
		{
			itop = edge_top;
		}
		if(pp->element[i].oy + edge_bottom > bottom)
		{
			bottom = edge_bottom;
		}
	}
	if(top)
	{
		*top = itop;
	}

	return bottom - itop;
}

void t3f_center_gui(T3F_GUI * pp, float oy, float my)
{
	float dheight = my - oy;
	float top;
	float height;
	float offset;

	height = t3f_get_gui_height(pp, &top);
	offset = oy + dheight / 2.0 - height / 2.0;
	pp->oy = offset - top;
}

void t3f_set_gui_shadow(T3F_GUI * pp, float x, float y)
{
	int i;

	for(i = 0; i < pp->elements; i++)
	{
		pp->element[i].sx = x;
		pp->element[i].sy = y;
	}
}

void t3f_set_gui_hover_lift(T3F_GUI * pp, float x, float y)
{
	int i;

	for(i = 0; i < pp->elements; i++)
	{
		pp->element[i].hx = x;
		pp->element[i].hy = y;
	}
}

void t3f_set_gui_element_interaction_colors(T3F_GUI * pp, ALLEGRO_COLOR inactive_color, ALLEGRO_COLOR active_color)
{
	int i;

	for(i = 0; i < pp->elements; i++)
	{
		pp->element[i].inactive_color = inactive_color;
		pp->element[i].active_color = active_color;
	}
}

static bool t3f_gui_check_hover_x(T3F_GUI * pp, int i, float x)
{
	int left, right, width;

	if((pp->element[i].flags & T3F_GUI_ELEMENT_STATIC))
	{
		return false;
	}
	t3f_gui_current_driver->get_element_edges(pp, i, &left, NULL, &right, NULL);
	width = right - left;
	if(x >= pp->ox + left && x < pp->ox + right)
	{
		return true;
	}
	return false;
}

static bool t3f_gui_check_hover_y(T3F_GUI * pp, int i, float y)
{
	int top, bottom;

	if((pp->element[i].flags & T3F_GUI_ELEMENT_STATIC))
	{
		return false;
	}
	t3f_gui_current_driver->get_element_edges(pp, i, NULL, &top, NULL, &bottom);
	if(y >= pp->oy + top && y < pp->oy + bottom)
	{
		return true;
	}
	return false;
}

static bool t3f_gui_check_hover(T3F_GUI * pp, int i, float x, float y)
{
	return t3f_gui_check_hover_x(pp, i, x) && t3f_gui_check_hover_y(pp, i, y);
}

bool t3f_select_hover_gui_element(T3F_GUI * pp, float x, float y)
{
	int i;

	pp->hover_element = -1;
	pp->hover_y = y;
	for(i = 0; i < pp->elements; i++)
	{
		if(t3f_gui_check_hover(pp, i, x, y))
		{
			pp->hover_element = i;
			return true;
		}
	}
	return false;
}

void t3f_select_previous_gui_element(T3F_GUI * pp)
{
	int loop_count = 0;

	while(1)
	{
		pp->hover_element--;
		if(pp->hover_element < 0)
		{
			pp->hover_element = pp->elements - 1;
			loop_count++;
			if(loop_count >= 2)
			{
				pp->hover_element = -1;
				break;
			}
		}
		if(pp->element[pp->hover_element].proc && !(pp->element[pp->hover_element].flags & T3F_GUI_ELEMENT_STATIC))
		{
			break;
		}
	}
}

void t3f_select_next_gui_element(T3F_GUI * pp)
{
	int loop_count = 0;

	while(1)
	{
		pp->hover_element++;
		if(pp->hover_element >= pp->elements)
		{
			pp->hover_element = 0;
			loop_count++;
			if(loop_count >= 2)
			{
				pp->hover_element = -1;
				break;
			}
		}
		if(pp->element[pp->hover_element].proc && !(pp->element[pp->hover_element].flags & T3F_GUI_ELEMENT_STATIC))
		{
			break;
		}
	}
}

void t3f_activate_selected_gui_element(T3F_GUI * pp, void * data)
{
	if(pp->hover_element >= 0 && pp->hover_element < pp->elements)
	{
		if(pp->element[pp->hover_element].proc)
		{
			pp->element[pp->hover_element].proc(data, pp->hover_element, pp);
		}
	}
}

//static bool check_mouse_moved(void)
//{
//	if(fabs(t3f_gui_mouse_x - t3f_get_mouse_x()) < 0.01 && fabs(t3f_gui_mouse_y - t3f_get_mouse_y()) < 0.01)
//	{
//		return false;
//	}
//	return true;
//}

void t3f_reset_gui_input(T3F_GUI * pp)
{
	if(pp)
	{
		pp->hover_element = -1;
	}
	t3f_gui_mouse_x = t3f_get_mouse_x();
	t3f_gui_mouse_y = t3f_get_mouse_y();
}

bool t3f_process_gui(T3F_GUI * pp, int flags, void * data)
{
	int i;
	bool touched = false;
	float mouse_x = 0.0, mouse_y = 0.0;
	bool ret = false;

	if(t3f_flags & T3F_USE_MOUSE && !(flags & T3F_GUI_NO_MOUSE))
	{
		if(t3f_gui_mouse_x <= -100000)
		{
			t3f_reset_gui_input(pp);
		}
		mouse_x = t3f_get_mouse_x();
		mouse_y = t3f_get_mouse_y();
		t3f_gui_mouse_x = t3f_get_mouse_x();
		t3f_gui_mouse_y = t3f_get_mouse_y();
		if(t3f_mouse_button_pressed(0))
		{
			touched = true;
			t3f_use_mouse_button_press(0);
		}
	}

	if(t3f_flags & T3F_USE_TOUCH && !(flags & T3F_GUI_NO_TOUCH))
	{
		if(t3f_touch_pressed(1))
		{
			mouse_x = t3f_get_touch_x(1);
			mouse_y = t3f_get_touch_y(1);
			touched = true;
			t3f_use_touch_press(1);
		}
	}
	if(pp)
	{
		if(!(flags & T3F_GUI_NO_MOUSE) || touched)
		{
			pp->hover_y = mouse_y;
			pp->hover_element = -1;
			for(i = 0; i < pp->elements; i++)
			{
				if(t3f_gui_check_hover(pp, i, mouse_x, mouse_y))
				{
					pp->hover_element = i;
					break;
				}
			}
		}
		else if(pp->hover_element >= 0)
		{
			pp->hover_y = pp->oy + pp->element[pp->hover_element].oy;
		}
		if(touched && pp->hover_element >= 0)
		{
			t3f_activate_selected_gui_element(pp, data);
			ret = true;
		}
	}
	return ret;
}

void t3f_render_gui_element(T3F_GUI * pp, int i, bool hover, int flags)
{
	bool hide = false;

	if((pp->element[i].flags & T3F_GUI_ELEMENT_AUTOHIDE) && !(flags & T3F_GUI_NO_MOUSE) && !t3f_gui_check_hover_y(pp, i, pp->hover_y))
	{
		hide = true;
	}
	if(!hide)
	{
		t3f_gui_current_driver->render_element(pp, i, hover, flags);
	}
}

void t3f_render_gui(T3F_GUI * pp, int flags)
{
	int i;

	if(pp)
	{
		for(i = 0; i < pp->elements; i++)
		{
			if(i != pp->hover_element)
			{
				t3f_render_gui_element(pp, i, false, flags);
			}
		}
		if(pp->hover_element >= 0)
		{
			t3f_render_gui_element(pp, pp->hover_element, true, flags);
		}

		/* render the hover element last so it appears on top */
//		if(pp->hover_element >= 0 && pp->hover_element < pp->elements)
//		{
//			t3f_hyperlink_page_render_element(pp, i, true);
//		}
	}
}
