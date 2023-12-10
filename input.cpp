#include "input.h"

int input_expr (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.fp_expr = fopen (tree->info.fp_name_expr, "r + b");
    my_assert (tree->info.fp_expr != NULL, ERR_FOPEN);

    CHECK_ERROR_RETURN (tree->info.size_file = get_file_size (tree->info.fp_expr, code_error), *code_error);

    tree->info.buf = (char *) calloc (tree->info.size_file + 1, sizeof (char));
    my_assert (tree->info.buf != NULL, ERR_MEM);

    size_t read_size = fread (tree->info.buf, sizeof (char), tree->info.size_file, tree->info.fp_expr);
    my_assert (read_size == tree->info.size_file, ERR_FREAD);

    *(tree->info.buf + tree->info.size_file) = '\0';

    FCLOSE_ (tree->info.fp_expr);

    tree->info.buf = skip_isspace (tree->info.buf, code_error);

    tree->root = get_g (&tree->info.buf, code_error);

    tree->root = set_parent (tree->root, NULL);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

NODE *get_g (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (NODE *node = get_e (str, code_error), NULL);

    syntax_assert (**str == '\0');

    return node;
}

NODE *get_e (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);
    
    CHECK_ERROR_RETURN (NODE *node = get_t (str, code_error), NULL);

    while (**str == '+' || **str == '-')
    {
        char op = **str;
        (*str)++;
        CHECK_ERROR_RETURN (NODE *node_r = get_t (str, code_error), NULL);
        CHECK_ERROR_RETURN (NODE *node_l = copy_tree (node, NULL, code_error), NULL);
        CHECK_ERROR_RETURN (delete_node (node, code_error), NULL);

        switch (op)
        {
            case ('+'):
            {
                CHECK_ERROR_RETURN (node = create_node_op (ADD, node_l, node_r, NULL, code_error), NULL);
                break;
            }
            case ('-'):
            {
                CHECK_ERROR_RETURN (node = create_node_op (SUB, node_l, node_r, NULL, code_error), NULL);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return node;
}

NODE *get_t (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (NODE *node = get_d (str, code_error), NULL);

    while (**str == '*' || **str == '/')
    {
        char op = **str;
        (*str)++;
        CHECK_ERROR_RETURN (NODE *node_r = get_d (str, code_error), NULL);
        CHECK_ERROR_RETURN (NODE *node_l = copy_tree (node, NULL, code_error), NULL);
        CHECK_ERROR_RETURN (delete_node (node, code_error), NULL);

        switch (op)
        {
            case ('*'):
            {
                CHECK_ERROR_RETURN (node = create_node_op (MUL, node_l, node_r, NULL, code_error), NULL);
                break;
            }
            case ('/'):
            {
                CHECK_ERROR_RETURN (node = create_node_op (DIV, node_l, node_r, NULL, code_error), NULL);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return node;
}

NODE *get_d (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (NODE *node = get_p (str, code_error), NULL);

    while (**str == '^')
    {
        (*str)++;

        CHECK_ERROR_RETURN (NODE *node_r = get_p (str, code_error), NULL);
        CHECK_ERROR_RETURN (NODE *node_l = copy_tree (node, NULL, code_error), NULL);
        CHECK_ERROR_RETURN (delete_node (node, code_error), NULL);

        CHECK_ERROR_RETURN (node = create_node_op (DEG, node_l, node_r, NULL, code_error), NULL);
    }

    return node;
}

NODE *get_p (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (**str == '(')
    {
        (*str)++;

        CHECK_ERROR_RETURN (NODE *node = get_e (str, code_error), NULL);

        syntax_assert (**str == ')');

        (*str)++;

        return node;
    }
    else
    {
        return get_trig (str, code_error);
    }
}

NODE *get_trig (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);
    
    CHECK_ERROR_RETURN (op_command op = is_trig (str, code_error), NULL);

    if (op != OP_NO)
    {
        CHECK_ERROR_RETURN (NODE *node_r = get_p (str, code_error), NULL);
        
        switch (op)
        {
            case (SIN):
            {
                return create_node_op (SIN, NUM_(1, NULL), node_r, NULL, code_error);
            }
            case (COS):
            {
                return create_node_op (COS, NUM_(1, NULL), node_r, NULL, code_error);
            }
            case (SQRT):
            {
                return create_node_op (SQRT, NUM_(1, NULL), node_r, NULL, code_error);
            }
            case (LN):
            {
                return create_node_op (LN, NUM_(1, NULL), node_r, NULL, code_error);
            }
            default:
            {
                break;
            }
        }
    }
    else
    {
        return get_var (str, code_error);
    }

    return NULL;
}

op_command is_trig (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (strncmp (*str, "sin", sizeof ("sin") - 1) == 0)
    {
        *str += sizeof ("sin") - 1;

        return SIN;
    }
    else if (strncmp (*str, "cos", sizeof ("cos") - 1) == 0)
    {
        *str += sizeof ("cos") - 1;

        return COS;
    }
    else if (strncmp (*str, "sqrt", sizeof ("sqrt") - 1) == 0)
    {
        *str += sizeof ("sqrt") - 1;

        return SQRT;
    }
    else if (strncmp (*str, "ln", sizeof ("ln") - 1) == 0)
    {
        *str += sizeof ("ln") - 1;

        return LN;
    }
 
    return OP_NO;
}

NODE *get_var (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (isalpha (**str))
    {
        char var = **str;
        (*str)++;

        return create_node_var (var, NULL, NULL, NULL, code_error);
    }
    else
    {
        return get_n (str, code_error);
    }
}

NODE *get_n (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    double value = 0;
    int n_read = 0;

    sscanf (*str, "%lf%n", &value, &n_read);

    *str += n_read; 

    return create_node_num (value, NULL, NULL, NULL, code_error);
}