include $(TOPDIR)/Makefile.inc
COMPILE=gcc
C_FLAGS += -fPIC
LINKER=gcc
L_FLAGS=-shared
LIB=-lpthread

OBJECT=driver.o
OUTPUT=$(LIBDIR)/mon_driver.so.1.0.1

all: $(OUTPUT)

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	rm -f *.o
