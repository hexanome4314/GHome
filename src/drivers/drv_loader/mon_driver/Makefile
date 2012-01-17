COMPILE=gcc
C_FLAGS=-c -fPIC
LINKER=gcc
L_FLAGS=-shared

OBJECT=driver.o
OUTPUT=libdriver.so.1.0.1

all: $(OUTPUT)

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(L_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	rm *.o
