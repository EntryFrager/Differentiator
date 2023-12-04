#ifndef DIFF_H
#define DIFF_H

#include <math.h>

#include "include/tree.h"
#include "include/utils.h"
#include "include/error.h"

NODE *diff (NODE *node, int *code_error);

NODE *set_parent (NODE *node, NODE *parent);

NODE *simpl_tree (NODE *node, int *code_error);

#endif //DIFF_H