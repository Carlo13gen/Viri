#include <GDT/gdt_error.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

/** Handle errors, notifying the user with the name of the file in
 * which the error occurred, the line, the function and the message.
 * If compiled in GraficVersion mode a window is opened. */

void __handle_error(const char* file, 
                    const int line, 
                    const char* function, 
                    const char* error) {
  
  cerr << "\n" << file << ":" << line << ": " << "gdt error";  
  cerr << "\n" << file << ":" << line << ": In method `" << function << "\'";
  cerr << "\n" << file << ":" << line << ": " << error << "\n";
  exit(1);
}

/** Handles textual messages */

void __handle_textual_message(const char* file, 
                              const int line, 
                              const char* function, 
                              const char* warning) {
  
  cerr << "\n" << file << ":" << line << ": " << "gdt warning";  
  cerr << "\n" << file << ":" << line << ": In method `" << function << "\'";
  cerr << "\n" << file << ":" << line << ": " << warning << "\n";
}

