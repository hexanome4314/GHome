COMPILE=gcc
C_FLAGS=-c

LINKER=gcc
L_FLAGS=

INC_DIR=-I../../../include/
LIB_DIR=-L../../../lib/
LIB=-lios -lpthread -ldl

AR=ar
AR_FLAGS=r

MAKE=make
SUBDIRS=

BIN_OBJECTS=main.o
BIN_OUT=drv_loader

LIB_OBJECTS=ios_api.o ios_data_mgr.o drv_verbose.o drv_manager.o drv_loader.o main.o
LIB_OUT=../../../lib/libios.a

all: $(OBJECTS)
	$(MAKE) $(BIN_OUT)
	$(MAKE) $(LIB_OUT)

$(BIN_OUT): $(BIN_OBJECTS)
	$(LINKER) -o $(BIN_OUT) $^ $(L_FLAGS) $(LIB_DIR) $(LIB)

$(LIB_OUT): $(LIB_OBJECTS)
	$(AR) $(AR_FLAGS) $@ $^

lib:
	$(MAKE) $(LIB_OUT)

bin:
	$(MAKE) $(BIN_OUT)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $(INC_DIR) $< -o $@

clean:
	rm -f *.o
	rm -f $(BIN_OUT)
