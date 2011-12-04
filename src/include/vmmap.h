/**
 * @file vmmap.h
 * @desc virtual memory map and includes definitions for implementing 
 *       virtual memory in kernel-mode and user-mode 
 */
#ifndef VMMAP_H
#define VMMAP_H

#ifndef __ASSEMBLER__
#include <types.h>
#include <queue.h>
#include <mmu.h>
#endif /* __ASSEMBLER__ */

/* global descriptor numbers */
#define GD_KT     0x08     /* kernel text */
#define GD_KD     0x10     /* kernel data */
#define GD_UT     0x18     /* user text */
#define GD_UD     0x20     /* user data */
#define GD_TSS    0x28     /* task segment selector */

/* virtual memory map:                                 permissions
                                                      (kernel/user)
 
       4GB -------->  +------------------------------+
                      |                              | RW/--
                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                      :              .               :
                      :              .               :
                      :              .               :
                      |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| RW/--
                      |                              | RW/--
                      |   Remapped Physical Memory   | RW/--
                      |                              | RW/--
     KERNBASE ----->  +------------------------------+ 0xf0000000
                      |  Cur. Page Table (Kern. RW)  | RW/--  PTSIZE
     VPT,KSTACKTOP--> +------------------------------+ 0xefc00000      --+
                      |         Kernel Stack         | RW/--  KSTKSIZE   |
                      | - - - - - - - - - - - - - - -|                 PTSIZE
                      |      Invalid Memory (*)      | --/--             |
     ULIM     ------> +------------------------------+ 0xef800000      --+
                      |  Cur. Page Table (User R-)   | R-/R-  PTSIZE
     UVPT      ---->  +------------------------------+ 0xef400000
                      |          RO PAGES            | R-/R-  PTSIZE
     UPAGES    ---->  +------------------------------+ 0xef000000
                      |           RO ENVS            | R-/R-  PTSIZE
  UTOP,UENVS ------>  +------------------------------+ 0xeec00000
  UXSTACKTOP -/       |     User Exception Stack     | RW/RW  PGSIZE
                      +------------------------------+ 0xeebff000
                      |       Empty Memory (*)       | --/--  PGSIZE
     USTACKTOP  --->  +------------------------------+ 0xeebfe000
                      |      Normal User Stack       | RW/RW  PGSIZE
                      +------------------------------+ 0xeebfd000
                      |                              |
                      |                              |
                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                      .                              .
                      .                              .
                      .                              .
                      |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
                      |     Program Data & Heap      |
     UTEXT -------->  +------------------------------+ 0x00800000
     PFTEMP ------->  |       Empty Memory (*)       |        PTSIZE
                      |                              |
     UTEMP -------->  +------------------------------+ 0x00400000      --+
                      |       Empty Memory (*)       |                   |
                      | - - - - - - - - - - - - - - -|                   |
                      |  User STAB Data (optional)   |                 PTSIZE
     USTABDATA ---->  +------------------------------+ 0x00200000        |
                      |       Empty Memory (*)       |                   |
     0 ------------>  +------------------------------+                 --+
 
  NOTE: kernel ensures that "Invalid Memory" (ULIM) is *never* mapped;  
        "Empty Memory" is normally unmapped, but user programs may map pages 
        there if desired. user programs map pages temporarily at UTEMP */


/* NOTE: macro defs below are in decreasing order of addresses from 4GB */

/* all physical memory mapped at this address */
#define KERNBASE   0xF0000000

/* virtual page table;  entry PDX[VPT] in the PD contains a pointer to
   the page directory itself, essentially turning the PD into a page table,
   which maps all the PTEs containing the page mappings for the entire
   virtual address space into that 4MB region starting at VPT */
#define VPT        (KERNBASE - PTSIZE)  /* kernel page table */
#define KSTACKTOP  VPT                  /* kernel stack top */
#define KSTKSIZE   (8 * PGSIZE)         /* size of a kernel stack */
#define ULIM       (KSTACKTOP - PTSIZE) /* boundary between kernal and user */

/* user read-only mappings; anything below here til UTOP are readonly to 
   user; they are global pages mapped in at env allocation time */
#define UVPT       (ULIM - PTSIZE)      /* same as VPT but RO for users */
#define UPAGES     (UVPT - PTSIZE)      /* RO copies of page structures */
#define UENVS      (UPAGES - PTSIZE)    /* RO copies of global env structs */

/* top of user VM; user can manipulate VA from UTOP-1 and down */

#define UTOP       UENVS                /* top of user-accessible VM */
#define UXSTACKTOP UTOP                 /* top of one-page user exn stack */

/* next page left invalid to guard against exception stack overflow; 
   then: Top of normal user stack */
#define USTACKTOP  (UTOP - 2 * PGSIZE)
#define UTEXT      (2 * PTSIZE)         /* where user programs begin */
#define UTEMP      ((void *) PTSIZE)    /* used for temporary page mappings; 
                                           typed (void *) for convenience */

/* used for temporary page mappings for the user page-fault handler
   (should not conflict with other temporary page mappings) */
#define PFTEMP     (UTEMP + PTSIZE - PGSIZE)

/* the location of the user-level STABS data structure */
#define USTABDATA  (PTSIZE / 2) 

/* at IOPHYSMEM (640K) there is a 384K hole for I/O; from the kernel,
   IOPHYSMEM can be addressed at KERNBASE + IOPHYSMEM;  the hole ends
   at physical address EXTPHYSMEM */
#define EXTPHYSMEM 0x100000
#define IOPHYSMEM  0x0A0000

#ifndef __ASSEMBLER__

/* page numbers are 32 bits long */
typedef uint32_t ppn_t;

/* off_t is used for file offsets and lengths */
typedef int32_t off_t;

/* the page directory entry corresponding to the virtual address range
   [VPT, VPT + PTSIZE) points to the page directory itself; thus, the page
   directory is treated as a page table as well as a page directory
   
   one result of treating the page directory as a page table is that all 
   PTEs can be accessed through a "virtual page table" at virtual address 
   VPT (to which vpt is set in entry.S); the PTE for page number N is 
   stored in vpt[N] (it's worth drawing a diagram of this)
   
   a second consequence is that the contents of the current page directory
   will always be available at virtual address (VPT + (VPT >> PGSHIFT)), to
   which vpd is set in entry.S */
typedef uint32_t pte_t;
typedef uint32_t pde_t;

extern volatile pte_t vpt[];     /* va of virtual page table */
extern volatile pde_t vpd[];     /* va of current page directory */

/* page descriptor structures, mapped at UPAGES; read/write to the kernel, 
   read-only to user programs; each page describes one physical page;
   you can map a (page *) to the corresponding physical address with 
   page2pa() in kern/pmap.h */
LIST_HEAD(pagelist, page);
typedef LIST_ENTRY(page) pagelist_entry_t;
                         
struct page {
  pagelist_entry_t pp_link; /* free list link */

  /* pp_ref is the count of pointers (usually in page table entries)
     to this page, for pages allocated using page_alloc; pages allocated 
     at boot time using pmap.c's boot_alloc do not have valid reference 
     count fields */
  uint16_t pp_ref;
};

#endif /* __ASSEMBLER__ */
#endif /* VMMAP_H */
