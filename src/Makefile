MAKE=make

SUBDIRS=system/ drivers/ ia/ remote-control/ remote-actionner/ core/

all: 
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) all); done	

clean:
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) clean); done
