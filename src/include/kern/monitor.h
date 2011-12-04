/**
 * @file monitor.h
 * @desc
 */
#ifndef KERN_MONITOR_H
#define KERN_MONITOR_H

#ifndef COMPILE_KERNEL
#error "This is a kernel header; user programs should not #include it"
#endif

struct trapframe;

/* activate the kernel monitor, optionally providing a trap frame indicating 
   the current state (NULL if none) */
void monitor(struct trapframe *tf);

/* functions implementing monitor commands */
int mon_help(int argc, char **argv, struct trapframe *tf);
int mon_kerninfo(int argc, char **argv, struct trapframe *tf);
int mon_backtrace(int argc, char **argv, struct trapframe *tf);

#endif	/* KERN_MONITOR_H */
