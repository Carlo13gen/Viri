BINDIR       = ../../bin/$(HOST)
LIBDIR       = ../../lib/$(HOST)
INCDIR 	     = ../../incl

CFLAGS   = $(CUSTOM_CFLAGS)
CINCLUDE = -I$(INCDIR) -I.

EXEC = $(BINDIR)/$(EXECNAME)
OBJ = $(EXECNAME).o

# -------------
# user targets
# -------------

default:  $(EXEC)


$(EXEC): $(OBJ) 
	$(LD) $(CUSTOM_LDFLAGS)  $(OBJ) $(AUXOBJ) $(LIBDIR)/libGDT.a   $(AUXLIB)  -o $(EXEC)

clean:
	rm -f *.o $(OBJ) $(AUXOBJ) *~

delete: clean
	rm -f $(EXEC)



# --------------- 
# private targets 
# ---------------

# ---------------------------------------------------
# for compiling under Linux with the "make" command
# or under solaris with the "gmake" command
# ---------------------------------------------------

%.o: %.cpp  $(TOOLSINCLUDE) 
	$(CC) -c -I../../incl $< -o $@ $(CFLAGS) 

#$(OBJ): $(SRC)
#	$(CC) -c $(SRC) -o $(OBJ) $(CFLAGS) $(CINCLUDE) 

dependencies.make: $(SRC) $(INCDIR)/GDT/*.h
	$(CC) $(CINCLUDE) $(CFLAGS) -M $(SRC) >dependencies.make


#dependancies
include dependencies.make





