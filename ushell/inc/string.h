#pragma once

#include <types.h>

int     strlen(const char *s);
int	    strnlen(const char *s, size_t size);
char *	strcpy(char *dst, const char *src);
char *	strncpy(char *dst, const char *src, size_t size);
char *	strcat(char *dst, const char *src);
size_t	strlcpy(char *dst, const char *src, size_t size);
int	    strcmp(const char *s1, const char *s2);
int	    strncmp(const char *s1, const char *s2, size_t size);
char *	strchr(const char *s, int c);
char *	strfind(const char *s, char c);

void *	memset(void *dst, int c, size_t len);
void *	memcpy(void *dst, const void *src, size_t len);
void *	memmove(void *dst, const void *src, size_t len);
int 	memcmp(const void *s1, const void *s2, size_t len);
void *	memfind(const void *s, int c, size_t len);

long	strtol(const char *s, char **endptr, int base);

size_t strspn(const char *s1,const char *s2);
char * strpbrk(const char *s1,const char *s2);
//strtok
char * strtok_inside(char* str_arr,const char* delimiters,char**temp_str);
char * strtok(char* str_arr,const char* delimiters);