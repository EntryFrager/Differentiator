#include "diff.h"

int main (int argc, char *argv[])
{
    int code_error = 0;

    TREE tree = {};

    create_tree (&tree, argc, argv, &code_error);

    for (size_t i = 0; i < tree.table_name.n_var; i++)
    {
        printf ("%s\n", tree.table_name.vars[i]);
    }
    
    tree.root = n_diff (&tree, 1, 0, &code_error);

    print_tree (tree.root, stdout, &code_error);

    TREE_LOG (&tree, code_error);

    destroy_tree (&tree, &code_error);

    PRINT_ERROR();

    return 0;
}