/**
 * @file mmu.h
 * @desc definitions for the x86 memory management unit (MMU);
 *       includes definitions, registers, traps for paging and segmentation
 */
#ifndef MMU_H
#define MMU_H

/*
 * #1: definitions for registers
 */

/* 32-bit EFLAGS register: for status flags, a control flag and system flags */
#define EFLAGS_CF        0x00000001   /* carry flag */
#define EFLAGS_PF        0x00000004   /* parity flag */
#define EFLAGS_AF        0x00000010   /* auxiliary carry flag */
#define EFLAGS_ZF        0x00000040   /* zero flag */
#define EFLAGS_SF        0x00000080   /* sign flag */
#define EFLAGS_TF        0x00000100   /* trap flag */
#define EFLAGS_IF        0x00000200   /* interrupt enable */
#define EFLAGS_DF        0x00000400   /* direction flag */
#define EFLAGS_OF        0x00000800   /* overflow flag */
#define EFLAGS_IOPL_MASK 0x00003000   /* I/O privilege level bitmask */
#define EFLAGS_IOPL_0    0x00000000   /*   IOPL == 0 */
#define EFLAGS_IOPL_1    0x00001000   /*   IOPL == 1 */
#define EFLAGS_IOPL_2    0x00002000   /*   IOPL == 2 */
#define EFLAGS_IOPL_3    0x00003000   /*   IOPL == 3 */
#define EFLAGS_NT        0x00004000   /* nested task */
#define EFLAGS_RF        0x00010000   /* resume flag */
#define EFLAGS_VM        0x00020000   /* virtual 8086 mode */
#define EFLAGS_AC        0x00040000   /* alignment check */
#define EFLAGS_VIF       0x00080000   /* virtual interrupt flag */
#define EFLAGS_VIP       0x00100000   /* virtual interrupt pending */
#define EFLAGS_ID        0x00200000   /* ID flag */

/* CR0 control register flags: for controlling processor operating mode */
#define CR0_PE           0x00000001   /* protection enable */
#define CR0_MP           0x00000002   /* monitor coprocessor */
#define CR0_EM           0x00000004   /* emulation */
#define CR0_TS           0x00000008   /* task switched */
#define CR0_ET           0x00000010   /* extension type */
#define CR0_NE           0x00000020   /* numeric errror */
#define CR0_WP           0x00010000   /* write protect */
#define CR0_AM           0x00040000   /* alignment mask */
#define CR0_NW           0x20000000   /* not writethrough */
#define CR0_CD           0x40000000   /* cache disable */
#define CR0_PG           0x80000000   /* paging */

/* 
 * #2: definitions for paging
 *
 * linear address, la, has a three-part structure as shown below:
 *
 * +--------10------+-------10-------+---------12----------+
 * | page directory |   page table   | offset within page  |
 * |      index     |      index     |                     |
 * +----------------+----------------+---------------------+
 *  \--- PDX(la) --/ \--- PTX(la) --/ \---- PGOFF(la) ----/
 *   \----------- PPN(la) ---------/
 *
 * PDX, PTX, PGOFF, and PPN macros decompose linear addresses as shown;
 * to construct a linear address, la, from PDX(la), PTX(la), and
 * PGOFF(la), use PGADDR(PDX(la), PTX(la), PGOFF(la))
 */

/* page directory and page table constants */
#define NPDENTRIES   1024    /* page dir entries per page directory */
#define NPTENTRIES   1024    /* page table entries per page table */

#define PGSIZE       4096    /* bytes mapped by a page */
#define PGSHIFT      12      /* log2(PGSIZE) */

#define PTSIZE       (PGSIZE * NPTENTRIES) 
                             /* bytes mapped by a page dir entry */
#define PTSHIFT      22      /* log2(PTSIZE) */
 
#define PTXSHIFT     12      /* offset of PTX in a linear address */
#define PDXSHIFT     22      /* offset of PDX in a linear address */

/* page number field of address */
#define PPN(la)   (((uintptr_t) (la)) >> PTXSHIFT)
#define VPN(la)   PPN(la) 

/* page directory index */
#define PDX(la)   ((((uintptr_t) (la)) >> PDXSHIFT) & 0x3FF)
#define VPD(la)   PDX(la)

/* page table index */
#define PTX(la)   ((((uintptr_t) (la)) >> PTXSHIFT) & 0x3FF)

/* offset in page */
#define PGOFF(la) (((uintptr_t) (la)) & 0xFFF)

/* construct linear address from indexes and offset */
#define PGADDR(d, t, o) ((void *) ((d) << PDXSHIFT | (t) << PTXSHIFT | (o)))

