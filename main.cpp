#include "diff.h"

int main ()
{
    int code_error = 0;

    TREE tree = {};

    CHECK_ERROR_PRINT (create_tree (&tree, &code_error));

    CHECK_ERROR_PRINT (input_expr (&tree, &code_error));

    CHECK_ERROR_PRINT (print_tree (tree.root, stdout, &code_error));
    printf ("\n");
    
    CHECK_ERROR_PRINT (tree.root = diff (tree.root, &code_error));

    set_parent (tree.root, NULL);

    CHECK_ERROR_PRINT (print_tree (tree.root, stdout, &code_error));

    TREE_LOG (&tree, 0);

    CHECK_ERROR_PRINT (tree.root = tree_simplific (tree.root, &code_error));

    printf ("\n");

    CHECK_ERROR_PRINT (print_tree (tree.root, stdout, &code_error));

    CHECK_ERROR_PRINT (destroy_tree (&tree, &code_error));

    return 0;
}