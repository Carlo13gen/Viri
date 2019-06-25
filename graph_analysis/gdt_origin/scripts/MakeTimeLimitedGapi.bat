cd ..
nmake -f makefile.win32 clean


rem **** Replace the configuration file with the demo configuration
copy common.make.win32 common.make.win32.original
copy public_release\common.make.win32.time_limited common.make.win32

nmake -f makefile.win32


set VERSION=4.0_TIME_LIMITED

mkdir gdt%VERSION%
mkdir gdt%VERSION%\incl
mkdir gdt%VERSION%\incl\GDT

mkdir gdt%VERSION%\bin
mkdir gdt%VERSION%\lib
mkdir gdt%VERSION%\tools

mkdir gdt%VERSION%\tools\simpletest
mkdir gdt%VERSION%\examples
mkdir gdt%VERSION%\output



rem ****** Copy public files in the target directory 


copy custom.win32 gdt%VERSION%

copy public_release\public-makefile.win32 gdt%VERSIOn%\makefile.win32

copy common.make.win32 gdt%VERSION%

copy public_release\README_DEMO_WIN32.txt gdt%VERSION%\README_WIN32.txt

copy incl\GDT\*.h gdt%VERSION%\incl\GDT\

copy lib\*.* gdt%VERSION%\lib\

copy bin\simpletest.exe gdt%VERSION%\bin\

copy tools\simpletest\simpletest.cpp gdt%VERSIOn%\tools\simpletest

copy tools\common.make.win32 gdt%VERSION%\tools

copy tools\simpletest\makefile.win32 gdt%VERSION%\tools\simpletest


copy examples\*.* gdt%VERSION%\examples\

copy make.bat gdt%VERSIOn%\



rem ****** restore the original configurationn file
copy common.make.win32.original common.make.win32
del common.make.win32.original
