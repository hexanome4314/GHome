INC_DIR=../../../include/
LIB_DIR=

MAKE=make

COMPILE=gcc
C_FLAGS = -c -I$(INC_DIR) -fPIC

LINKER=gcc
L_FLAGS=-shared

LIB=-lpthread

OBJECT=driver.o
OUTPUT=mon_driver.so.1.0.1

all:
	$(MAKE) $(OUTPUT)

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

%.so: %.o
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

clean:
	rm -f *.o
	rm -f $(OUTPUT)
