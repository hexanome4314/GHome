TOPDIR ?= $$(pwd)
LIBDIR=$(TOPDIR)/lib
BINDIR=$(TOPDIR)/bin
OBJDIR=$(TOPDIR)/obj
INCDIR=$(TOPDIR)/include

COMPILE=gcc
C_FLAGS=-c -I$(INCDIR)
LINKER=gcc
L_FLAGS=-L$(LIBDIR)
LIB=-lios -lpthread -ldl
MAKE=make

SUBDIRS=drv_loader/ mon_driver/ enocean_driver/

all: 
	for i in $(SUBDIRS); do ($(MAKE) -C $$i TOPDIR=$(TOPDIR)); done

clean:
	for i in $(SUBDIRS); do ($(MAKE) clean -C $$i TOPDIR=$(TOPDIR)); done

