TOPDIR ?= $$(pwd)
include $(TOPDIR)/Makefile.inc

SUBDIRS= mon_driver/

OBJECTS=$(addprefix $(OBJDIR)/, ios_api.o ios_data_mgr.o drv_verbose.o drv_manager.o drv_loader.o)

all: $(OBJECTS)
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) all); done

#	$(LINKER) -o $@ $^ $(L_FLAGS) $(LIB)

$(OBJDIR)/%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) clean); done
	rm -f $(OBJECTS)
