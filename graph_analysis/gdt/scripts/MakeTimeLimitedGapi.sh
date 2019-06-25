#!/bin/sh

cd ..

#----------------------------------------------
#compile in TIME LIMTED mode
#----------------------------------------------

make clean
cp custom.${HOSTTYPE} custom.original
cp public_release/custom.time_limited custom.${HOSTTYPE}
make gdt
make tools

HOST=$HOSTTYPE
RELNUM=4.0_TIME_LIMITED

# --------------------------------------------------
# Step 1 - create a temporary directory
#          named gdt${RELNUM} and its subdirectories
# -------------------------------------------------- 

echo mkdir gdt${RELNUM}
mkdir gdt${RELNUM}

echo mkdir gdt${RELNUM}/incl
mkdir gdt${RELNUM}/incl

echo mkdir gdt${RELNUM}/incl/GDT
mkdir gdt${RELNUM}/incl/GDT

echo mkdir gdt${RELNUM}/bin
mkdir gdt${RELNUM}/bin

echo mkdir gdt${RELNUM}/bin/$HOST/
mkdir gdt${RELNUM}/bin/$HOST

echo mkdir gdt${RELNUM}/lib
mkdir gdt${RELNUM}/lib

echo mkdir gdt${RELNUM}/lib/$HOST
mkdir gdt${RELNUM}/lib/$HOST

echo mkdir gdt${RELNUM}/tools
mkdir gdt${RELNUM}/tools

echo mkdir gdt${RELNUM}/tools/simpletest
mkdir gdt${RELNUM}/tools/simpletest

echo mkdir gdt${RELNUM}/examples
mkdir gdt${RELNUM}/examples

echo mkdir gdt${RELNUM}/output
mkdir gdt${RELNUM}/output

# --------------------------------------
# Step 2 - copy all the relase files in 
#          the subdirectory gdt${RELNUM}
# --------------------------------------


echo cp custom.i386-linux-example gdt${RELNUM}
cp custom.i386-linux-example gdt${RELNUM}

echo cp public_release/public_makefile gdt${RELNUM}/makefile
cp public_release/public_makefile gdt${RELNUM}/makefile

echo cp public_release/README_DEMO gdt${RELNUM}
cp public_release/README_DEMO gdt${RELNUM}

echo cp incl/GDT/*.h gdt${RELNUM}/incl/GDT/
cp incl/GDT/*.h gdt${RELNUM}/incl/GDT/

echo cp -r lib/* gdt${RELNUM}/lib/
cp -r lib/* gdt${RELNUM}/lib/


echo cp bin/$HOST/simpletest gdt${RELNUM}/bin/${HOST}
cp bin/$HOST/simpletest gdt${RELNUM}/bin/${HOST}

echo cp tools/public_common.make gdt${RELNUM}/tools/common.make
cp tools/public_common.make gdt${RELNUM}/tools/common.make


echo cp tools/simpletest/simpletest.cpp gdt${RELNUM}/tools/simpletest/
cp tools/simpletest/simpletest.cpp gdt${RELNUM}/tools/simpletest/
echo cp tools/simpletest/public_makefile gdt${RELNUM}/tools/simpletest/makefile
cp tools/simpletest/public_makefile gdt${RELNUM}/tools/simpletest/makefile

echo cp examples/* gdt${RELNUM}/examples/
cp examples/* gdt${RELNUM}/examples/


# ---------------------------------------------------
# Step 3 - make a compressed archive gdt${RELNUM}.tgz 
#          and remove the subdirectory gdt4.0
# ---------------------------------------------------

echo tar cvzf gdt${RELNUM}_${HOSTTYPE}.tgz gdt${RELNUM}/
tar cvzf gdt${RELNUM}_${HOSTTYPE}.tgz gdt${RELNUM}/

echo rm -rf gdt${RELNUM}/
rm -rf gdt${RELNUM}/


#----------------------------------------------------
# Step 4 - restore the original custom.${HOSTTYPE}
#----------------------------------------------------
cp custom.original custom.${HOST}

