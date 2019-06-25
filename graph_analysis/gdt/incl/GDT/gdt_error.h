/*! \file */

#ifndef __gdterror__
#define __gdterror__


#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ "<unknow function>"
#endif

#define gdt_warning(s) \
__handle_warning(__FILE__,__LINE__,__PRETTY_FUNCTION__,s)

#define gdt_error(s) \
__handle_error(__FILE__,__LINE__,__PRETTY_FUNCTION__,s)


void __handle_error(const char* file, 
                    const int line, 
                    const char* function, 
                    const char* error);

void __handle_warning(const char* file, 
                      const int line, 
                      const char* function, 
                      const char* warning);



#endif
