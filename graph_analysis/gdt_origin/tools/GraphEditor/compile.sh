#!/bin/sh

#HOME = $$GDTHOME

if [ -z $GDTHOME ]; then  
	echo
	echo	
	echo The variable GDTHOME is not set.
	echo
	echo Please create a GDTHOME environment variable corresponding to the GDTookit home directory 
	exit 1  
elif [ ! -e $GDTHOME/lib/$HOSTTYPE/GdtJavaWrapper.jar ]; then 
	echo
	echo
	echo GDToolkit Java Wrapper not found, compiling the Java Wrapper
	cd $GDTHOME
	make wrap
	
	cd $GDTHOME/tools/GraphEditor
fi

echo  Starting GDToolkit graph editor compilation...

javac -cp $GDTHOME/lib/$HOSTTYPE/GdtJavaWrapper.jar `find -name *.java` 

echo  End of compilation.

