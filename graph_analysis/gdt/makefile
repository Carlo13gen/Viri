#------------------------------------------------------------------------------
#  This file is part of the GDToolkit distribution
#  (c) 1999-2005 Third University of Rome (DIA) & University of Perugia (DIEI) 
#
#  This file contains commands to automatically compile and link 
#  the GDToolkit library and its related tools.
#   
#  use "#" to comment lines
#
# ------------------------------------------------------------------------------

HOST=$$HOSTTYPE

# ------------
# user targets
# ------------

# ----------------
# make all targets
# ----------------

all: .chk dirs gdt tools 

# -------------------------
# create the Doxygen documentation
# -------------------------

doc:
	(cd incl/GDT; doxygen ../../Doxyfile; cd ../..)

# -------------------------
# remove the Doxygen documentation
# -------------------------

del_doc:
	rm -Rf documentation

# -------------------------
# compile gdt library only 
# -------------------------

gdt: dirs 
	(cd src; $(MAKE) HOST=$(HOST))


# -------------------------
# compile gdt library and wrapper 
# -------------------------

wrap: .chk dirs 
	(cd src; $(MAKE) gdtdyn_wrapper HOST=$(HOST))


wrap-test: tools/java_test/javaTest.class
	 java  -classpath java_wrapper/bin:tools/java_test \
	 -Djava.library.path=lib/$(HOST)  javaTest

# -------------------------
# compile all tools 
# -------------------------

tools: dirs
	(cd tools; $(MAKE) all HOST=$(HOST))

# -------------------------
# compile single tools 
# -------------------------

simpletest:  
	(cd tools/simpletest; $(MAKE) HOST=$(HOST))

test:  
	(cd tools/test; $(MAKE) HOST=$(HOST))

#gdtcommline:  
#	(cd tools/gdtcommline; $(MAKE) HOST=$(HOST))
	
# -------------------------
# execute self-test 	
# -------------------------
	
self-test:
	(cd tools/test; ../../bin/$(HOST)/test)

# -------------------------
# clean all obj files  
# but not the library obj
# -------------------------

clean: 
	(cd src; $(MAKE)  -e clean HOST=$(HOST) )
	(cd tools; $(MAKE) -e clean HOST=$(HOST) )
	(cd tools/java_test; rm -f `find -name *.class`; rm -f `find -name *~`)
	(cd lib/$(HOST); rm -f *)
	(cd java_wrapper/bin; rm -rf GdtJavaWrapper; cd ../cpp/include; rm -f GdtJavaWrapper_WrapperImplementation.h; cd ../src/; rm -f *.o )

# -------------------------------------
# delete all obj and bin files
# including the library obj
# -------------------------------------

delete: 
	(cd src; $(MAKE) delete HOST=$(HOST))
	(cd tools; $(MAKE) delete HOST=$(HOST) )
	rm -R lib/$(HOST) obj/$(HOST)
	mkdir bin/$(HOST)/delete_me
	rmdir -p bin/$(HOST)/delete_me

# -----------------------------
# delete every obj and bin file
# -----------------------------

delall: clean
	rm -R lib obj bin


# ------------------------------------
# private targets: directories manager
# ------------------------------------

dirs: .chk lib/$(HOST) bin/$(HOST) obj/$(HOST)

deldirs:
	rm -R lib bin obj

lib: 
	mkdir -p lib

bin:
	mkdir -p bin

obj:
	mkdir -p obj


lib/$(HOST): lib
	-mkdir -p lib/$(HOST)

bin/$(HOST): bin
	-mkdir -p bin/$(HOST)

obj/$(HOST): obj
	-mkdir -p obj/$(HOST)

.chk: .host custom.$(HOST)


tools/java_test/javaTest.class: wrap tools/java_test/javaTest.java
	cd tools/java_test; \
	javac -Xlint:unchecked -classpath ../../java_wrapper/bin javaTest.java; 
	
# -------------------
# Warnings and Errors
# -------------------

.host:
	@echo ""
	@if [ $(HOST) = "" ]; then \
	echo "The environment variable HOSTTYPE is not set.";\
	echo "Please read the troubleshooting section in the INSTALL file";\
	echo "";\
	exit 1;\
	fi

custom.$(HOST):
	@echo ""
	@if [ ! -f custom.$(HOST) ]; then \
	echo "Please create a file \"custom.$(HOST)\" for your platform";\
	echo "according to the example \"custom.i386-linux-example\".";\
	echo "If you experience any problem, please email to gdt@dia.uniroma3.it";\
	echo "";\
	exit 1;\
	fi

home:
	@echo ""
	@if [ -z $(GDTHOME) ]; then \
	echo "The environment variable GDTHOME is not set.";\
	echo "Please create a GDTHOME environment variable corresponding to the GDToolkit installation directory.";\
	echo "";\
	exit 1;\
	fi
