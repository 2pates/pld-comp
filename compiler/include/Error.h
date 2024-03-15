
#ifdef TRACE
#include <iostream>
#define debug(expression) (std::cerr << __FILE__ << ":" << __LINE__ << \
" -> " << (expression) << std::endl)
#else
#define debug(expression) ((void)0)
#endif


#define EXIST 0      // all good
#define UNDECLARED 1 // no declaration
#define UNDEFINED 2  // no affectation
#define DOUBLE_DECLARATION 3
#define RESERVED_KEY_WORD 4
#define PROGRAMER_ERROR -1
