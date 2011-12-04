/** 
 * @file string.h
 * @desc
 */
#ifndef STRING_H
#define STRING_H

#include <types.h>

int strlen(const char *s);
int strnlen(const char *s, size_t size);

char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t size);
size_t  strlcpy(char *dst, const char *src, size_t size);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t size);

char *strchr(const char *s, char c);
char *strfind(const char *s, char c);

long  strtol(const char *s, char **endptr, int base);

/* no memcpy; use memmove instead */
void *memset(void *dst, int c, size_t len);
void *memmove(void *dst, const void *src, size_t len);
int memcmp(const void *s1, const void *s2, size_t len);
void *memfind(const void *s, int c, size_t len);

#endif /* STRING_H */
