INC_DIR=../../../include
LIB_DIR=

MAKE=make

COMPILE=gcc
C_FLAGS=-c -I$(INC_DIR) -fPIC -Wall -m32 #Options de compilations

LINKER=gcc
L_FLAGS=-shared -Wall -m32

LIB=-lpthread

OBJ= drv_api.o utils.o listen.o 
OUTPUT=../../../drivers/libenocean.so #EXE= ../enocean_driver #Nom du binaire à construire

all:
	$(MAKE) $(OUTPUT)

$(OUTPUT): $(OBJ)
	@echo building $<
	$(LINKER)  -o $(OUTPUT) $(L_FLAGS) $(OBJ) $(LIB)
	@echo done

%.o : %.c 
	@echo building $< ...
	$(COMPILE) $(C_FLAGS) $< -o $@
	@echo done

clean: 
	@echo -n cleaning repository... 
	@rm -f *.o
	@rm -f .*.swp
	@rm -f *~
	@rm -f *.log
	@rm -f *.pid
	@rm -f *.out
	@rm -f *.so
	@echo cleaned.

coffee : clean
	@echo No way!
