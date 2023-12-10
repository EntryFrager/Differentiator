#include "tree.h"

#define DEF_CMD(num, name, ...) name,

static const char *NAME_OP[] = {
    #include "comand.h"
};

#undef DEF_CMD

int create_tree (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (tree->root = create_node_num (0, NULL, NULL, NULL, code_error), *code_error);

    tree->init_status = INIT;

    tree->info.fp_name_expr = "include/expr.txt";

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = "include/file_err_tree.txt";
    tree->info.fp_dot_name       = "include/dump.dot";
    tree->info.fp_name_html      = "include/dot.html";
    tree->info.fp_image          = "dot.svg";
    tree->info.fp_tex_name       = "./pdf/diff_expr.tex";
#endif

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

#define calloc_node()                                   \
    NODE *node = (NODE *) calloc (1, sizeof (NODE));    \
    my_assert (node != NULL, ERR_MEM);                  \
    node->left      = left;                             \
    node->right     = right;                            \
    node->parent    = parent;

NODE *create_node_num (ELEMENT value, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    calloc_node ()
    
    node->data.value = value;
    node->type      = NUM;

    return node;
}
NODE *create_node_op (op_command types_op, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    calloc_node ()

    node->data.types_op = types_op;
    node->type          = OP;

    return node;
}

NODE *create_node_var (char var, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    calloc_node ()

    node->data.var = var;
    node->type     = VAR;

    return node;
}

int add_node (NODE *node, ELEMENT value, char var, op_command types_op, int type, const bool side, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    NODE *new_node = NULL;

    if (side == LEFT)
    {
        CHECK_TYPE (type,
                    {CHECK_ERROR_RETURN (new_node = create_node_var (var, node->left, NULL, node, code_error), 
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_num (value, node->left, NULL, node, code_error),
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_op (types_op, node->left, NULL, node, code_error),
                                        *code_error);});

        node->left = new_node;
    }
    else
    {
        CHECK_TYPE (type,
                    {CHECK_ERROR_RETURN (new_node = create_node_var (var, NULL, node->right, node, code_error), 
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_num (value, NULL, node->right, node, code_error),
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_op (types_op, NULL, node->right, node, code_error),
                                        *code_error);});

        node->right = new_node;
    }

    return ERR_NO;
}

NODE *set_parent (NODE *node, NODE *parent)
{
    CHECK_NODE_PTR (NULL);

    node->parent = parent;

    set_parent (node->left, node);
    set_parent (node->right, node);

    return node;
}

int delete_node (NODE *node, int *code_error)
{
    CHECK_NODE_PTR (ERR_NO);

    CHECK_ERROR_RETURN (delete_node (node->left, code_error), *code_error);
    
    CHECK_ERROR_RETURN (delete_node (node->right, code_error), *code_error);

    free (node);

    return ERR_NO;
}

NODE *copy_tree (NODE *node, NODE *parent, int *code_error)
{
    CHECK_NODE_PTR (NULL);

    NODE *copy_node = NULL;

    CHECK_TYPE (node->type,
        {CHECK_ERROR_RETURN (copy_node  = create_node_var (node->data.var, NULL, NULL, parent, code_error),
                            NULL);},
        {CHECK_ERROR_RETURN (copy_node = create_node_num (node->data.value, NULL, NULL, parent, code_error), 
                            NULL);},
        {CHECK_ERROR_RETURN (copy_node = create_node_op (node->data.types_op, NULL, NULL, parent, code_error), 
                            NULL);});


    CHECK_ERROR_RETURN (copy_node->left  = copy_tree (node->left, copy_node, code_error), NULL);
    CHECK_ERROR_RETURN (copy_node->right = copy_tree (node->right, copy_node, code_error), NULL);

    return copy_node;
}

int print_tree (NODE *node, FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    CHECK_NODE_PTR (ERR_NO);

    PRINT_BRACKET (node->left, "(");
    
    if (node->type == OP && node->data.types_op < SIN)
    {
        CHECK_ERROR_RETURN (print_tree (node->left, stream, code_error), *code_error);
    }

    PRINT_BRACKET (node->left, ")");

    CHECK_TYPE (node->type,
                {fprintf (stream, "%c", node->data.var);},
                {fprintf (stream, "%lf", node->data.value);},
                {fprintf (stream, "%s", NAME_OP[node->data.types_op]);});

    PRINT_BRACKET (node->right, "(");

    if (node->type == OP && node->data.types_op >= SIN) {fprintf (stream, "(");}
    
    CHECK_ERROR_RETURN (print_tree (node->right, stream, code_error), *code_error);

    if (node->type == OP && node->data.types_op >= SIN) {fprintf (stream, ")");}

    PRINT_BRACKET (node->right, ")");

    return ERR_NO;
}

int destroy_tree (TREE *tree, int *code_error)
{
    assert_tree (tree, ERR_NO);
    
    CHECK_ERROR_RETURN (delete_node (tree->root, code_error), *code_error);
    tree->root = NULL;

    tree->init_status = INIT_NO;

    tree->info.fp_name_expr = NULL;
    tree->info.size_file = 0;

    free (tree->info.buf);
    tree->info.buf = NULL;

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = NULL;
    tree->info.fp_dot_name       = NULL;
    tree->info.fp_name_html      = NULL;
    tree->info.fp_image          = NULL;
#endif

    return ERR_NO;
}

#ifdef DEBUG_TREE

#define VERIF_EXPR(expr, name_error) if (!(expr)) {code_error |= name_error;}

int tree_verificator (TREE *tree)
{
    int code_error = 0;

    VERIF_EXPR (tree != NULL, TREE_ERR_PTR)

    VERIF_EXPR (tree->init_status == INIT, TREE_INIT)

    VERIF_EXPR (tree->root != NULL, TREE_ERR_ROOT_PTR)

    return code_error;
}

int node_verificator (NODE *node)
{
    int code_error = 0;

    CHECK_NODE_PTR (ERR_NO);

    if (node->parent != NULL)
    {
        VERIF_EXPR (node->parent->left == node || node->parent->right == node, NODE_ERR)
    }

    code_error |= node_verificator (node->left);
    code_error |= node_verificator (node->right);

    return code_error;
}

#undef VERIF_EXPR

#endif