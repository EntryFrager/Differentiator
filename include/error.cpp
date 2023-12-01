/// @file error.cpp

#include "error.h"

/**
 * Types of errors.
*/

#define DEF_ERR(name) name,

static const char *err_msgs_arr[] = {
    #include "error_name.h"
};

#undef DEF_ERR

/**
 * Error print function.
 * @param[in] code_error
*/

void my_strerr (unsigned code_error, FILE *stream)
{
    for (int i = 0; i < ERR_CNT; i++)
    {
        if (code_error & (1 << i))
        {
            fprintf (stream, "\x1b[31mERROR: %s\x1b[0m\n", err_msgs_arr[i]);
        }
    }
}
