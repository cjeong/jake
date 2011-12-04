/**
 * @file bootmain.c
 * @desc part of the boot sector, along with bootasm.S, which calls 
 *       bootmain(); bootasm.S has put the processor into protected 
 *       32-bit mode; bootmain() loads an ELF kernel image from the 
 *       disk starting at sector 1 and then jumps to the kernel entry 
 *       routine
 */
#include <types.h>
#include <elf.h>
#include <x86.h>

#define SECTSIZE  512

void readsect(void *dst, uint offset);
void readseg(uint va, uint count, uint offset);

void bootmain()
{
  struct elfhdr *elf;
  struct elf_proghdr *ph, *eph;
  uchar *va;

	/* put the boot sector at 0x10000, somewhere in low memory */
  elf = (struct elfhdr *) 0x10000;

  /* read the first page (8 sectors = 4KB) of the disk into <elf> */
  readseg((uint) elf, SECTSIZE * 8, 0);

  /* if not ELF executable, let bootasm.S handle error */
  if(elf->magic != ELF_MAGIC)
    return;

  /* load each program segment (ignores <ph> flags); <ph> is the first 
		 program section header in the elf file and <eph> is the last program
		 section header + 1 */
  ph = (struct elf_proghdr *) ((uchar *) elf + elf->phoff);
  eph = ph + elf->phnum;
  for (; ph < eph; ph++){
		/* read <ph->filesz> bytes starting from <ph->offset> into <va> */
    readseg(ph->va, ph->memsz, ph->offset);
  }

  /* call the entry point from the elf header; does not return */
	((void (*)(void)) (elf->entry & 0xFFFFFF))();
}

void waitdisk(void)
{
  /* wait for disk ready */
  while ((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

/* read a single sector at offset into <dst> */
void readsect(void *dst, uint offset)
{
  /* issue command */
  waitdisk();
  outb(0x1F2, 1);                        /* count = 1 */
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);                     /* cmd 0x20: read sectors */

  /* read data */
  waitdisk();
  insl(0x1F0, dst, SECTSIZE/4);
}

/* read <count> bytes at <offset> from kernel into virtual address <va>;
	 might copy more than asked */
void readseg(uint va, uint count, uint offset)
{
  uint end_va;
	va &= 0xFFFFFF;
  end_va = va + count;

  /* round down to sector boundary */
  va &= ~(SECTSIZE -1);

  /* translate from bytes to sectors; kernel starts at sector 1 */
  offset = (offset / SECTSIZE) + 1;

  /* if this is too slow, we could read lots of sectors at a time;
     we'd write more to memory than asked, but it doesn't matter --
		 we load in increasing order */
  for (; va < end_va; va += SECTSIZE, offset++)
    readsect((char *) va, offset);
}
