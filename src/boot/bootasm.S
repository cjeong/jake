/**
 * @file bootasm.S
 * @desc bootstrap image to be loaded to boot sector. should fit in 512B
 *       for details, see "xv6 documentation, chapter 1. bootstrap"
 */
#include "asm.h"

# start the first CPU, switch to 32-bit protected mode, jump into bootmain()
# C function. BIOS loads this code from the first sector of the hard disk
# into memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

#define SEG_KCODE 1              /* kernel code */
#define SEG_KDATA 2              /* kernel data+stack */
#define CR0_PE    1              /* protected mode enable bit */

.code16                           # assemble for 16-bit mode
.globl start
start:
  # BIOS has its own interrupt table and by default it's enabled.
  # disable interrupt, which will be enabled after control transferred to
  # kernel. kernel will set up its own interrupt table
  cli                             # disable interrupts enabled by BIOS
  cld                             # string operations increment

  # set up the important data segment registers (DS, ES, SS).
  # initialize all to zero
  xorw    %ax, %ax                # 0 -> AX
  movw    %ax, %ds                # 0 -> DS
  movw    %ax, %es                # 0 -> ES
  movw    %ax, %ss                # 0 -> SS

  # physical address line A20 is tied to zero so that the first PCs 
  # with 2 MB would run software that assumed 1 MB.  undo that.
seta20.1:
  inb     $0x64, %al              # wait for not busy
  testb   $0x2, %al
  jnz     seta20.1

  movb    $0xd1, %al              # 0xd1 -> port 0x64
  outb    %al, $0x64

seta20.2:
  inb     $0x64, %al              # wait for not busy
  testb   $0x2, %al
  jnz     seta20.2

  movb    $0xdf, %al              # 0xdf -> port 0x60
  outb    %al, $0x60

  # switch from real to protected mode. use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
  movl    %cr0, %eax              # CR0 -> EAX
  orl     $CR0_PE, %eax           # set enable protection bit of CR0
  movl    %eax, %cr0              # EAX -> CR0

  # complete transition to 32-bit protected mode by using long jmp
  # to reload %cs and %eip.  the segment registers are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32

.code32                           # assemble to 32-bit code from now on
start32:
  # set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # 0x10 -> AX
  movw    %ax, %ds                # -> DS
  movw    %ax, %es                # -> ES
  movw    %ax, %ss                # -> SS
  movw    $0, %ax                 # zero segments not ready for use
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS

  # set up the stack pointer and call into C.
  movl    $start, %esp
  call    bootmain

  # in case bootmain returns (it shouldn't), trigger a Bochs
  # breakpoint if running under Bochs, then loop.
  movw    $0x8a00, %ax                    # 0x8a00 -> port 0x8a00
  movw    %ax, %dx
  outw    %ax, %dx
  movw    $0x8ae0, %ax                    # 0x8ae0 -> port 0x8a00
  outw    %ax, %dx
spin:
  jmp     spin

# bootstrap GDT
.p2align 2                                # force 4-byte alignment
gdt:
  SEG_NULLASM                             # null segment descriptor
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code segment descriptor
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data segment descriptor

gdtdesc:
  .word   (gdtdesc - gdt - 1)             # sizeof(gdt) - 1 (0x17)
  .long   gdt                             # address gdt
