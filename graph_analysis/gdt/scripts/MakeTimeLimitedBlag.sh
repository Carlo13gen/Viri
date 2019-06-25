#!/bin/sh

#----------------------------------------------
#compile in TIME LIMITED mode
#----------------------------------------------
cd ..

make clean
cp custom.${HOSTTYPE} custom.original
cp public_release/custom.time_limited custom.${HOSTTYPE}
make gdt
make tools

HOST=$HOSTTYPE
RELNUM=4.0_TIME_LIMITED

# --------------------------------------------------
# Step 1 - create a temporary directory
#          named blag${RELNUM} and its subdirectories
# -------------------------------------------------- 

echo mkdir blag${RELNUM}
mkdir blag${RELNUM}


echo mkdir blag${RELNUM}/examples
mkdir blag${RELNUM}/examples


# --------------------------------------
# Step 2 - copy all the relase files in 
#          the subdirectory blag${RELNUM}
# --------------------------------------


echo cp bin/$HOST/blag blag${RELNUM}
cp bin/$HOST/blag blag${RELNUM}

echo cp tools/blag/bin_distribution_stuff/blag.ini blag${RELNUM}
cp tools/blag/bin_distribution_stuff/blag.ini blag${RELNUM}

echo cp tools/blag/bin_distribution_stuff/graph blag${RELNUM}
cp tools/blag/bin_distribution_stuff/graph blag${RELNUM}

echo cp tools/blag/bin_distribution_stuff/run_me.sh blag${RELNUM}
cp tools/blag/bin_distribution_stuff/run_me.sh blag${RELNUM}

echo cp tools/blag/bin_distribution_stuff/README-unix.bin blag${RELNUM}
cp tools/blag/bin_distribution_stuff/README-unix.bin blag${RELNUM}


echo cp examples/* blag${RELNUM}/examples/
cp examples/* blag${RELNUM}/examples/


# ---------------------------------------------------
# Step 3 - make a compressed archive gdt${RELNUM}.tgz 
#          and remove the subdirectory gdt4.0
# ---------------------------------------------------

echo tar cvzf blag${RELNUM}_${HOSTTYPE}.tgz blag${RELNUM}/
tar cvzf blag${RELNUM}_${HOSTTYPE}.tgz blag${RELNUM}/

echo rm -rf blag${RELNUM}/
rm -rf blag${RELNUM}/


#----------------------------------------------------
# Step 4 - restore the original custom.${HOSTTYPE}
#----------------------------------------------------
cp custom.original custom.${HOST}

