#include "t3f.h"
#include "resource.h"

static T3F_RESOURCE * t3f_resource[T3F_MAX_RESOURCES] = {NULL};
static int t3f_resources = 0;

void * t3f_bitmap_resource_handler_proc(ALLEGRO_FILE * fp, const char * filename, int option, int flags, unsigned long offset)
{
	void * ptr = NULL;
	ALLEGRO_STATE old_state;
	bool openfp = false; // operating on already open file
	int old_flags;
	
	old_flags = al_get_new_bitmap_flags();
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE | (old_flags & ALLEGRO_NO_PREMULTIPLIED_ALPHA));
	if(fp)
	{
		openfp = true;
	}
	if(!openfp && offset == 0)
	{
		ptr = al_load_bitmap(filename);
	}
	else
	{
		if(!openfp)
		{
			fp = al_fopen(filename, "rb");
			al_fseek(fp, offset, ALLEGRO_SEEK_SET);
		}
		if(fp)
		{
			ptr = t3f_load_bitmap_f(fp);
			if(!openfp)
			{
				al_fclose(fp);
			}
		}
	}
	al_restore_state(&old_state);
	return ptr;
}

void t3f_bitmap_resource_handler_destroy_proc(void * ptr)
{
	al_destroy_bitmap(ptr);
}

void * t3f_font_resource_handler_proc(ALLEGRO_FILE * fp, const char * filename, int option, int flags, unsigned long offset)
{
	void * ptr = NULL;
	ALLEGRO_STATE old_state;
	bool openfp = false;
	
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE);
	if(fp)
	{
		openfp = true;
	}
	
	/* load file directly if offset is 0 and open file not passed */
	else if(offset == 0)
	{
		ptr = al_load_ttf_font(filename, option, flags);
	}
	else
	{
		if(!openfp)
		{
			fp = al_fopen(filename, "rb");
			if(fp)
			{
				al_fseek(fp, offset, ALLEGRO_SEEK_SET);
			}
		}
		if(fp)
		{
			ptr = al_load_ttf_font_f(fp, filename, option, flags);
			if(!openfp)
			{
				al_fclose(fp);
			}
		}
	}
	al_restore_state(&old_state);
	return ptr;
}

void t3f_font_resource_handler_destroy_proc(void * ptr)
{
	al_destroy_font(ptr);
}

void * t3f_bitmap_font_resource_handler_proc(ALLEGRO_FILE * fp, const char * filename, int option, int flags, unsigned long offset)
{
	void * ptr = NULL;
	ALLEGRO_STATE old_state;
	
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE);
	ptr = al_load_bitmap_font_flags(filename, flags);
	al_restore_state(&old_state);
	return ptr;
}

void t3f_bitmap_font_resource_handler_destroy_proc(void * ptr)
{
	al_destroy_font(ptr);
}

void * t3f_t3f_font_gen_resource_handler_proc(ALLEGRO_FILE * fp, const char * filename, int option, int flags, unsigned long offset)
{
	void * ptr = NULL;
	ALLEGRO_STATE old_state;
	
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE);
	if(!fp && offset == 0)
	{
		ptr = t3f_generate_font(filename, option, flags);
	}
	al_restore_state(&old_state);
	return ptr;
}

void t3f_t3f_font_gen_resource_handler_destroy_proc(void * ptr)
{
	t3f_destroy_font(ptr);
}

void * t3f_t3f_font_load_resource_handler_proc(ALLEGRO_FILE * fp, const char * filename, int option, int flags, unsigned long offset)
{
	void * ptr = NULL;
	ALLEGRO_STATE old_state;
	
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_NO_PRESERVE_TEXTURE);
	if(!fp && offset == 0)
	{
		ptr = t3f_load_font(filename, flags);
	}
	al_restore_state(&old_state);
	return ptr;
}

void t3f_t3f_font_load_resource_handler_destroy_proc(void * ptr)
{
	t3f_destroy_font(ptr);
}

