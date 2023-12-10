#include "diff.h"

int main ()
{
    int code_error = 0;

    TREE tree = {};

    CHECK_ERROR_PRINT (create_tree (&tree, &code_error));

    CHECK_ERROR_PRINT (input_expr (&tree, &code_error));
    
    CHECK_ERROR_PRINT (tree.root = n_diff (&tree, 2, &code_error));

    TREE_LOG (&tree, code_error);

    CHECK_ERROR_PRINT (destroy_tree (&tree, &code_error));

    return 0;
}