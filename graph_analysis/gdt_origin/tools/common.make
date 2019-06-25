BINDIR       = ../../bin/$(HOST)
OBJDIR       = ../../obj/$(HOST)
LIBDIR       = ../../lib/$(HOST)
INCDIR 	     = ../../incl

CFLAGS   = $(CUSTOM_CFLAGS)
CINCLUDE = -I$(INCDIR) -I.

EXEC = $(BINDIR)/$(EXECNAME)
OBJ = $(OBJDIR)/$(EXECNAME).o

# -------------
# user targets
# -------------

default:  $(EXEC)


$(EXEC): $(OBJ) $(AUXOBJ) gdt
	$(LD) $(CUSTOM_LDFLAGS)  $(OBJ) $(AUXOBJ) $(LIBDIR)/libGDT.a   $(AUXLIB)  -o $(EXEC)

clean:
	rm -f *.o $(OBJ) $(AUXOBJ) *~

delete: clean
	rm -f $(EXEC)

gdt:
	(cd ../../src; $(MAKE) HOST=$(HOST))

# --------------- 
# private targets 
# ---------------

# ---------------------------------------------------
# for compiling under Linux with the "make" command
# or under solaris with the "gmake" command
# ---------------------------------------------------

$(OBJDIR)/%.o: %.cpp  $(TOOLSINCLUDE) $(INCDIR)/GDT/*.h
	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE)

$(OBJDIR)/%.o: %.cc $(TOOLSINCLUDE) $(INCDIR)/GDT/*.h
	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE)

$(OBJDIR)/%.o: %.c  $(TOOLSINCLUDE) $(INCDIR)/GDT/*.h
	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE)


# ---------------------------------------------------
# for compiling under solaris with the "make" command
# ---------------------------------------------------

#$(OBJDIR)/%.o: %.cc $(TOOLSINCLUDE)
#	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE) 

#$(OBJDIR)/%.o: %.c  $(TOOLSINCLUDE) 
#	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE) 


%.c: %.l 
	flex -o$@  $<


#$(OBJ): $(SRC)
#	$(CC) -c $(SRC) -o $(OBJ) $(CFLAGS) $(CINCLUDE) 

dependencies.make: $(SRC) $(INCDIR)/GDT/*.h
	$(CC) $(CINCLUDE) $(CFLAGS) -M $(SRC) >dependencies.make


#dependancies
include dependencies.make





