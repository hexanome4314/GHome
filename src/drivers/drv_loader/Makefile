COMPILE=gcc
C_FLAGS=-c

LINKER=gcc
L_FLAGS=

INCLUDE=-I../../../include/
LIB=-lpthread -ldl

SUBDIRS=

OBJECTS=ios_api.o ios_data_mgr.o drv_verbose.o drv_manager.o drv_loader.o main.o
OUTPUT=drv_loader

all: $(OBJECTS)
	$(LINKER) -o $(OUTPUT) $^ $(L_FLAGS) $(LIB)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $(INCLUDE) $< -o $@

clean:
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) clean); done
	rm -f $(OBJECTS)
	rm -f *.o
