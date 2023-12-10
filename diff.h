#ifndef DIFF_H
#define DIFF_H

#include <math.h>

#include "include/input.h"
#include "include/tree.h"

NODE *n_diff (TREE *tree, size_t n, int *code_error);

NODE *diff (NODE *node, int *code_error);

NODE *tree_simplific (NODE *node, int *code_error);

NODE *hanging_tree (NODE *node, NODE *turn_off_node, NODE *parent, int *code_error);

ELEMENT eval_node (int op, ELEMENT first_value, ELEMENT second_value, int *code_error);

#endif //DIFF_H