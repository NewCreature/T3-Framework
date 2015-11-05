#ifndef T3F_STRING_H
#define T3F_STRING_H

int t3f_strget(const char * s, int point, int size);
int t3f_strset(char * s, int point, int val, int size);
int t3f_strcasecmp(const char *, int size1, const char *, int size2);
int t3f_strcmp(const char *, int size1, const char *, int size2);
char * t3f_strcat(char *, const char *, int size);
char * t3f_strcpy(char *, const char *, int size);
char * t3f_strdup(const char * s1, int size);
size_t t3f_strlen(const char *, int size);
char * t3f_strupr(char *, int size);
char * t3f_strlower(char *, int size);

#endif
