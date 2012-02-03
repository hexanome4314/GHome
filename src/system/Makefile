include $(TOPDIR)/Makefile.inc
C_TEST_FLAGS:=-c -DMEMORY_DEBUG
L_TEST_FLAGS=
TEST_OBJECTS=memory.o memory_test.o
OBJECTS=$(OBJDIR)/memory.o
TEST_OUTPUT=memory_test

all: $(OBJECTS)

$(OBJDIR)/%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

%.o: %.c
	$(COMPILE) $(C_TEST_FLAGS) $< -o $@

$(TEST_OUTPUT): $(TEST_OBJECTS)
	$(LINKER) $(L_TEST_FLAGS) $(TEST_OBJECTS) -o $@

# Pour creer une lib dynamique et tester des programmes externes
memory.so: memory.c
	$(COMPILE) -shared -fPIC -DOVERRIDE_STDLIB -Wall $^ -o $@

clean:
	rm -f $(TEST_OBJECTS) $(TEST_OUTPUT) $(OBJECTS)
