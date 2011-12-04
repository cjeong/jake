/**
 * @file monitor.c
 * @desc simple command-line kernel monitor useful for
 *       controlling the kernel and exploring the system interactively
 */
#include <stdio.h>
#include <string.h>
#include <vmmap.h>
#include <assert.h>
#include <x86.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>

#define CMDBUF_SIZE	80	        /* enough for one VGA text line */

struct command {
	const char *name;
	const char *desc;

	/* return -1 to force monitor to exit */
	int (*func)(int argc, char** argv, struct trapframe* tf);
};

static struct command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
};

#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

unsigned read_eip();

/* implementations of basic kernel monitor commands */
int mon_help(int argc, char **argv, struct trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int mon_kerninfo(int argc, char **argv, struct trapframe *tf)
{
	extern char _start[], etext[], edata[], end[];

	cprintf("special kernel symbols:\n");
	cprintf("  _start %08x (virt)  %08x (phys)\n", _start, _start - KERNBASE);
	cprintf("   etext %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("   edata %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("     end %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("kernel executable memory footprint: %dKB\n",
          (end-_start+1023)/1024);
	return 0;
}

int mon_backtrace(int argc, char **argv, struct trapframe *tf)
{
  cprintf("Stack backtrace:\n");
  uintptr_t *ebp_addr = (uintptr_t *) read_ebp();
  uintptr_t *eip_addr, *arg1_addr, *arg2_addr, *arg3_addr;
  while (*ebp_addr != 0x0) {
    eip_addr = ebp_addr + 1;
    arg1_addr = ebp_addr + 2;
    arg2_addr = ebp_addr + 3;
    arg3_addr = ebp_addr + 4;

    cprintf("ebp %08x  eip %08x  args %08x %08x %08x\n", *ebp_addr, *eip_addr, 
            *arg1_addr, *arg2_addr, *arg3_addr);
    ebp_addr = (uintptr_t *) *ebp_addr;
  }

	return 0;
}

/* kernel monitor command interpreter */
#define WHITESPACE "\t\r\n "
#define MAXARGS 16
static int runcmd(char *buf, struct trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	/* parse the command buffer into whitespace-separated arguments */
	argc = 0;
	argv[argc] = 0;
	while (1) {
		/* gobble whitespace */
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		/* save and scan past next arg */
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	/* lookup and invoke the command */
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void monitor(struct trapframe *tf)
{
	char *buf;

	cprintf("*** Jake Kernel Monitor!\n");
	cprintf("*** Type 'help' for a list of commands.\n");

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}

/* return EIP of caller; does not work if inlined; putting at the end of the 
	 file seems to prevent inlining */
unsigned read_eip()
{
	uint32_t callerpc;
	__asm __volatile("movl 4(%%ebp), %0" : "=r" (callerpc));
	return callerpc;
}

