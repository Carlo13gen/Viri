HOST=$$HOSTTYPE

CC = g++
LD = g++
RANLIB = ranlib



CUSTOM_CFLAGS= -Wall  -g  -I/usr/include 


CUSTOM_LDFLAGS= -L/usr/X11R6/lib  -L$(GDTHOME)/lib/$(HOST) 


EXECNAME = test_geometry

SRC = $(EXECNAME).cpp


TOOLSINCLUDE = *.h


LIBDIR       = ../../lib/$(HOST)
INCDIR 	     = ../../incl


CFLAGS   = $(CUSTOM_CFLAGS) 
CINCLUDE = -I$(INCDIR) -I.

EXEC = $(EXECNAME)
OBJ = $(EXECNAME).o


#--------------------------------------------------------------- user targets


default:  $(EXEC)



$(EXEC): $(OBJ)
	$(LD) $(CUSTOM_LDFLAGS)     $(OBJ) -lGDT   -o $(EXEC)

clean:
	rm -f *.o $(OBJ) $(AUXOBJ) *~

delete: clean
	rm -f $(EXEC)


#--------------------------------------------------------------- private targets

# ---------------------------------------------------
# for compiling under Linux with the "make" command
# or under solaris with the "gmake" command
# ---------------------------------------------------

.cpp.o:
	$(CC) -c  $< -o $@ $(CFLAGS) $(CUSTOM_CFLAGS) $(CINCLUDE)








