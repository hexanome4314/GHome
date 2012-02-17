MAKE=make

SUBDIRS=system/ drivers/ ia/ core/ remote-control/

all: 
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) all); done	

clean:
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) clean); done
