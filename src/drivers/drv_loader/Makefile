COMPILE=gcc
C_FLAGS=-c -I../../../include
LINKER=gcc
L_FLAGS=
LIB=-lpthread -ldl

SUBDIRS=mon_driver/
MAKE=make

OBJECT=ios_api.o ios_data_mgr.o drv_verbose.o drv_manager.o drv_loader.o main.o
OUTPUT=drv_loader

all: 
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) all); done
	$(MAKE) $(OUTPUT)

$(OUTPUT): $(OBJECT)
	$(LINKER) -o $@ $^ $(LIB) $(L_FLAGS)

%.o: %.c
	$(COMPILE) $(C_FLAGS) $< -o $@

clean:
	for i in $(SUBDIRS); do (cd $$i ; $(MAKE) clean); done
	rm *.o
