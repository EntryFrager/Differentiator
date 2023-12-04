#include "diff.h"

int main ()
{
    int code_error = 0;

    /*TREE tree = {};
    DATA data = {};

    CHECK_ERROR_PRINT (create_tree (&tree, &data, &code_error));

    CHECK_ERROR_PRINT (input_base (&tree, &code_error));

    CHECK_ERROR_PRINT (print_tree (tree.root, stdout, &code_error));

    printf ("\n");

    CHECK_ERROR_PRINT (tree.root = diff (tree.root, &code_error));

    set_parent (tree.root, NULL);

    TREE_LOG (&tree, 0);

    CHECK_ERROR_PRINT (print_tree (tree.root, stdout, &code_error));

    CHECK_ERROR_PRINT (destroy_tree (&tree, &code_error));*/


    printf ("%d\n", ERR_CNT);

    return 0;
}