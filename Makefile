TOPDIR ?= $$(pwd)
include ./Makefile.inc

SUBDIRS=src/

OBJS=main.o
OUTPUT=$(BINDIR)/ghome

all: 
	$$([ -d "$(LIBDIR)" ] || mkdir -p $(LIBDIR))
	$$([ -d "$(BINDIR)" ] || mkdir -p $(BINDIR))
	$$([ -d "$(OBJDIR)" ] || mkdir -p $(OBJDIR))
	$$([ -d "$(INCDIR)" ] || mkdir -p $(INCDIR))

	for i in $(SUBDIRS); do ($(MAKE) -C $$i TOPDIR=$(TOPDIR)); done

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	for i in $(SUBDIRS); do ($(MAKE) clean -C $$i TOPDIR=$(TOPDIR)); done
	rm -f $(OBJDIR)/*.o
