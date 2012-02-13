LIB_DIR=../../lib
INC_DIR=../../include

COMPILE=gcc
C_FLAGS=-Wall -I$(INC_DIR) -c `xml2-config --cflags`

LINKER=gcc
L_FLAGS=-L$(LIB_DIR)

AR=ar
AR_FLAGS=r

MAKE=make

LIB=`xml2-config --libs`

LIB_OBJ=XMLParser.o
LIB_OUT=../../lib/libutils.a

all:
	$(MAKE) lib

lib:
	$(MAKE) $(LIB_OUT)

$(LIB_OUT): $(LIB_OBJ)
	$(AR) $(AR_FLAGS) $@ $^

%.o: %.c
	$(COMPILE) $(C_FLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f $(BIN_OUT)
