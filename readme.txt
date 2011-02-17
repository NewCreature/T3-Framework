T^3 Framework v0.9 (c)2010 T^3 Software
---------------------------------------

T^3 Framework (T3F) is a collection of routines which are designed to simplify
the process of making a game. It utilizes Allegro 5 under the hood but allows
you to write a program without having to deal with the new event system.

T3F is divided into modules which handle different tasks. These will be broken
down below. You only need to link the modules you use to your project.


t3f.c
-----

This is the main module which handles the events and processes your logic and
rendering routines. Here is a list of functions and what they do:

int t3f_initialize(const char * name, int w, int h, double fps,
	void (*logic_proc)(), void (*render_proc)(), int flags)

	Initialize T3F with the given parameters.
	
	'name' is the name of the program. The window title will be set to this and
	the configuration file will be stored in a folder with this name.
	
	'w' and 'h' are the virtual display dimensions. These determine how big the
	drawing area is. This is independant of the display size. If your display is
	a different size all drawing will be stretched accordingly.
	
	'fps' is the rate at which the logic routine is run. Typically a value of 60
	will give good results.
	
	'logic_proc' is a pointer to your logic routine.
	
	'render_proc' is a pointer to your render routine.
	
	'flags' are described below:
	
		T3F_USE_KEYBOARD
		
			Your program will be using the keyboard.
			
		T3F_USE_MOUSE
		
			Your program will be using the mouse.
			
		T3F_USE_JOYSTICK
		
			Your program will be using the joystick(s).
			
		T3F_USE_SOUND
		
			Your program will be using sound.
			
		T3F_USE_FULLSCREEN
		
			Your program will be full screen.
			
		T3F_RESIZABLE
		
			Your program will have a resizable window.
			
		T3F_FORCE_ASPECT
		
			Your program will maintain its aspect ratio when resized.
			
		T3F_USE_CONSOLE
		
			Your program will not create a window.
			
		T3F_DEFAULT
		
			Same as using "T3F_USE_KEYBOARD | T3F_USE_SOUND."
	
int t3f_set_gfx_mode(int w, int h, int flags)

	Changes the size of the display. Accepts the display flags listed above.
	
void t3f_exit(void)

	Shuts down T3F. Call this somewhere in your logic routine (e.g. when user
	presses Escape).

void t3f_run(void)

	Start the main program loop. This is where all the events are handled and
	your logic and render routines are called from. This function will not
	return until a call to t3f_exit().
	

T3F provides keyboard access similar to Allegro 4. Use the t3f_key[] array to
check if a specific key is pressed (see Allegro 5 docs for key values). Buffered
input is also provided using the following functions:

void t3f_clear_keys(void)

	Clear the keyboard buffer.

bool t3f_add_key(char key)

	Add a key to the keyboard buffer.

char t3f_read_key(int flags)

	Read the next key from the keyboard buffer. Returns 0 if there are no keys
	in the buffer.
	

T3F has a few contains functions which allow you to create some modest 3D
effects. Below is a list of structures and functions and their descriptions:

T3F_VIEW

	This structure hold projection data which is needed for all of the 3D
	functions.

T3F_VIEW * t3f_create_view(float ox, float oy, float w, float h, float vpx,
	float vpy)

	Allocate a T3F_VIEW with the specified parameters. 'ox' and 'oy' represent
	the offset of the viewport in relation to T3F's virtual display. 'w' and 'h'
	are the viewport's dimensions. 'vpx' and 'vpy' are the coordinates of the
	vanishing point.
	
void t3f_destroy_view(T3F_VIEW * vp)

	Frees the passed T3F_VIEW from memory.

void t3f_select_view(T3F_VIEW * vp)

	Select the specified T3F_VIEW. Any drawing operation called after this,
	including Allegro's own drawing functions, will be scaled and offset so that
	the coordinate (0, 0) represents the upper left corner of the drawing area
	and (t3f_virtual_display_width, t3f_virtual_display_height) falls just
	outside the drawing area.

