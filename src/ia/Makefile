include $(TOPDIR)/Makefile.inc

SUBDIRS=

OBJECTS=$(OBJDIR)/engine.o

all: $(OBJECTS)
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) all); done

$(OBJDIR)/%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) clean); done
	rm -f $(OBJECTS)
