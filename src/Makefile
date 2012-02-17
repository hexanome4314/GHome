MAKE=make

SUBDIRS=system/ drivers/ utils/ core/ ia/ remote-control/

all: 
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) all); done	

clean:
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) clean); done
