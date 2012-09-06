#include "t3f.h"

#define T3F_MAX_MEMORY_INFO 1000000

static ALLEGRO_MEMORY_INTERFACE t3f_memory_interface;
int t3f_alloc_count = 0;

void * t3f_malloc(size_t n, int line, const char * file, const char * func)
{
	void * p;
	
	p = malloc(n);
	if(p)
	{
		t3f_alloc_count++;
	}
	return p;
}

void * t3f_realloc(void * ptr, size_t n, int line, const char * file, const char * func)
{
	return realloc(ptr, n);
}

void * t3f_calloc(size_t count, size_t n, int line, const char * file, const char * func)
{
	void * p;
	
	p = calloc(count, n);
	if(p)
	{
		t3f_alloc_count++;
	}
	return p;
}

void t3f_free(void * ptr, int line, const char * file, const char * func)
{
	free(ptr);
	t3f_alloc_count--;
}

void t3f_setup_memory_interface(void)
{
	t3f_memory_interface.mi_malloc = t3f_malloc;
	t3f_memory_interface.mi_free = t3f_free;
	t3f_memory_interface.mi_realloc = t3f_realloc;
	t3f_memory_interface.mi_calloc = t3f_calloc;
	al_set_memory_interface(&t3f_memory_interface);
}
