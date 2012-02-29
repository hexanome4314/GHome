MAKE=make

SUBDIRS=src/

all: 
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) all); done	

clean:
	for i in $(SUBDIRS); do (cd $$i; $(MAKE) clean); done
	rm -f lib/*.a
	rm -f lib/*.so
