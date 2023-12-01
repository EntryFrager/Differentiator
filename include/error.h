/// @file error.h

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Macro checking the truth of an expression.
 * @param[in] expr Expression
*/

//#define DEBUG
        //TODO ERROR
#ifdef DEBUG
    size_t CODE_ERROR = 0;
    #define CHECK_ERROR_PRINT(func) if (CODE_ERROR != ERR_NO) my_strerr (CODE_ERROR, stderr);
    #define CHECK_ERROR_RETURN(func) if ((CODE_ERROR |= func) != ERR_NO) return CODE_ERROR; 
#else
    #define CHECK_ERROR_PRINT(func) func
    #define CHECK_ERROR_RETURN(func) func
#endif

#define my_assert(expr) if (!(expr)) {  \
        fprintf(stderr, "\x1b[31m%s %s:%d: My assertion failed: \"" #expr "\"\x1b[0m\n", __FILE__, __func__, __LINE__); \
        abort (); \
    }

enum code_error {                               ///< Error codes.
    ERR_NO            = 0,
    ERR_FOPEN         = 1 << 0,
    ERR_FCLOSE        = 1 << 1,
    ERR_FREAD         = 1 << 2,

    TREE_ERR_PTR      = 1 << 3,
    TREE_INIT         = 1 << 4,
    TREE_ERR_SIZE     = 1 << 5,
    TREE_ERR_ROOT_PTR = 1 << 6,
    
    NODE_ERR          = 1 << 7,

    ERR_CNT           = 8
};

void my_strerr (unsigned code_error, FILE *stream);    ///< Error print function.

#endif // ERROR_H
