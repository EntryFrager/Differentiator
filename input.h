#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>

#include "include/tree.h"
#include "include/utils.h"

#define syntax_assert(expr) if (!expr) {printf ("\x1b[31mSYNTAX ERROR: expected \"" #expr "\"\x1b[0m"); *code_error |= SYNTAX_ERROR;}

int input_expr (TREE *tree, int *code_error);

NODE *get_g (char **str, int *code_error);

NODE *get_e (char **str, int *code_error);

NODE *get_d (char **str, int *code_error);

NODE *get_t (char **str, int *code_error);

NODE *get_p (char **str, int *code_error);

NODE *get_trig (char **str, int *code_error);

op_command is_trig (char **str, int *code_error);

NODE *get_var (char **str, int *code_error);

NODE *get_n (char **str, int *code_error);

#endif //INPUT_H