/**
 * @file pmap.h
 * @desc definitions for physical memory allocator which support allocation
 *       and deallocation of physical memory pages; for this, we need to 
 *       keep track of free and used memory pages
 */
#ifndef KERN_PMAP_H
#define KERN_PMAP_H

#include <vmmap.h>
#include <assert.h>

/* TRUP macro takes a user supplied address and turns it into something 
	 that will cause a fault if it is a kernel address; ULIM itself is 
	 guaranteed never to contain a valid page */
#define TRUP(_p)																					\
	({																											\
		register typeof((_p)) __m_p = (_p);										\
		(uintptr_t) __m_p > ULIM ? (typeof(_p)) ULIM : __m_p;	\
	})

/* PADDR macro takes a kernel virtual address -- an address that points above
	 KERNBASE, where the machine's top 256MB of physical memory is mapped --
	 and returns the corresponding physical address; it panics if you pass it a
	 non-kernel virtual address */
#define PADDR(kva)																						\
	({																													\
		physaddr_t __m_kva = (physaddr_t) (kva);									\
		if (__m_kva < KERNBASE)																		\
			panic("PADDR called with invalid kva %08lx", __m_kva);	\
		__m_kva - KERNBASE;																				\
	})

/* KADDR macro takes a physical address and returns the corresponding kernel
	 virtual address; it panics if you pass an invalid physical address */
#define KADDR(pa)																						\
	({																												\
		physaddr_t __m_pa = (pa);																\
		uint32_t __m_ppn = PPN(__m_pa);													\
		if (__m_ppn >= npage)																		\
			panic("KADDR called with invalid pa %08lx", __m_pa);	\
		(void *) __m_pa + KERNBASE;                             \
	})


extern char bootstacktop[], bootstack[];

extern struct page *pages;
extern size_t npage;

extern physaddr_t boot_cr3;
extern pde_t *boot_pgdir;

extern struct segdesc gdt[];
extern struct pseudodesc gdt_pd;

/* setup memory management */
void i386_detect_memory();
void i386_vm_init();

/* page management functions */
void page_init(void);
void page_check(void);
int  page_alloc(struct page **pp_store);
void page_free(struct page *pp);
int  page_insert(pde_t *pgdir, struct page *pp, void *va, int perm);
void page_remove(pde_t *pgdir, void *va);
struct page *page_lookup(pde_t *pgdir, void *va, pte_t **ppte);
void page_decref(struct page *pp);
void tlb_invalidate(pde_t *, void *va);

/* returns page frame number for the given page */
static inline ppn_t page2ppn(struct page *pp)
{
	return pp - pages;
}

/* returns physical address for the given page frame number */
static inline physaddr_t page2pa(struct page *pp)
{
	return page2ppn(pp) << PGSHIFT;
}

/* inverse of page2pa */
static inline struct page *pa2page(physaddr_t pa)
{
	if (PPN(pa) >= npage)
		panic("pa2page called with invalid pa");
	return &pages[PPN(pa)];
}

/* returns the kernel virtual address of the given page */
static inline void *page2kva(struct page *pp)
{
	return KADDR(page2pa(pp));
}

static pte_t *pgdir_walk(pde_t* pgdir, const void *va, int create);

#endif /* KERN_PMAP_H */
