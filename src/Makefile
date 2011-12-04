# @file Makefile
# @desc makefile for Jake, A Java Kernel
#
MAKE = make

# project directories
prjdir = $(HOME)/work/jake
blddir = $(prjdir)/build
incdir = $(prjdir)/src/include

# configurations
# set "V = @" to suppress verbose build commands
V = @

# compilation/linking options
#   -pipe: use pipe for communication between compilation stages
#   -nostdinc: do not search for standard include directories; only use -I
#   -fno_builtin: needed to avoid refs to undefined functions in the kernel
#   -fno-stack-protector: don't add stack smashing protection code
CC = gcc -pipe
DEFS := -DDEBUG
CFLAGS := $(CFLAGS) $(DEFS) -O1 -fno-builtin -I$(incdir) -MD -m32
CFLAGS += -Wall -Wno-format -Wno-unused -Werror -gstabs
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
LD = ld
LDFLAGS = -m elf_i386
GCC_LIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
AS = as
AR = ar
OBJDUMP = objdump
OBJCOPY = objcopy
NM = nm
PERL = perl
TAGS = /usr/bin/etags

# subdirectories under $(blddir) where object files will be created;
# to be added by Makefile.sub's
objdirs := 

# default target
all:

.SUFFIXES:
.DELETE_ON_ERROR:
.PRECIOUS: %.o $(blddir)/boot/*.o $(blddir)/kern/%.o $(blddir)/lib/%.o

# kernel/user-specific flags;
#   -gstabs: create debugging info in stabs format (for gdb)
#   -nostdlib: do not use standard glibc libraries when linking
KERN_CFLAGS := $(CFLAGS) -DCOMPILE_KERNEL -nostdinc
USER_CFLAGS := $(CFLAGS) -DCOMPILE_USER
KERN_LDFLAGS := $(LDFLAGS) -T kern/kernel.ld -nostdlib
USER_LDFLAGS := $(LDFLAGS) -T user/user.ld


# include local makefiles from subdirectories
include ./boot/Makefile.sub
include ./kern/Makefile.sub

IMAGES = $(blddir)/kern/kernel.img
QEMU = qemu
QEMUOPTS = -hda $(blddir)/kern/kernel.img -serial mon:stdio

GDBPORT := $(shell expr `id -u` % 5000 + 25000)
QEMUGDB = $(shell if $(QEMU) -nographic -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

run: qemu

qemu: $(IMAGES)
	$(QEMU) $(QEMUOPTS)

qemu-gdb: $(IMAGES) .gdbinit
	@echo "running 'gdb'." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)

sources := $(foreach dir, $(objdirs), $(wildcard $(dir)/*))
headers := $(foreach dir, $(incdir), $(wildcard $(dir)/*.h)) \
	$(foreach dir, $(incdir)/*, $(wildcard $(dir)/*.h))
tags: $(sources) $(headers)
	@echo + tags 
	@$(TAGS) -o ./TAGS $(sources) $(headers)

clean:
	$(RM) -rf $(blddir) .gdbinit

$(blddir)/.deps: $(foreach dir, $(objdirs), $(wildcard $(blddir)/$(dir)/*.d))
	@mkdir -p $(@D)
	@$(PERL) mergedep.pl $@ $^

-include $(objdir)/.deps

always:
	@:

.PHONY: all always 
