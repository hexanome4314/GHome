COMPILE=gcc
C_FLAGS=-c -I../../../include
LINKER=gcc
L_FLAGS=-L../../../lib
LIB=-lios -lpthread -ldl
AR=ar
AR_FLAGS=r

SUBDIRS=mon_driver/
MAKE=make

OBJECT=main.o
OUTPUT=drv_loader

AR_OBJ=ios_api.o ios_data_mgr.o drv_verbose.o drv_manager.o drv_loader.o
AR_OUT=../../../lib/libios.a

all: 
	$(MAKE) $(AR_OUT)
	$(MAKE) $(OUTPUT)

$(OUTPUT): $(OBJECT)
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) all); done
	$(LINKER) -o $@ $^ $(L_FLAGS) $(LIB)

$(AR_OUT): $(AR_OBJ)
	$(AR) $(AR_FLAGS) $@ $^

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) clean); done
	rm *.o
