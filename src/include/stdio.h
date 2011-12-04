/**
 * @file stdio.h
 * @desc standard I/O
 */
#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#ifndef NULL
#define NULL	((void *) 0)
#endif /* !NULL */

/* lib/stdio.c */
void cputchar(int c);
int	getchar(void);
int	iscons(int fd);

/* lib/printfmt.c */
void printfmt(void (*putch)(int, void *), void *putdat, const char *fmt, ...);
void vprintfmt(void (*putch)(int, void *), void *putdat, const char *fmt, va_list);
int	snprintf(char *str, int size, const char *fmt, ...);
int	vsnprintf(char *str, int size, const char *fmt, va_list);

/* lib/printf.c */
int	cprintf(const char *fmt, ...);
int	vcprintf(const char *fmt, va_list);

/* lib/fprintf.c */
int	printf(const char *fmt, ...);
int	fprintf(int fd, const char *fmt, ...);
int	vfprintf(int fd, const char *fmt, va_list);

/* lib/readline.c */
char*	readline(const char *prompt);

#endif /* STDIO_H */