#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>

#include "../diff.h"
#include "tree.h"
#include "utils.h"

#define syntax_assert(expr) if (!expr) {printf ("\x1b[31mSYNTAX ERROR: expected \"" #expr "\"\x1b[0m"); *code_error |= SYNTAX_ERROR;}

int input_expr (TREE *tree, int *code_error);

NODE *get_expr (TOKEN *token, int *code_error);

#endif //INPUT_H