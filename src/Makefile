TOPDIR ?= $$(pwd)
include $(TOPDIR)/Makefile.inc

SUBDIRS=system/ drivers/

OBJS=$(OBJDIR)/main.o
OUTPUT=$(BINDIR)/ghome

all: 
	$(MAKE) $(OUTPUT)

$(OUTPUT): $(OBJS)
	for i in $(SUBDIRS); do ($(MAKE) -C $$i TOPDIR=$(TOPDIR)); done
	$(LINKER) -o $(OUTPUT) $(L_FLAGS) $(LIB) $(OBJDIR)/*.o

$(OBJDIR)/%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	for i in $(SUBDIRS); do ($(MAKE) clean -C $$i TOPDIR=$(TOPDIR)); done
	rm -f $(OBJS)
