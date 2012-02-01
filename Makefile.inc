LIBDIR=$(TOPDIR)/lib
BINDIR=$(TOPDIR)/bin
OBJDIR=$(TOPDIR)/obj
INCDIR=$(TOPDIR)/include

COMPILE=gcc
C_FLAGS=-c -I$(INCDIR) -I. -Wall
LINKER=gcc
L_FLAGS=-L$(LIBDIR)
LIB= -lpthread -ldl
MAKE=make
