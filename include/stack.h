/// @file stack.h

#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "error.h"

//#define CANARIES_CHECK                                                                                                  ///< Macro to enable canary protection.

//#define HASH_CHECK                                                                                                      ///< Macro to enable hashed protection.

#ifdef HASH_CHECK
    #define ON_DEBUG(...)                                                       \
    __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#define assert_stack(stack) {                                                   \
    int stack_code_error = 0;                                                   \
    if ((stack_code_error = stack_verification (stack)) != STACK_OK)            \
    {                                                                           \
        stack_dump (stack, stack_code_error, __FILE__, __func__, __LINE__);     \
        abort ();                                                               \
    }                                                                           \
}

typedef int ELEMENT;                                                                                                    ///< The type of stack elements.

const ELEMENT STACK_VALUE_VENOM = -100000;                                                                              ///< Variable containing poison

#ifdef CANARIES_CHECK
    typedef unsigned long long CANARY_TYPE;                                                                             ///< Type of canaries.

    const CANARY_TYPE CANARY = 0xDEDEDEDEDEDEDEDE;                                                                      ///< The meaning of canaries.
#endif

#ifdef HASH_CHECK
    typedef unsigned long long HASH_TYPE;                                                                               ///< Hash type.
#endif

typedef struct {                                                                                                        ///< Structure containing stack values.            
#ifdef CANARIES_CHECK
    CANARY_TYPE left_canary = CANARY;                                                                                   ///< Variable storing the value of the left canary.
#endif

    ELEMENT *data = NULL;                                                                                               ///< Array with stack elements.

    int size = STACK_VALUE_VENOM;                                                                                       ///< Stack size.
    int position = STACK_VALUE_VENOM;                                                                                   ///< The position on the stack at which the next element is written.

#ifdef HASH_CHECK
    HASH_TYPE hash_data = STACK_VALUE_VENOM;                                                                            ///< Variable storing the hash value of an array with stack elements.
    HASH_TYPE hash_struct = STACK_VALUE_VENOM;                                                                          ///< Variable storing the structure hash value.
#endif

#ifdef CANARIES_CHECK
    CANARY_TYPE right_canary = CANARY;                                                                                  ///< Variable storing the value of the right canary.
#endif
} STACK;

enum stack_code_error                                                                                                   ///< Error codes.                                                                               
{
    STACK_OK,                                                                                                           ///< No error.
    STACK_ERR,                                                                                                          ///< Null stack pointer.
    STACK_DATA_ERR,                                                                                                     ///< Null pointer to an array of stack elements.
    STACK_SIZE_ERR,                                                                                                     ///< Invalid stack size value.
    STACK_POSITION_ERR,                                                                                                 ///< Invalid stack position value.
    STACK_SIZE_POS_ERR,                                                                                                 ///< Stack size is less than position.  
    FILE_OPEN_ERR,                                                                                                      ///< Error opening file.
    FILE_CLOSE_ERR,                                                                                                     ///< Error closing file.
    STACK_POINTER_GARBAGE,                                                                                              ///< Stack not created.

    STACK_HASH_STRUCT_ERR,                                                                                              ///< Invalid structure hash value.
    STACK_HASH_DATA_ERR,                                                                                                ///< Invalid array hash value with stack elements.
    STACK_CANARY_ERR,                                                                                                   ///< The canaries died.
    STACK_LEFT_CANARY_ERR,                                                                                              ///< The left canary has died.  
    STACK_RIGHT_CANARY_ERR,                                                                                             ///< The right canary has died.
    STACK_DATA_CANARY_ERR,                                                                                              ///< Canaries in an array with stack elements have died.
    STACK_DATA_LEFT_CANARY_ERR,                                                                                         ///< The left canary in an array with stack elements has died.
    STACK_DATA_RIGHT_CANARY_ERR,                                                                                        ///< The right canary in an array with stack elements has died.
    ERR_STACK_REALLOC,                                                                                                  ///< Error when allocating memory in stack_realloc.
};

const int STACK_ERROR_CNT = 17;                                                                                         ///< Constant indicating the number of possible errors.

void stack_ctor (STACK *stk, const size_t capacity);                                                                    ///< Stack creation function.

void stack_dtor (STACK *stk);                                                                                           ///< Stack clearing function.

void stack_push (STACK *stk, const ELEMENT value);                                                                      ///< Function that adds an element to the stack.

ELEMENT stack_pop (STACK *stk);                                                                                         ///< Function that returns the last element added.

int stack_realloc (STACK *stk, const int size);                                                                         ///< A function that changes the stack size.

#ifdef HASH_CHECK  
HASH_TYPE hash_control_data (STACK *stk);                                                                               ///< Function that creates a hash of an array of stack elements.

HASH_TYPE hash_control_struct (STACK *stk);                                                                             ///< Function that creates a hash of a structure.

HASH_TYPE hash_djb(int str);                                                                                            ///< Hash generating function.
#endif

int stack_verification (STACK *stk);                                                                                    ///< Function that checks the stack for errors.

void stack_dump (STACK *stk, const int code_error, const char *file_err, const char *func_err, const int line_err);     ///< Function that prints error and stack information.

#endif // STACK_H