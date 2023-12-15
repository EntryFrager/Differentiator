#include "diff.h"

int main (int argc, char *argv[])
{
    int code_error = 0;

    TREE tree = {};

    create_tree (&tree, argc, argv, &code_error);
    
    print_tex (&tree, &code_error);

    TREE_LOG (&tree, code_error);

    destroy_tree (&tree, &code_error);

    PRINT_ERROR();

    return 0;
}