HOST=$(shell echo $$HOSTTYPE)  

include ../../../custom.$(HOST)

INCDIR1= ../../../incl
INCDIR2= ../include

SRC = GdtJavaWrapper.cpp


default: GdtJavaWrapper.o

GdtJavaWrapper.o : GdtJavaWrapper.cpp
	$(CC) -fpic -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(INCDIR1) -I$(INCDIR2) -c $(SRC) -o GdtJavaWrapper.o


clean:
	rm -f *.o *~


 
