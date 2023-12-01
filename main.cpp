#include "diff.h"

int main ()
{
    TREE tree = {};

    create_tree (&tree, 0);

    input_base (&tree);

    print_tree (tree.root, stdout);

    return 0;
}