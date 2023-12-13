/// @file error.h

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Macro checking the truth of an expression.
 * @param[in] expr Expression
*/

#define DEBUG

#undef $$
//
#ifdef DEBUG
    #define PRINT_ERROR() if (code_error != ERR_NO) my_strerr (code_error, stderr);
    #define $$(ret_value) if (*code_error != 0) {return ret_value;}
#else
    #define PRINT_ERROR()
    #define $$(...)
#endif

#define my_assert(expr, cd_err) if (!(expr)) {  \
        fprintf(stderr, "\x1b[31m%s %s:%d: My assertion failed: \"" #expr "\"\x1b[0m\n", __FILE__, __func__, __LINE__); \
        *code_error |= cd_err;  \
    }

enum code_error {                               ///< Error codes.
    ERR_NO            = 0,
    ERR_PTR           = 1 << 0,
    ERR_NAN           = 1 << 1,
    ERR_MEM           = 1 << 2,
    ERR_FOPEN         = 1 << 3,
    ERR_FCLOSE        = 1 << 4,
    ERR_FREAD         = 1 << 5,
    ERR_DIV_ZERO      = 1 << 6,
    ERR_SQRT_NEGAT    = 1 << 7,
    ERR_LOG_NEGAT     = 1 << 8,
    SYNTAX_ERROR      = 1 << 9,
    ERR_TREE          = 1 << 10,

    TREE_ERR_PTR      = 1 << 11,
    TREE_INIT         = 1 << 12,
    TREE_ERR_ROOT_PTR = 1 << 13,
    
    NODE_ERR          = 1 << 14,
    NODE_DATA_ERR_PTR = 1 << 15,

    ERR_CNT           = 16
};

void my_strerr (unsigned code_error, FILE *stream);    ///< Error print function.

#endif // ERROR_H