static T3F_RESOURCE_HANDLER t3f_resource_handler[T3F_MAX_RESOURCE_HANDLERS] = 
{
	{t3f_bitmap_resource_handler_proc, t3f_bitmap_resource_handler_destroy_proc, 0},	
	{t3f_font_resource_handler_proc, t3f_font_resource_handler_destroy_proc, 0},	
	{t3f_bitmap_font_resource_handler_proc, t3f_bitmap_font_resource_handler_destroy_proc, 0},	
	{t3f_t3f_font_gen_resource_handler_proc, t3f_t3f_font_gen_resource_handler_destroy_proc, 0},	
	{t3f_t3f_font_load_resource_handler_proc, t3f_t3f_font_load_resource_handler_destroy_proc, 0},	
};

void t3f_register_resource_handler(int type, void * (*proc)(ALLEGRO_FILE * fp, const char * filename, int option, int flags, unsigned long offset), void (*destroy_proc)(void * ptr))
{
	t3f_resource_handler[type].proc = proc;
	t3f_resource_handler[type].destroy_proc = destroy_proc;
}

static bool t3f_add_resource(int type, void ** ptr, const char * filename, int option, int flags, unsigned long offset)
{
	if(t3f_resources < T3F_MAX_RESOURCES)
	{
		t3f_resource[t3f_resources] = al_malloc(sizeof(T3F_RESOURCE));
		if(t3f_resource[t3f_resources])
		{
			t3f_resource[t3f_resources]->type = type;
			t3f_resource[t3f_resources]->ptr = ptr;
			strcpy(t3f_resource[t3f_resources]->filename, filename);
			t3f_resource[t3f_resources]->offset = offset;
			t3f_resource[t3f_resources]->option = option;
			t3f_resource[t3f_resources]->flags = flags;
			t3f_resources++;
			return true;
		}
	}
	return false;
}

void t3f_remove_resource(int i)
{
	int j;
	
	al_free(t3f_resource[i]);
	for(j = i; j < t3f_resources - 1; j++)
	{
		t3f_resource[j] = t3f_resource[j + 1];
	}
	t3f_resources--;
}

void * t3f_load_resource(void ** ptr, int type, const char * filename, int option, int flags, unsigned long offset)
{
	if(t3f_resource_handler[type].proc)
	{
		*ptr = t3f_resource_handler[type].proc(NULL, filename, option, flags, offset);
		if(*ptr)
		{
			t3f_add_resource(type, ptr, filename, option, flags, offset);
		}
	}
	return *ptr;
}

void * t3f_load_resource_f(void ** ptr, int type, ALLEGRO_FILE * fp, const char * filename, int option, int flags)
{
	unsigned long offset;
	if(t3f_resource_handler[type].proc)
	{
		offset = al_ftell(fp);
		*ptr = t3f_resource_handler[type].proc(fp, filename, option, flags, offset);
		if(*ptr)
		{
			t3f_add_resource(type, ptr, filename, option, flags, offset);
		}
	}
	return *ptr;
}

static void t3f_actually_unload_resource(int i)
{
	if(t3f_resource_handler[t3f_resource[i]->type].destroy_proc)
	{
		t3f_resource_handler[t3f_resource[i]->type].destroy_proc(*t3f_resource[i]->ptr);
		*t3f_resource[i]->ptr = NULL;
	}
}

int t3f_unload_resource(void * ptr)
{
	int i;
	
	for(i = 0; i < t3f_resources; i++)
	{
		if(*t3f_resource[i]->ptr == ptr)
		{
			t3f_actually_unload_resource(i);
			return i;
		}
	}
	return -1;
}

void t3f_destroy_resource(void * ptr)
{
	int i;
	
	i = t3f_unload_resource(ptr);
	if(i >= 0)
	{
		t3f_remove_resource(i);
	}
}

void t3f_unload_resources(void)
{
	int i;
	
	for(i = 0; i < t3f_resources; i++)
	{
		if(*t3f_resource[i]->ptr)
		{
			t3f_actually_unload_resource(i);
		}
	}
}

void t3f_reload_resources(void)
{
	int i;
	
	for(i = 0; i < t3f_resources; i++)
	{
		if(t3f_resource_handler[t3f_resource[i]->type].proc)
		{
			*t3f_resource[i]->ptr = t3f_resource_handler[t3f_resource[i]->type].proc(NULL, t3f_resource[i]->filename, t3f_resource[i]->option, t3f_resource[i]->flags, t3f_resource[i]->offset);
		}
	}
}
