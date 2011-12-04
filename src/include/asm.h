/**
 * @file asm.h
 * @desc assembler macros used to fill up the x86 segment descriptor table
 */

/* for NULL segment descriptor */
#define SEG_NULLASM                                             \
  .word 0, 0;                                                   \
  .byte 0, 0, 0, 0


/* PC address space after startup:

   +------------------+  <- 0xFFFFFFFF (4GB)
   |      32-bit      |
   |  memory mapped   |
   |     devices      |
   |                  |
   /\/\/\/\/\/\/\/\/\/\
   
   /\/\/\/\/\/\/\/\/\/\
   |                  |
   |      Unused      |
   |                  |
   +------------------+  <- depends on amount of RAM
   |                  |
   |                  |
   | Extended Memory  |
   |                  |
   |                  |
   +------------------+  <- 0x00100000 (1MB)
   |     BIOS ROM     |
   +------------------+  <- 0x000F0000 (960KB)
   |  16-bit devices, |
   |  expansion ROMs  |
   +------------------+  <- 0x000C0000 (768KB)
   |   VGA Display    |
   +------------------+  <- 0x000A0000 (640KB)
   |                  |
   |    Low Memory    |  <==== 0x7c00-0x7d00 (bootloader loaded here)
   |                  |  <==== 0x7c00 (top of stack; grows downwards)
   |                  |
   +------------------+  <- 0x00000000 */


/* SEG_ASM defines the segment descriptor 0xC0 means the limit is in 
	 4096-byte units and (for executable segments) 32-bit mode;
	 - base (32-bit): linear address for the first byte of the segment
	 - limit (20-bit): offset of the last memory cell in the segment
	 - type: segment type and access right

             G   D/B   O   Avl
	 - 0xC0:   1    1    0    0
     . G (granularity): 0 if byte, 1 if multiples of 4096 bytes (4K)
     . D/B: 1 f 32-bit offsets, 0 if 16-bit offsets 
*/
#define SEG_ASM(type, base, lim)																\
  .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);            \
  .byte (((base) >> 16) & 0xff), (0x90 | (type)),               \
    (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

/* 4-bit type field in code segment descriptor */
#define STA_X     0x8       /* executable segment */
#define STA_E     0x4       /* expand down (non-executable segments) */
#define STA_C     0x4       /* conforming code segment (executable only) */
#define STA_W     0x2       /* writeable (non-executable segments) */
#define STA_R     0x2       /* readable (executable segments) */
#define STA_A     0x1       /* accessed */

