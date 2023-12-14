#include "tree.h"
#include "input.h"

#define DEF_CMD(num, name, ...) name,

static const char *NAME_OP[] = {
    #include "comand.h"
};

#undef DEF_CMD

static NODE *calloc_node (NODE *left, NODE *right, NODE *parent, int *code_error);

int create_tree (TREE *tree, int argc, char *argv[], int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->root = create_node_num (0, NULL, NULL, NULL, code_error);
    $$(ERR_NO);

    tree->is_init = true;

    if (argc == 2)
    {
        tree->info.fp_name_expr = argv[1];
    }
    else
    {
        tree->info.fp_name_expr = "include/expr.txt";
    }

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = "include/file_err_tree.txt";
    tree->info.fp_dot_name       = "include/dump.dot";
    tree->info.fp_name_html      = "include/dot.html";
    tree->info.fp_image          = "dot.svg";
    tree->info.fp_tex_name       = "./pdf/diff_expr.tex";
#endif

    input_expr (tree, code_error);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

NODE *calloc_node (NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = (NODE *) calloc (1, sizeof (NODE));
    my_assert (node != NULL, ERR_MEM);
    node->left      = left;
    node->right     = right;
    node->parent    = parent;

    return node;
}

NODE *create_node_num (ELEMENT value, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    $$(NULL);
    
    node->data.value = value;
    node->type       = NUM;

    return node;
}
NODE *create_node_op (op_comand types_op, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    $$(NULL);

    node->data.types_op = types_op;
    node->type          = OP;

    return node;
}

NODE *create_node_var (char *var, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = calloc_node (left, right, parent, code_error);
    $$(NULL);

    node->data.var = var;
    node->type     = VAR;

    return node;
}

NODE *set_parent (NODE *node, NODE *parent)
{
    IS_NODE_PTR_NULL (NULL);

    node->parent = parent;

    set_parent (node->left, node);
    set_parent (node->right, node);

    return node;
}

int delete_node (NODE *node)
{
    IS_NODE_PTR_NULL (ERR_NO);

    delete_node (node->left);

    delete_node (node->right);

    free (node);

    return ERR_NO;
}

NODE *copy_tree (NODE *node, NODE *parent, int *code_error)
{
    IS_NODE_PTR_NULL (NULL);

    NODE *copy_node = NULL;

    switch (node->type)
    {
        case (NUM):
        {
            copy_node = create_node_num (node->data.value, NULL, NULL, parent, code_error);
            $$(NULL);

            break;
        }
        case (OP):
        {
            copy_node = create_node_op (node->data.types_op, NULL, NULL, parent, code_error);
            $$(NULL);
            
            break;
        }
        case (VAR):
        {
            copy_node = create_node_var (node->data.var, NULL, NULL, parent, code_error);
            $$(NULL);

            break;
        }
        case (DEF_TYPE): {}
        default:
        {
            break;
        }
    }

    copy_node->left  = copy_tree (node->left, copy_node, code_error);
    $$(NULL);

    copy_node->right = copy_tree (node->right, copy_node, code_error);
    $$(NULL);

    return copy_node;
}

#define PRINT_NODE(str) fprintf (stream, str);
#define PRINT_NODE_PARAM(str, ...) fprintf (stream, str, __VA_ARGS__);

void print_tree (NODE *node, FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    IS_NODE_PTR_NULL ();

    switch (node->type)
    {
        case (NUM):
        {
            print_num (node, stream, code_error);
            $$ ();

            break;
        }
        case (VAR):
        {
            PRINT_NODE_PARAM ("%s", node->data.var);
            break;
        }
        case (OP):
        {
            print_operator (node, stream, code_error);
            $$ ();

            break;
        }
        case (DEF_TYPE): {}
        default:
        {
            break;
        }
    }
}

void print_num (NODE *node, FILE *stream, int *code_error)
{
    my_assert (node != NULL, ERR_PTR)
    my_assert (stream != NULL, ERR_PTR)
    
    if (node->data.value < 0)
    {
        PRINT_NODE_PARAM ("(%lg)", node->data.value);
    }
    else
    {
        PRINT_NODE_PARAM ("%lg", node->data.value);
    }
}

void print_operator (NODE *node, FILE *stream, int *code_error)
{
    my_assert (node != NULL, ERR_PTR)
    my_assert (stream != NULL, ERR_PTR)

    bool is_bracket = false;

    if (node->data.types_op < OP_SEP)
    {
        is_bracket = print_left_node_bracket (node, stream, code_error);
        $$()

        print_tree (node->left, stream, code_error);
        $$ ();

        PRINT_CLOSE_BRACKET (stream);
    }

    PRINT_NODE_PARAM ("%s", NAME_OP[node->data.types_op]);

    is_bracket = print_right_node_bracket (node, stream, code_error);

    print_tree (node->right, stream, code_error);
    $$ ();
    
    PRINT_CLOSE_BRACKET (stream);
}

bool print_left_node_bracket (NODE *node, FILE *stream, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (stream != NULL, ERR_PTR);

    bool is_bracket = print_bracket (node, node->left, stream, code_error);
    $$ (false);

    return is_bracket;
}

bool print_right_node_bracket (NODE *node, FILE *stream, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (stream != NULL, ERR_PTR);

    bool is_bracket = false;

    if (node->data.types_op > OP_SEP)
    {
        PRINT_NODE ("(");
        is_bracket = true;
    }
    else 
    {
        is_bracket = print_bracket (node, node->right, stream, code_error);
        $$(false);
    }

    return is_bracket;
}

bool print_bracket (NODE *node, NODE *node_side, FILE *stream, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (stream != NULL, ERR_PTR);
    
    if (node->data.types_op == MUL)
    {
        if (node_side->type == OP)
        {
            if (node_side->data.types_op == DEG || node_side->data.types_op == ADD || node_side->data.types_op == SUB)
            {
                PRINT_NODE ("(");
                return true;
            }
        }
    }
    else if (node->data.types_op == DIV)
    {
        if (node_side->type == OP)
        {
            PRINT_NODE ("(");
            return true;
        }
    }

    return false;
}

#undef PRINT_NODE
#undef PRINT_NODE_PARAM

int destroy_tree (TREE *tree, int *code_error)
{
    assert_tree (tree, ERR_NO);
    
    delete_node (tree->root);
    tree->root = NULL;

    tree->is_init = false;

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

    VERIF_EXPR (tree->is_init == true, TREE_INIT)

    VERIF_EXPR (tree->root != NULL, TREE_ERR_ROOT_PTR)

    return code_error;
}

int node_verificator (NODE *node)
{
    int code_error = 0;

    IS_NODE_PTR_NULL (ERR_NO);

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