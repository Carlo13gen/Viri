

include ../custom.$(HOST)

OBJDIR       = ../obj/$(HOST)
LIBDIR       = ../lib/$(HOST)
LIBDIRDYNA   = ../lib/$(HOST)
INCDIR 	     = ../incl

OBJECTS =\
	  $(OBJDIR)/date.o \
	  $(OBJDIR)/rel_coord_orth.o \
          $(OBJDIR)/gdt_graph_array.o \
	  $(OBJDIR)/gdt_error.o \
	  $(OBJDIR)/gdt_geometry.o \
	  $(OBJDIR)/stopwatch.o \
	  $(OBJDIR)/_rm3_global.o \
	  $(OBJDIR)/_rm3_simple_graph.o \
	  $(OBJDIR)/_rm3_constraints.o \
	  $(OBJDIR)/_rm3_undi_graph.o \
	  $(OBJDIR)/_rm3_dire_graph.o \
	  $(OBJDIR)/_rm3_plan_undi_graph.o \
	  $(OBJDIR)/_rm3_orth_plan_undi_graph.o \
	  $(OBJDIR)/_rm3_upwa_plan_undi_graph.o \
	  $(OBJDIR)/_rm3_flow_dire_graph.o \
          $(OBJDIR)/_rm3_dime_slicing.o \
	  $(OBJDIR)/_rm3_dime_orth_plan_undi_graph.o \
	  $(OBJDIR)/_rm3_draw_undi_graph.o \
	  $(OBJDIR)/_rm3_tree.o \
          $(OBJDIR)/_rm3_SPQR_tree.o \
	  $(OBJDIR)/_rm3_interval_set.o \
	  $(OBJDIR)/_rm3_layered_undi_graph.o \
	  $(OBJDIR)/_rm3_morpher.o 

JAVADIR= ../java_wrapper/bin
JAVASRC= ../java_wrapper/src
JAVACPPSRC= ../java_wrapper/cpp/src
JAVACPPINCL= ../java_wrapper/cpp/include

JAVA_CLASSES= \
	$(JAVADIR)/GdtJavaWrapper/Wrapper.class \
	$(JAVADIR)/GdtJavaWrapper/Node.class \
	$(JAVADIR)/GdtJavaWrapper/Edge.class \
	$(JAVADIR)/GdtJavaWrapper/Graph.class \
	$(JAVADIR)/GdtJavaWrapper/SPQRNode.class \
	$(JAVADIR)/GdtJavaWrapper/SPQRTree.class \
	$(JAVADIR)/GdtJavaWrapper/OrthogonalGraph.class \
	$(JAVADIR)/GdtJavaWrapper/Drawer.class \
	$(JAVADIR)/GdtJavaWrapper/OrthogonalDrawer.class 


CFLAGS   = $(CUSTOM_CFLAGS) -fPIC
CINCLUDE = -I$(INCDIR) 

# ------------
# user targets
# ------------

default:  $(LIBDIR)/libGDT.a

allobjs: $(OBJECTS)


$(LIBDIR)/libGDT.a: $(OBJECTS)
	ar cr $@ $?
	$(RANLIB) $@
	
# ----------------------
# make a dynamic library
# ----------------------

gdtdin: $(OBJECTS)
	$(CC) --shared  -o $(LIBDIRDYNA)/libGDT.so $(OBJECTS)


# ------------------------------------------
# make a dynamic library with a java wrapper
# ------------------------------------------


gdtdyn_wrapper: allobjs  wrapper $(LIBDIRDYNA)/libGdtJavaWrapper.so


#############################################################################
#OLD TARGETS, NO LONGER USED
java:
	cd ../java_wrapper; ./compile.sh;  \
	mv bin/GdtJavaWrapper.jar $(LIBDIRDYNA); 

old_wrapper: java
	cd $(JAVACPPSRC) \
	$(CC) -c -fPIC GdtJavaWrapper.cpp -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux \
	-I../../../incl -I../../../incl/GDT -I../include; 


jar: $(JAVA_CLASSES) 
	jar cvf $(JAVADIR)/GdtJavaWrapper.jar $(JAVADIR)/GdtJavaWrapper/*.*
	mv $(JAVADIR)/GdtJavaWrapper.jar $(LIBDIRDYNA); 

##############################################################################

wrapper: $(LIBDIRDYNA)/GdtJavaWrapper.jar $(JAVACPPSRC)/GdtJavaWrapper.o   
	

orthobox:
	cd ../tools/orthobox; \
	$(CC) -I../../incl -c -fpic orthobox.cpp -o orthobox.o
	

# ----------------------
# delete all obj files 
# for the src files
# ----------------------

clean:
	rm -f $(OBJECTS) *~ *.obj *.cfg *.ttitt
	

# ----------------------
# delete all obj files 
# for the src files,
# and also the obj for 
# library
# ----------------------

delete: clean
	rm -f $(LIBDIR)/libGDT.a
	rm -f $(LIBDIR)/libGDT.so

# ---------------
# private targets
# ---------------

# -------------------------------------------------
# for compiling under Linux with the "make" command
# or under solaris with the "gmake" command
# -------------------------------------------------

$(OBJDIR)/%.o: %.c $(INCDIR)/GDT/*.h
	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE) 

$(OBJDIR)/%.o: %.cpp $(INCDIR)/GDT/*.h
	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE)

$(OBJDIR)/%.o: %.cc $(INCDIR)/GDT/*.h
	$(CC) -c $< -o $@ $(CFLAGS) $(CINCLUDE)

##############################################################################
# I need these targets to handle the Java classes dependencies

$(JAVADIR)/GdtJavaWrapper/%.class: $(JAVASRC)/%.java
	javac -Xlint -classpath $(JAVADIR) -d $(JAVADIR) $<  


########################################################################################

../lib/$(HOST)/GdtJavaWrapper.jar: $(JAVA_CLASSES)
	cd $(JAVADIR); \
	jar cvf ../$(LIBDIRDYNA)/GdtJavaWrapper.jar GdtJavaWrapper/*.*; \
	cd ../../src; 

$(JAVACPPSRC)/GdtJavaWrapper.o: $(JAVACPPSRC)/GdtJavaWrapper.cpp $(JAVACPPINCL)/*.h 
	cd $(JAVACPPINCL); \
	javah -jni -classpath ../../bin GdtJavaWrapper.WrapperImplementation; \
	cd ../src;  make

$(LIBDIRDYNA)/libGdtJavaWrapper.so: $(OBJECTS) $(JAVACPPSRC)/GdtJavaWrapper.o
	$(CC) --shared  -o $(LIBDIRDYNA)/libGdtJavaWrapper.so $(OBJECTS) \
	$(JAVACPPSRC)/GdtJavaWrapper.o 

dependencies.make: *.cpp *.c 
	$(CC) $(CINCLUDE) $(CFLAGS) -M *.c *.cpp >dependencies.make


#dependancies
#include dependencies.make

