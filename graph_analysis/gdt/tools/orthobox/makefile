include ../../custom.$(HOST)

EXECNAME = obtest

AUXOBJ = $(OBJDIR)/orthobox.o 



SRC = $(EXECNAME).cpp


include ../common.make

export:
	-mkdir win_version
	unix2dos <orthobox.cc >win_version/orhtobox.cpp	
	unix2dos <orthobox.h >win_version/orthobox.h


obtest.o : obtest.cpp
	g++ -c obtest.cpp -o obtest.o

orthobox.o : orthobox.cpp
	g++ $(JAVAINCLUDE) $(CINCLUDE) -c orthobox.cpp -o orthobox.o

traceNative2impl.o : traceNative2impl.cc
	g++ $(JAVAINCLUDE) $(CINCLUDE) -c traceNative2impl.cc -o traceNative2impl.o 
	


 
