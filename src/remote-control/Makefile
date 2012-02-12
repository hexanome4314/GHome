COMPILE=gcc
C_FLAGS=-c -I../../../include
LINKER=gcc
L_FLAGS=-L../../../lib
LIB=-lpthread
AR=ar
AR_FLAGS=r

MAKE=make

OBJECT=command_interp.o connection.o console.o main.o
OUTPUT=remote-control

AR_OBJ=console.o
AR_OUT=../../../lib/libremote-ctrl.a

all: 
	# $(MAKE) $(AR_OUT)
	$(MAKE) $(OUTPUT)

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(L_FLAGS) $(LIB)

$(AR_OUT): $(AR_OBJ)
	$(AR) $(AR_FLAGS) $@ $^

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(OUTPUT)
