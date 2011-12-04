/**
 * @file init.c
 * @desc kernel init code; i386_init is the beginning of all kernel
 *       code, which is loaded/executed by the bootloader
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <kern/pmap.h>
#include <kern/kclock.h>
#include <kern/monitor.h>
#include <kern/console.h>

/* test the stack backtrace function */
void test_backtrace(int x)
{
	cprintf("Entering test_backtrace %d\n", x);
	if (x > 0)
		test_backtrace(x-1);
	else
		mon_backtrace(0, 0, 0);
	cprintf("Leaving test_backtrace %d\n", x);
}

void i386_init(void)
{
	extern char edata[], end[];

	/* before doing anything else, complete the ELF loading process;
		 clear the uninitialized global data (BSS) section of our program;
		 this ensures that all static/global variables start out zero */
	memset(edata, 0, end - edata);

	/* initialize the console; can't call cprintf until after we do this */
	cons_init();

	cprintf("\n*** Welcome to Jake, The Java Kernel! ***");
	cprintf("\nCopyright (C) 2011. Enterprise Java Systems.\n\n");

	/* test the stack backtrace function */
	test_backtrace(5);

  /* memory setup */
  i386_detect_memory();
  i386_vm_init();

	/* drop into the kernel monitor */
	while (1)
		monitor(NULL);
}


/* variable panicstr contains argument to first call to panic; used as flag
	 to indicate that the kernel has already called panic */
static const char *panicstr;

/* panic is called on unresolvable fatal errors; it prints "panic: mesg", 
	 and then enters the kernel monitor */
void _panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	if (panicstr)
		goto dead;
	panicstr = fmt;

	va_start(ap, fmt);
	cprintf("Kernel panic at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);

dead:
	/* break into the kernel monitor */
	while (1)
		monitor(NULL);
}

/* like panic, but don't */
void _warn(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	va_start(ap, fmt);
	cprintf("Kernel warning at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);
}
