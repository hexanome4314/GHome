MAKE=make

SUBDIRS=drv_loader/ mon_driver/ enocean_driver/

all: 
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) all); done

clean:
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) clean); done

