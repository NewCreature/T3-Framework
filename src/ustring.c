#include <ctype.h>

#include "t3f.h"
#include "ustring.h"

int t3f_strget(const char * s, int point)
{
    ALLEGRO_USTR_INFO ustr_info;
    const ALLEGRO_USTR * ustr;

    ustr = al_ref_cstr(&ustr_info, s);
    return al_ustr_get(ustr, point);
}

int t3f_strset(char * s, int point, int val, int size)
{
    ALLEGRO_USTR * ustr;
    int ret = -1;

    ustr = al_ustr_new(s);
    if(ustr)
    {
        ret = al_ustr_set_chr(ustr, al_ustr_offset(ustr, point), val);
        if(ret)
        {
            memcpy(s, al_cstr(ustr), size);
        }
        al_ustr_free(ustr);
    }
    return ret;
}

int t3f_strcasecmp(const char * str1, const char * str2)
{
    int i;
    int c1;
    int c2;

    for(i = 0; i < t3f_strlen(str1) && i < t3f_strlen(str2); i++)
    {
        c1 = t3f_strget(str1, i);
        if(c1 >= 'A' && c1 <= 'Z')
        {
            c1 = tolower(c1);
        }
        c2 = t3f_strget(str2, i);
        if(c2 >= 'A' && c2 <= 'Z')
        {
            c2 = tolower(c2);
        }
        if(c1 < c2)
        {
            return -1;
        }
        else if(c1 > c2)
        {
            return 1;
        }
    }
    return 0;
}

int t3f_strcmp(const char * str1, const char * str2)
{
    int i;
    int c1;
    int c2;

    for(i = 0; i < t3f_strlen(str1) && i < t3f_strlen(str2); i++)
    {
        c1 = t3f_strget(str1, i);
        c2 = t3f_strget(str2, i);
        if(c1 < c2)
        {
            return -1;
        }
        else if(c1 > c2)
        {
            return 1;
        }
    }
    return 0;
}

char * t3f_strcat(char * str1, const char * str2, int size)
{
    ALLEGRO_USTR * ustr;

    ustr = al_ustr_new(str1);
    if(ustr)
    {
        if(al_ustr_append_cstr(ustr, str2))
        {
            memcpy(str1, al_cstr(ustr), size);
        }
        al_ustr_free(ustr);
    }
    return str1;
}

char * t3f_strcpy(char * str1, const char * str2, int size)
{
    ALLEGRO_USTR * ustr2;

    ustr2 = al_ustr_new(str2);
    if(ustr2)
    {
        memcpy(str1, al_cstr(ustr2), size);
        al_ustr_free(ustr2);
    }

    return str1;
}

char * t3f_strdup(const char * str, int size)
{
    char * ret;

    ret = al_malloc(size);
    if(ret)
    {
        memcpy(ret, str, size);
    }
    return ret;
}

size_t t3f_strlen(const char * str)
{
    int i;
    int c;

    for(i = 0; 1; i++)
    {
        c = t3f_strget(str, i);
        if(c < 0 || c == '\0')
        {
            break;
        }
    }
    return i;
}

char * t3f_strupr(char * str, int size)
{
    int i;
    int c;

    for(i = 0; i < t3f_strlen(str); i++)
    {
        c = t3f_strget(str, i);
        if(c >= 'a' && c <= 'z')
        {
            t3f_strset(str, i, toupper(c), size);
        }
    }
    return str;
}

char * t3f_strlower(char * str, int size)
{
    int i;
    int c;

    for(i = 0; i < t3f_strlen(str); i++)
    {
        c = t3f_strget(str, i);
        if(c >= 'A' && c <= 'Z')
        {
            t3f_strset(str, i, tolower(c), size);
        }
    }
    return str;
}
