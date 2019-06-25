cd ..
nmake -f makefile.win32 clean

copy common.make.win32 common.make.win32.original
copy common.make.win32.time_limited common.make.win32

nmake -f makefile.win32


mkdir blag4.0_TIME_LIMITED

copy bin\blag.exe blag4.0_TIME_LIMITED

copy tools\blag\bin_distribution_stuff\blag.ini blag4.0_TIME_LIMITED
copy tools\blag\bin_distribution_stuff\graph blag4.0_TIME_LIMITED
copy tools\blag\bin_distribution_stuff\README-win32.bin blag4.0_TIME_LIMITED
copy tools\blag\bin_distribution_stuff\run_me.bat blag4.0_TIME_LIMITED

mkdir blag4.0_TIME_LIMITED\examples

copy examples\*.gdt blag4.0_TIME_LIMITED\examples


copy common.make.win32.original common.make.win32