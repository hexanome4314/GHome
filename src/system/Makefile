COMPILE=gcc
C_FLAGS=-c -DMEMORY_DEBUG -fPIC

LINKER=gcc
L_FLAGS=-Wall

L_SO_FLAGS=-shared -fPIC -DOVERRIDE_STDLIB

MAKE=make

BIN_OBJ=memory.o memory_test.o
BIN_OUT=memory_test

SO_OBJ=memory.o
SO_OUT=memory.so

all:
	$(MAKE)	$(BIN_OUT)
	$(MAKE) $(SO_OUT)

bin:
	$(MAKE) $(BIN_OUT)

so:
	$(MAKE) $(SO_OUT)

$(BIN_OUT): $(BIN_OBJ)
	$(LINKER) -o $@ $^ $(L_FLAGS)

$(SO_OUT): $(SO_OBJ)
	$(LINKER) -o $@ $^ $(L_FLAGS) $(L_SO_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

$(TEST_OUTPUT): $(TEST_OBJECTS)
	$(LINKER) $(L_TEST_FLAGS) $(TEST_OBJECTS) -o $@

clean:
	rm -f *.o
	rm -f *.swp
	rm -f $(SO_OUT) $(BIN_OUT)
