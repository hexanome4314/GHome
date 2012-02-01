include $(TOPDIR)/Makefile.inc
#Variables utilisées :
C_FLAGS=-I$(INCDIR) -Wall -m32 #Options de compilations
EDLFLAGS=-L$(LIBDIR) -Wall -m32
OUTPUT=$(LIBDIR)/libenocean.so #EXE= ../enocean_driver #Nom du binaire à construire

OBJ= drv_api.o utils.o listen.o 
LIBS=-lpthread

$(OUTPUT): $(OBJ)
	@echo building $<
	$(EDL)  -o $(OUTPUT) $(EDLFLAGS) $(OBJ) $(LIBS)
	@echo done

%.o : %.c 
	@echo building $< ...
	$(COMPILE) $(C_FLAGS) $<
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
