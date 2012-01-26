#Variables utilisées :
CC=gcc #Compilateur
EDL=gcc #Linker
CCFLAGS=-Wall -m32 #Options de compilations
EDLFLAGS=-Wall -m32
EXE= ../enocean_driver #Nom du binaire à construire

OBJ= drv_api.o utils.o listen.o 
LIBS=-lpthread



$(EXE): $(OBJ)
	@echo building $<
	$(EDL)  -o $(EXE) $(EDLFLAGS) $(OBJ) $(LIBS)
	@echo done

%.o : %.c 
	@echo building $< ...
	$(CC) $(CCFLAGS) -c $<
	@echo done
	
clean: 
	@echo -n cleaning repository... 
	@rm -f *.o
	@rm -f .*.swp
	@rm -f *~
	@rm -f *.log
	@rm -f *.pid
	@rm -f *.out
	@echo cleaned.

coffee : clean
	@echo No way!