float t3f_project_x(float x, float z)

	Given 'x' and 'z,' return x as projected onto the screen plane.

float t3f_project_y(float y, float z)

	Given 'y' and 'z,' return y as projected onto the screen plane.

void t3f_draw_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float x, float y,
	float z, int flags)

	Same as al_draw_tinted_bitmap() except it accepts a 'z' coordinate as well.

void t3f_draw_rotated_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float cx,
	float cy, float x, float y, float z, float angle, int flags)

	Same as al_draw_tinted rotated_bitmap() except it accepts a 'z' coordinate
	as well.

void t3f_draw_rotated_scaled_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color,
	float cx, float cy, float x, float y, float z, float angle, float scale_x,
	float scale_y, 	int flags)

	Same as al_draw_tinted_rotated_scaled_bitmap() except it accepts a 'z'
	coordinate as well.

void t3f_draw_scaled_bitmap(ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, float x,
	float y, float z, float w, float h, int flags);

	Same as al_draw_tinted_scaled_bitmap() except it accepts a 'z' coordinate as
	well.


T3F has a few functions to help you create sprite and tile atlases. If your game
has a lot of sprites, placing them on a sprite sheet can greatly improve the
speed of your program. Hardware accelerated rendering is fast but switching
textures often can dramatically slow down your game. Use this in conjunction
with Allegro's al_hold_bitmap_drawing() function and you can draw thousands of
sprites without breaking a sweat. Here's a list of functions and what they do:

