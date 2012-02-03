ifdef TOPDIR
include $(TOPDIR)/Makefile.inc
else
BINDIR=.
INCDIR=../../../include/
endif
COMPILE=gcc
C_FLAGS = -c -I$(INCDIR) -fPIC
LINKER=gcc
L_FLAGS=-shared
LIB=-lpthread

OBJECT=driver.o
OUTPUT=$(BINDIR)/mon_driver.so.1.0.1

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

%.so: %.o
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

clean:
	rm -f $(OBJECT)
