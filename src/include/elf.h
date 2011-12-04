/**
 * @file elf.h
 * @desc format of 32-bit ELF executable file
 */
#ifndef ELF_H
#define ELF_H

#define ELF_MAGIC 0x464C457FU  /* "\x7FELF" in little endian */

/*
 * ELF header 
 */
struct elfhdr {
  uint magic;        /* ELF_MAGIC */
  uchar elf[12];
  ushort type;
  ushort machine;
  uint version;
  uint entry;        /* program entry point */
  uint phoff;        /* offset of program header table */
  uint shoff;
  uint flags;
  ushort ehsize;
  ushort phentsize;
  ushort phnum;      /* number of program headers */
  ushort shentsize;
  ushort shnum;
  ushort shstrndx;
};

/*
 *  program section header 
 */

/* values for elf_proghdr type */
#define ELF_PROG_LOAD           1

/* flag bits for elf_proghdr flags */
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

struct elf_proghdr {
  uint type;
  uint offset;
  uint va;           /* virtual address */
  uint pa;
  uint filesz;       /* number of bytes of this section in disk */
  uint memsz;        /* number of bytes to load in mem (may >= filesz) */
  uint flags;
  uint align;
};

struct elf_secthdr {
	uint name;
	uint type;
	uint flags;
	uint addr;
	uint offset;
	uint size;
	uint link;
	uint info;
	uint addralign;
	uint entsize;
};

#endif /* ELF_H */
