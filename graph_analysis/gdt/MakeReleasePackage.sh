#!/bin/csh

set RELNUM=4.0 

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

echo mkdir gdt${RELNUM}/src
mkdir gdt${RELNUM}/src

echo mkdir gdt${RELNUM}/tools
mkdir gdt${RELNUM}/tools

echo mkdir gdt${RELNUM}/tools/simpletest
mkdir gdt${RELNUM}/tools/simpletest

echo mkdir gdt${RELNUM}/tools/test
mkdir gdt${RELNUM}/tools/test

echo mkdir gdt${RELNUM}/tools/test/graphs  
mkdir gdt${RELNUM}/tools/test/graphs


# --------------------------------------
# Step 2 - copy all the relase files in 
#          the subdirectory gdt${RELNUM}
# --------------------------------------


echo cp custom.i386-linux-example gdt${RELNUM}
cp custom.i386-linux-example gdt${RELNUM}

echo cp makefile gdt${RELNUM}
cp makefile gdt${RELNUM}

echo cp README gdt${RELNUM}
cp README gdt${RELNUM}

echo cp incl/GDT/*.h gdt${RELNUM}/incl/GDT/
cp incl/GDT/*.h gdt${RELNUM}/incl/GDT/

echo cp src/*.cpp gdt${RELNUM}/src/
cp src/*.cpp gdt${RELNUM}/src/
echo cp src/*.cc gdt${RELNUM}/src/
cp src/*.cc gdt${RELNUM}/src/
echo cp src/makefile gdt${RELNUM}/src/
cp src/makefile gdt${RELNUM}/src/

echo cp tools/common.make gdt${RELNUM}/tools/
cp tools/common.make gdt${RELNUM}/tools/
echo cp tools/makefile gdt${RELNUM}/tools/
cp tools/makefile gdt${RELNUM}/tools/

echo cp tools/simpletest/simpletest.cpp gdt${RELNUM}/tools/simpletest/
cp tools/simpletest/simpletest.cpp gdt${RELNUM}/tools/simpletest/
echo cp tools/simpletest/makefile gdt${RELNUM}/tools/simpletest/
cp tools/simpletest/makefile gdt${RELNUM}/tools/simpletest/

echo cp tools/test/test.cpp gdt${RELNUM}/tools/test/
cp tools/test/test.cpp gdt${RELNUM}/tools/test/
echo cp tools/test/test_table gdt${RELNUM}/tools/test/
cp tools/test/test_table gdt${RELNUM}/tools/test/
echo cp tools/test/README gdt${RELNUM}/tools/test/
cp tools/test/README gdt${RELNUM}/tools/test/
echo cp tools/test/graphs/*.gdt* gdt${RELNUM}/tools/test/graphs
cp tools/test/graphs/*.gdt* gdt${RELNUM}/tools/test/graphs
echo cp tools/test/makefile gdt${RELNUM}/tools/test/
cp tools/test/makefile gdt${RELNUM}/tools/test/


# ---------------------------------------------------
# Step 3 - make a compressed archive gdt${RELNUM}.tgz 
#          and remove the subdirectory gdt4.0
# ---------------------------------------------------

echo tar cvzf gdt${RELNUM}.tgz gdt${RELNUM}/
tar cvzf gdt${RELNUM}.tgz gdt${RELNUM}/

echo rm -rf gdt${RELNUM}/
rm -rf gdt${RELNUM}/
