
#ifdef TRACE
#include <iostream>
#define debug(expression) (std::cerr << __FILE__ << ":" << __LINE__ << " -> " << (expression) << std::endl)
#else
#define debug(expression) ((void)0)
#endif

#define error(expression) (std::cout << (expression) << std::endl)

#define GOOD 0                 /// all good
#define UNDECLARED 1           /// no declaration
#define UNDEFINED 2            /// no affectation
#define DOUBLE_DECLARATION 3   /// double declaration
#define RESERVED_KEY_WORD 4    /// reserved keyword
#define VOID_VARIABLE 5        /// void variable
#define UNUSED_VARIABLE 6      /// unused variable
#define PROGRAMER_ERROR -1     /// error from the programmer in the compiler
#define NOT_IMPLEMENTED_YET -2 /// functionality not implemented in the compiler