T3F_ATLAS * t3f_create_atlas(int w, int h, int type)

	Create an atlas of the specified width, height, and type. 'type' refers to
	the kind of data this atlas will hold. 'T3F_ATLAS_TILES' sets up the atlas
	as tiles for a tilemap. 'T3F_ATLAS_SPRITES' sets up the atlas as regular
	sprites.
	
	Example usage:
	
		T3F_ATLAS * atlas = t3f_create_atlas(512, 512, T3F_ATLAS_SPRITES);
		if(atlas)
		{
			/* code to add
			t3f_start_sprite_sheet(sprite_sheet, T3F_SPRITE_SHEET_TYPE_SPRITES);
		}

ALLEGRO_BITMAP * t3f_add_bitmap_to_atlas(T3F_ATLAS * ap, ALLEGRO_BITMAP * bp)

	This function returns a pointer to an ALLEGRO_BITMAP that contains the
	bitmap data from 'bp'. Returns NULL if there isn't room in the atlas to
	store a copy of the passed ALLEGRO_BITMAP.

void t3f_destroy_atlas(T3F_ATLAS * ap)

	Destroy the specified atlas.


T3F provides a few miscellaneous functions that are useful for game development
that aren't provided by Allegro 5. These are:

float t3f_distance(float x1, float y1, float x2, float y2)

	Find the distance between two points.

double t3f_drand(void)

	Returns a random number in the range 0 to 1.

float t3f_fread_float(ALLEGRO_FILE * fp)

	Read a float from a file.

int t3f_fwrite_float(ALLEGRO_FILE * fp, float f)

	Write a float to a file.

ALLEGRO_FONT * t3f_load_font(const char * fn)

	Load an anti-aliased bitmap font.

ALLEGRO_FILE * t3f_open_file(ALLEGRO_PATH * pp, const char * fn, const char * m)

	Open a file in the specified path.

unsigned long t3f_checksum_file(const char * fn)

	Create a 32-bit checksum of the specified file.


music.c
-------

This module handles playback of streaming music (e.g. Vorbis). Here is a list of
functions and what they do:

bool t3f_play_music(char * fn)

	Play the specified file. Looping data can be stored in a corresponding INI
	file (e.g. loop data will be read from "bgm.ini" if you play "bgm.ogg").
	
	The INI format is:
	
		[loop]
		start=0.0
		end=86.5

	'start' and 'end' are specified in seconds.

void t3f_stop_music(void)

	Stop the playing music, closing the stream and freeing all associated
	resources.

void t3f_pause_music(void)

	Pause the playing music.
	
void t3f_resume_music(void)

	Unpause the music.

void t3f_set_music_volume(float volume)

	Set the music playback volume.

float t3f_get_music_volume(void)

	Get the music playback volume.


sound.c
-------

This module contains a few useful sound functions which offer greater simplicity
than the standard Allegro 5 functions. Here is a list of functions and what they
do:

void t3f_set_sound_volume(float volume)

	Set the global sound volume. All sounds will be played back at this volume
	multiplied by the volume passed to t3f_play_sample().

float t3f_get_sound_volume(void)

	Get the global sound volume.

bool t3f_play_sample(ALLEGRO_SAMPLE * sp, float vol, float pan, float speed)

	Play a sample. 'vol' is the volume, 'pan' is the position of the sample (in
	the range -1 to 1), and 'speed' is the playback speed.

T3F provides a sound queueing system, useful when you need sounds to play back
to back (for a commentary, character voices, or similar situation). These
functions are described below:

bool t3f_queue_sample(ALLEGRO_SAMPLE * sp)

	Add a sample to the queue. If there are currently no sounds in the queue,
	this sample will begin playing immediately.

void t3f_clear_sample_queue(void)

	Clear all samples from the queue.

ALLEGRO_SAMPLE * t3f_get_queue_sample(void)

	Returns the next sample in the queue.

void t3f_poll_sound_queue(void)

	If you are using the queue functions, you will need to call this at regular
	intervals in order for the queue to be processed.

The functions below are useful for position and fading sounds based on the sound
source's position in relation to the listener.

float t3f_get_sound_position(float earx, float eary, float soundx, float soundy)

	Given the position of the listener ('earx' and 'eary') and the position of
	the sound ('soundx' and 'soundy'), this function returns the correct panning
	to be passed to the sample playing functions.

float t3f_get_sound_gain(float earx, float eary, float soundx, float soundy,
	float scale)

	Given the position of the listener ('earx' and 'eary'), the position of the
	sound ('soundx' and 'soundy'), and a scale, this function returns the volume
	level of the sound. A sound that is greater than 'scale' distance away from
	the listener will not be heard.


controller.c
------------

This module provides a simple way to deal with controllers. Controllers may
contain any number of bindings which can be linked to keyboard, joystick, or
the mouse. Here's a list of structures and functions:

T3F_CONTROLLER

	Holds data for one controller. If your game has multiplayer you will usually
	want one of these for each player.

T3F_CONTROLLER * t3f_create_controller(int bindings)

	Create a controller with the specified number of bindings.

void t3f_destroy_controller(T3F_CONTROLLER * cp)

	Free resources used by a controller.

bool t3f_bind_controller(T3F_CONTROLLER * cp, int binding)

	A blocking function which waits for the user to press a key, joystick button
	or axis, or mouse button and sets the specified binding accordingly.

const char * t3f_get_controller_binding_name(T3F_CONTROLLER * cp, int binding)

	Returns the name of the binding, useful if you need to display the controls.

void t3f_read_controller(T3F_CONTROLLER * cp)

	Read the current state of the controller. Does not update the entire state,
	only determines which buttons, keys, etc. are currently held.

void t3f_update_controller(T3F_CONTROLLER * cp)

	Update controller state based on data retrieved by t3f_read_controller().
	Usually you will want to call t3f_read_controller() prior to calling this.
	
	Example usage:
	
		t3f_read_controller(&my_controller);
		t3f_update_controller(&my_controller);
		if(cp->state[0].pressed) // button was just pressed
		{
			...
		}
		if(cp->state[1].released) // button was just released
		{
			...
		}
		if(cp->state[2].held) // button is currently held
		{
			...
		}

void t3f_clear_controller_state(T3F_CONTROLLER * cp)

	Reset the controller state.


animation.c
-----------

This module provides a way to handle animations, including load and saving
animation files, creating animations from a series of bitmaps, and drawing
animations. Below is a list of structures and functions and their descriptions:

T3F_ANIMATION

	Holds data for one animation. Animations consist of ALLEGRO_BITMAPs and
	T3F_ANIMATION_FRAMEs which specify the bitmap and its orientation.

T3F_ANIMATION_FRAME

	This structure has several members which define how to display this frame.
	'bitmap' is the index to the bitmap this frame uses. 'x,' 'y,' and 'z'
	specify the placement of the bitmap in the frame. 'width' and 'height'
	define how the bitmap will be stretched. 'angle' defines how the bitmap will
	be rotated.

There are several functions which handle the creation and destruction of
animations. These are described below:

T3F_ANIMATION * t3f_create_animation(void)

	Create an empty animation.

T3F_ANIMATION * t3f_clone_animation(T3F_ANIMATION * ap)

	Create a copy of an animation.

void t3f_destroy_animation(T3F_ANIMATION * ap)

	Free all resources used by an animation.

T3F_ANIMATION * t3f_load_animation_f(ALLEGRO_FILE * fp)

	Load an animation from an open file. The file must already be opened and the
	file will remain open after the animation is loaded. This is useful if you
	are using animations as part of a larger data structure.

T3F_ANIMATION * t3f_load_animation(const char * fn)

	Load an animation from the specified file.

T3F_ANIMATION * t3f_load_animation_from_bitmap(const char * fn)

	Load the specified bitmap file as an animation.

int t3f_save_animation_f(T3F_ANIMATION * ap, ALLEGRO_FILE * fp)

	Save an animation to an open file. The file must alread be opened and the
	file will remain open after the animation is saved.

int t3f_save_animation(T3F_ANIMATION * ap, const char * fn)

	Save the specified animation to the specified file.

These functions are used for building animations on the fly:

int t3f_animation_add_bitmap(T3F_ANIMATION * ap, ALLEGRO_BITMAP * bp)

	Add the specified bitmap to the animation.

int t3f_animation_delete_bitmap(T3F_ANIMATION * ap, int bitmap)

	Remove the specified bitmap from the animation.

int t3f_animation_add_frame(T3F_ANIMATION * ap, int bitmap, float x, float y, float z, float w, float h, float angle, int ticks)

	Add a new frame to the specified animation with the specified parameters.

int t3f_animation_delete_frame(T3F_ANIMATION * ap, int frame)

	Delete the specified frame from the animation.

void t3f_add_animation_to_sprite_sheet(T3F_ANIMATION * ap)

	Moves an animation's bitmaps to the current sprite sheet.
	
	Example usage:
	
	my_sprite_sheet = al_create_bitmap(512, 512);
	if(!my_sprite_sheet)
	{
		return;
	}
	t3f_start_sprite_sheet(my_sprite_sheet);
	my_animation = t3f_load_animation("example.ani");
	if(!my_animation)
	{
		return;
	}
	t3f_add_animation_to_sprite_sheet(my_animation);
	t3f_finish_sprite_sheet();
	
Several functions are provided which aid with drawing animations. These are as
follows:

ALLEGRO_BITMAP * t3f_animation_get_bitmap(T3F_ANIMATION * ap, int tick)

	Get the bitmap based on the specified tick. Looping is handled for you. It
	'tick' is 0 it will display the first frame.

T3F_ANIMATION_FRAME * t3f_animation_get_frame(T3F_ANIMATION * ap, int tick)

	Get the animation frame based on the specified tick.

void t3f_draw_animation(T3F_ANIMATION * ap, ALLEGRO_COLOR color, int tick,
	float x, float y, float z, int flags);

	Draw the animation at the specified location using 'tick' to determine which
	frame to display.

void t3f_draw_scaled_animation(T3F_ANIMATION * ap, ALLEGRO_COLOR color,
	int tick, float x, float y,	float z, float scale, int flags);
	
	Same as t3f_draw_animation() except it allows you to specify a scale factor.
	
void t3f_draw_rotated_animation(T3F_ANIMATION * ap, ALLEGRO_COLOR color,
	int tick, float cx, float cy, float x, float y, float z, float angle,
	int flags)

	Same as t3f_draw_animation() except it allows you to specify an angle to
	rotate the animation by.

void t3f_draw_rotated_scaled_animation(T3F_ANIMATION * ap, ALLEGRO_COLOR color,
	int tick, float cx, float cy, float x, float y, float z, float angle,
	float scale, int flags);
	
	Same as t3f_draw_animation() except it allows you to specify a scale factor
	and angle of rotation.


collision.c
-----------

Routines for dealing with object-to-object and object-to-tile collisions. There
are a lot of functions here but I will only list some here for simplicity. When
this documentation is made more formal all the functions will be documented.

Here are the structures used for detecting collisions:

T3F_COLLISION_OBJECT

	A collision object. This is what you tie to your in-game sprites so that
	they can interact.

T3F_COLLISION_TILEMAP

	A map of collision information that can be used to define how your sprites
	interact with the game world.

Here are some of the most useful functions:

T3F_COLLISION_OBJECT * t3f_create_collision_object(float rx, float ry, float w,
	float h, int tw, int th, int flags)
	
	Create a collision object. (rx, ry) are the coordinates of the upper left
	corner of the collision object as they relate to the actual sprite. 'w' and
	'h' are the width and height of the collision object. 'tw' and 'th' are the
	width and height of tiles in a T3F_COLLISION_TILEMAP that the object needs
	to interact with. If you are not using T3F_COLLISION_TILEMAPs you can safely
	set these to 32.
	
void t3f_recreate_collision_object(T3F_COLLISION_OBJECT * cp, float rx,
	float ry, float w, float h, int tw, int th, int flags)
	
	Same as t3f_create_collision_object() but instead updates an existing
	T3F_COLLISION_OBJECT.
	
void t3f_destroy_collision_object(T3F_COLLISION_OBJECT * cp)

	Destroy the object, freeing all memory used by it.

void t3f_move_collision_object_x(T3F_COLLISION_OBJECT * cp, float x)

	Set the x-coordinate of the collision object.

void t3f_move_collision_object_y(T3F_COLLISION_OBJECT * cp, float y)

	Set the y-coordinate of the collision object.

void t3f_move_collision_object_xy(T3F_COLLISION_OBJECT * cp, float x, float y)

	Set the position of the collision object. If your objects are going to be
	obstructed from moving in certain situations you should move along the x and
	y axes separately (see ex_collision). Otherwise it is safe to use this
	function instead.

int t3f_check_object_collision(T3F_COLLISION_OBJECT * cp1,
	T3F_COLLISION_OBJECT * cp2)
	
	See if the two objects collided.


tilemap.c
---------

A collection of routines dealing with tilemaps. Below is the list of functions
you will need to use. Lower level functions will not be documented here.

T3F_TILESET * t3f_load_tileset(const char * fn);

	Load a tileset from a file on disk.

T3F_TILESET * t3f_load_tileset_f(ALLEGRO_FILE * fp);

	Load a tileset from an open ALLEGRO_FILE. This is useful if you are storing
	a tileset as part of your own level file format.

int t3f_save_tileset(T3F_TILESET * tsp, const char * fn);

	Save a tileset to a file on disk.

int t3f_save_tileset_f(T3F_TILESET * tsp, ALLEGRO_FILE * fp)

	Save a tileset to an open ALLEGRO_FILE. This is useful if you are storing a
	tileset as part of your own level file format.

bool t3f_atlas_tileset(T3F_TILESET * tsp)

	Create a tile atlas for the tileset. This will greatly increase the speed at
	which your tilemaps are drawn.

T3F_TILEMAP * t3f_load_tilemap(const char * fn);

	Load a tilemap from a file on disk.

T3F_TILEMAP * t3f_load_tilemap_f(ALLEGRO_FILE * fp);

	Load a tilemap from an open ALLEGRO_FILE. This is useful if you are storing
	a tilemap as part of your own level file format.

int t3f_save_tilemap(T3F_TILEMAP * tmp, const char * fn);

	Save a tilemap to a file on disk.

int t3f_save_tilemap_f(T3F_TILEMAP * tmp, ALLEGRO_FILE * fp);

	Save a tilemap to an open ALLEGRO_FILE. This is useful if you are storing a
	tilemap as part of your own level file format.

void t3f_render_tilemap(T3F_TILEMAP * tmp, T3F_TILESET * tsp, int layer,
	int tick, float ox, float oy, float oz, ALLEGRO_COLOR color);

	Render a layer of the tilemap. 'tick' is the animation tick used to
	determine which frames of the tile animations to draw. 'color' is a tint
	color to be applied to all drawn tiles. This is useful for drawing a layer
	at half transparency, darkened, etc.


gui.c
-----

This module provides a simple menuing system which allows you to set up a menu
quickly. Here's a list of structures and functions:

T3F_GUI_DRIVER

	A GUI system driver. Make your own driver by filling in the three members
	with your own functions. Members are:
	
		float(*get_element_width)(T3F_GUI_ELEMENT * ep);
		float(*get_element_height)(T3F_GUI_ELEMENT * ep);
		void(*render_element)(T3F_GUI * pp, int i, bool hover);

T3F_GUI

	Contains one page of menu items.

void t3f_set_gui_driver(T3F_GUI_DRIVER * dp)

	Select the driver to be used by the GUI system. Pass NULL to use the default
	Allegro 5 driver which uses ALLEGRO_FONTs and ALLEGRO_BITMAPs for the
	various elements. You must call this before using t3f_process_gui() or
	t3f_render_gui().

T3F_GUI * t3f_create_gui(int ox, int oy)

	Create a blank page at the specified location.

void t3f_destroy_gui(T3F_GUI * pp)

	Free all resources used by a page.

int t3f_add_gui_image_element(T3F_GUI * pp, int (*proc)(int, void *),
	ALLEGRO_BITMAP * bp, int ox, int oy, int flags)

	Add an image element to a GUI. The image will be displayed at the specified
	location within the page. Flags can be:
	
		T3F_GUI_ELEMENT_CENTER
		T3F_GUI_ELEMENT_SHADOW
		T3F_GUI_ELEMENT_AUTOHIDE
		T3F_GUI_ELEMENT_COPY
		T3F_GUI_ELEMENT_STATIC

int t3f_add_gui_text_element(T3F_GUI * pp, int (*proc)(int, void *),
	char * text, ALLEGRO_FONT * fp, int ox, int oy, ALLEGRO_COLOR color,
	int flags);
	
	Add a text element to a GUI. The text will be displayed at the specified
	location using the specified font. Flags can be:
	
		T3F_GUI_ELEMENT_CENTER
		T3F_GUI_ELEMENT_SHADOW
		T3F_GUI_ELEMENT_AUTOHIDE
		T3F_GUI_ELEMENT_COPY
		T3F_GUI_ELEMENT_STATIC
	
void t3f_center_gui(T3F_GUI * pp, float oy, float my)

	Vertically center a GUI. The GUI's center will be the center point between
	'oy' and 'my.'

void t3f_process_gui(T3F_GUI * pp)

	Handle the GUI logic. Clicking on an element will call the element's proc()
	if it has one.

void t3f_render_gui(T3F_GUI * pp)

	Render the page.
