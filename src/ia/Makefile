LIB_DIR=../../lib
INC_DIR=../../include

COMPILE=gcc
C_FLAGS=-Wall -I$(INC_DIR) -c `xml2-config --cflags`

LINKER=gcc
L_FLAGS=-L$(LIB_DIR)

AR=ar
AR_FLAGS=r

MAKE=make

LIB=`xml2-config --libs` -lios -ldl -lpthread

BIN_OBJ= engine.o XMLParser.o rulesParser.o engine-test.o
BIN_OUT= engine-test

LIB_OBJ= engine.o XMLParser.o rulesParser.o
LIB_OUT=../../lib/libia.a

all:
	$(MAKE) bin
	$(MAKE) lib

bin:
	$(MAKE) $(BIN_OUT)

lib:
	$(MAKE) $(LIB_OUT)

$(BIN_OUT): $(BIN_OBJ)
	$(LINKER) $(L_FLAGS) -o $@ $^ $(LIB)

$(LIB_OUT): $(LIB_OBJ)
	$(AR) $(AR_FLAGS) $@ $^

%.o: %.c
	$(COMPILE) $(C_FLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f $(BIN_OUT)
