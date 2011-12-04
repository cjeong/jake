/* See COPYRIGHT for copyright information. */

/* The Run Time Clock and other NVRAM access functions that go with it. */
/* The run time clock is hard-wired to IRQ8. */

#include <x86.h>
#include <kern/kclock.h>


unsigned mc146818_read(void *sc, unsigned reg)
{
	outb(IO_RTC, reg);
	return inb(IO_RTC+1);
}

void mc146818_write(void *sc, unsigned reg, unsigned datum)
{
	outb(IO_RTC, reg);
	outb(IO_RTC+1, datum);
}



