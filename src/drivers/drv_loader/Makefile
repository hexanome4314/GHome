COMPILE=gcc
C_FLAGS=-c 
LINKER=gcc
L_FLAGS=
LIB=-lpthread -ldl

OBJECT=drv_loader.o main.o
OUTPUT=drv_loader

all: $(OUTPUT)

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	rm *.o