#define PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) ((char *) ((((uint) (a)) & ~(PGSIZE - 1))))

/* page table/directory entry flags */
#define PTE_P        0x001   /* present */
#define PTE_W        0x002   /* writable */
#define PTE_U        0x004   /* user accessible */
#define PTE_PWT      0x008   /* write-through */
#define PTE_PCD      0x010   /* cache-disable */
#define PTE_A        0x020   /* accessed */
#define PTE_D        0x040   /* dirty */
#define PTE_PS       0x080   /* page size */
#define PTE_MBZ      0x180   /* bits must be zero */

/* address in page table or page directory entry */
#define PTE_ADDR(pte) ((uint)(pte) & ~0xFFF)

/*
 * #3: definitions for segmentation
 */

#ifdef __ASSEMBLER__

/* macros to build GDT entries in assembly */
#define SEG_NULL        \
  .word 0, 0;           \
  .byte 0, 0, 0, 0
#define SEG(type,base,lim)                                  \
  .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);        \
  .byte (((base) >> 16) & 0xff), (0x90 | (type)),           \
    (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#else /* !__ASSEMBLER__ */

#include <types.h>

/* segment descriptor */
struct segdesc {
  uint lim_15_0   :16;            /* low bits of segment limit */
  uint base_15_0  :16;            /* low bits of segment base address */
  uint base_23_16 :8;             /* middle bits of segment base address */
  uint type       :4;             /* segment type (see STS_ constants) */
  uint s          :1;             /* 0: system, 1: application */
  uint dpl        :2;             /* descriptor privilege level */
  uint p          :1;             /* present */
  uint lim_19_16  :4;             /* high bits of segment limit */
  uint avl        :1;             /* unused (available for software use) */
  uint rsv1       :1;             /* reserved */
  uint db         :1;             /* 0: 16-bit segment, 1: 32-bit segment */
  uint g          :1;             /* granularity: scale limit by 4K */
  uint base_31_24 :8;             /* high bits of segment base address */
} segdest_t, *segdest_p;

/* null segment */
#define SEG_NULL  (struct segdesc) { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

/* segment that is loadable but faults when used */
#define SEG_FAULT (struct segdesc) { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 }

/* normal segment */
#define SEG(type, base, lim, dpl) (struct segdesc)    \
  { ((lim) >> 12) & 0xffff, (uint)(base) & 0xffff,    \
      ((uint)(base) >> 16) & 0xff, type, 1, dpl, 1,   \
      (uint)(lim) >> 28, 0, 0, 1, 1, (uint)(base) >> 24 }
#define SEG16(type, base, lim, dpl) (struct segdesc)  \
  { (lim) & 0xffff, (uint)(base) & 0xffff,            \
      ((uint)(base) >> 16) & 0xff, type, 1, dpl, 1,   \
      (uint)(lim) >> 16, 0, 0, 1, 0, (uint)(base) >> 24 }

#endif /* !__ASSEMBLER */

#define DPL_USER    0x3            /* user descriptor privilege level */

/* user segment type bits; for type field of segdest_t */
#define STA_X       0x8            /* executable segment */
#define STA_E       0x4            /* expand down (non-exec segments) */
#define STA_C       0x4            /* conforming code segment (exec only) */
#define STA_W       0x2            /* writable (non-exec segments) */
#define STA_R       0x2            /* readable (exec segments) */
#define STA_A       0x1            /* accessed */

/* system segment type bits; for type field of segdest_t */
#define STS_T16A    0x1            /* available 16-bit TSS */
#define STS_LDT     0x2            /* local descriptor Table */
#define STS_T16B    0x3            /* busy 16-bit TSS */
#define STS_CG16    0x4            /* 16-bit call gate */
#define STS_TG      0x5            /* task gate / coum transmitions */
#define STS_IG16    0x6            /* 16-bit interrupt gate */
#define STS_TG16    0x7            /* 16-bit trap gate */
#define STS_T32A    0x9            /* available 32-bit TSS */
#define STS_T32B    0xB            /* busy 32-bit TSS */
#define STS_CG32    0xC            /* 32-bit call gate */
#define STS_IG32    0xE            /* 32-bit interrupt gate */
#define STS_TG32    0xF            /* 32-bit trap gate */


/*
 * #4: definitions for traps
 */
#ifndef __ASSEMBLER__

/* task state segment format */
struct taskstate {
  uint32_t ts_link;                /* old ts selector */
  uintptr_t ts_esp0;               /* stack pointers and segment selectors */
  uint16_t ts_ss0;                 /* after increase in privilege level */
  uint16_t ts_padding1;
  uintptr_t ts_esp1;
  uint16_t ts_ss1;
  uint16_t ts_padding2;
  uintptr_t ts_esp2;
  uint16_t ts_ss2;
  uint16_t ts_padding3;
  physaddr_t ts_cr3;               /* page directory base */
  uintptr_t ts_eip;                /* saved state from last task switch */
  uint32_t ts_eflags;
  uint32_t ts_eax;                 /* more saved state (registers) */
  uint32_t ts_ecx;
  uint32_t ts_edx;
  uint32_t ts_ebx;
  uintptr_t ts_esp;
  uintptr_t ts_ebp;
  uint32_t ts_esi;
  uint32_t ts_edi;
  uint16_t ts_es;                  /* more saved state (segment selectors) */
  uint16_t ts_padding4;
  uint16_t ts_cs;
  uint16_t ts_padding5;
  uint16_t ts_ss;
  uint16_t ts_padding6;
  uint16_t ts_ds;
  uint16_t ts_padding7;
  uint16_t ts_fs;
  uint16_t ts_padding8;
  uint16_t ts_gs;
  uint16_t ts_padding9;
  uint16_t ts_ldt;
  uint16_t ts_padding10;
  uint16_t ts_t;                   /* trap on task switch */
  uint16_t ts_iomb;                /* I/O map base address */
};

/* gate descriptors for interrupts and traps */
struct gatedesc {
  unsigned gd_off_15_0  : 16;      /* low 16 bits of offset in segment */
  unsigned gd_ss        : 16;      /* segment selector */
  unsigned gd_args      : 5;       /* # args, 0 for interrupt/trap gates */
  unsigned gd_rsv1      : 3;       /* reserved(should be zero I guess) */
  unsigned gd_type      : 4;       /* type(STS_{TG,IG32,TG32}) */
  unsigned gd_s         : 1;       /* must be 0 (system) */
  unsigned gd_dpl       : 2;       /* descriptor(meaning new) privilege level */
  unsigned gd_p         : 1;       /* present */
  unsigned gd_off_31_16 : 16;      /* high bits of offset in segment */
};

/* set up a normal interrupt/trap gate descriptor
   - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate;
             interrupt gate clears FL_IF, trap gate leaves FL_IF alone;
             see section 9.6.1.3 of the i386 reference: "The difference 
             between an interrupt gate and a trap gate is in the effect on 
             IF (the interrupt-enable flag). An interrupt that vectors 
             through an interrupt gate resets IF, thereby preventing other 
             interrupts from interfering with the current interrupt handler. 
             A subsequent IRET instruction restores IF to the value in the 
             EFLAGS image on the stack. An interrupt through a trap gate 
             does not change IF."
   - sel: Code segment selector for interrupt/trap handler
   - off: Offset in code segment for interrupt/trap handler
   - dpl: descriptor privilege level; the privilege level required for 
          software to invoke this interrupt/trap gate explicitly using an 
          int instruction. */
#define SETGATE(gate, istrap, sel, off, dpl)          \
  {                                                   \
    (gate).gd_off_15_0 = (uint32_t) (off) & 0xffff;   \
    (gate).gd_ss = (sel);                             \
    (gate).gd_args = 0;                               \
    (gate).gd_rsv1 = 0;                               \
    (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;  \
    (gate).gd_s = 0;                                  \
    (gate).gd_dpl = (dpl);                            \
    (gate).gd_p = 1;                                  \
    (gate).gd_off_31_16 = (uint32_t) (off) >> 16;     \
  }

/* set up a call gate descriptor */
#define SETCALLGATE(gate, ss, off, dpl)                     \
  {                                                         \
    (gate).gd_off_15_0 = (uint32_t) (off) & 0xffff;         \
    (gate).gd_ss = (ss);                                    \
    (gate).gd_args = 0;                                     \
    (gate).gd_rsv1 = 0;                                     \
    (gate).gd_type = STS_CG32;                              \
    (gate).gd_s = 0;                                        \
    (gate).gd_dpl = (dpl);                                  \
    (gate).gd_p = 1;                                        \
    (gate).gd_off_31_16 = (uint32_t) (off) >> 16;           \
  }

/* pseudo-descriptors used for LGDT, LLDT and LIDT instructions */
struct pseudodesc {
  uint16_t pd_lim;                 /* limit */
  uint32_t pd_base;                /* base address */
} __attribute__ ((packed));

#endif /* !__ASSEMBLER__ */
#endif /* MMU_H */
