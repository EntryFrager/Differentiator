#include "input.h"

static NODE *get_add_sub (char **str, int *code_error);
static NODE *get_mul_div (char **str, int *code_error);
static NODE *get_pow (char **str, int *code_error);
static NODE *get_bracket (char **str, int *code_error);
static NODE *get_trig (char **str, int *code_error);
static op_comand is_trig (char **str, int *code_error);
static NODE *get_var (char **str, int *code_error);
static NODE *get_num (char **str, int *code_error);

int input_expr (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.fp_expr = fopen (tree->info.fp_name_expr, "r + b");
    my_assert (tree->info.fp_expr != NULL, ERR_FOPEN);

    tree->info.size_file = get_file_size (tree->info.fp_expr, code_error);

    tree->info.buf = (char *) calloc (tree->info.size_file + 1, sizeof (char));
    my_assert (tree->info.buf != NULL, ERR_MEM);

    size_t read_size = fread (tree->info.buf, sizeof (char), tree->info.size_file, tree->info.fp_expr);
    my_assert (read_size == tree->info.size_file, ERR_FREAD);

    *(tree->info.buf + tree->info.size_file) = '\0';

    FCLOSE_ (tree->info.fp_expr);

    tree->info.buf = skip_isspace (tree->info.buf, code_error);

    tree->root = get_expr (&tree->info.buf, code_error);

    tree->root = set_parent (tree->root, NULL);

    assert_tree (tree, ERR_TREE);

    $$(*code_error);

    return ERR_NO;
}

NODE *get_expr (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    char *str_ptr = *str;

    NODE *node = get_add_sub (str, code_error);

    syntax_assert (**str == '\0');

    *str = str_ptr;

    $$(NULL);

    return node;
}

NODE *get_add_sub (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);
    
    NODE *node = get_mul_div (str, code_error);

    while (**str == '+' || **str == '-')
    {
        char op = **str;
        (*str)++;
        NODE *node_r = get_mul_div (str, code_error);
        NODE *node_l = copy_tree (node, NULL, code_error);
        delete_node (node);

        switch (op)
        {
            case ('+'):
            {
                node = create_node_op (ADD, node_l, node_r, NULL, code_error);
                break;
            }
            case ('-'):
            {
                node = create_node_op (SUB, node_l, node_r, NULL, code_error);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    $$(NULL);

    return node;
}

NODE *get_mul_div (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    NODE *node = get_pow (str, code_error);

    while (**str == '*' || **str == '/')
    {
        char op = **str;
        (*str)++;
        NODE *node_r = get_pow (str, code_error);
        NODE *node_l = copy_tree (node, NULL, code_error);
        delete_node (node);

        switch (op)
        {
            case ('*'):
            {
                node = create_node_op (MUL, node_l, node_r, NULL, code_error);
                break;
            }
            case ('/'):
            {
                node = create_node_op (DIV, node_l, node_r, NULL, code_error);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    $$(NULL);

    return node;
}

NODE *get_pow (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    NODE *node = get_bracket (str, code_error);

    while (**str == '^')
    {
        (*str)++;

        NODE *node_r = get_bracket (str, code_error);
        NODE *node_l = copy_tree (node, NULL, code_error);
        delete_node (node);

        node = create_node_op (DEG, node_l, node_r, NULL, code_error);
    }

    $$(NULL);

    return node;
}

NODE *get_bracket (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (**str == '(')
    {
        (*str)++;

        NODE *node = get_add_sub (str, code_error);

        syntax_assert (**str == ')');

        (*str)++;

        $$(NULL);

        return node;
    }

    $$(NULL);

    return get_trig (str, code_error);
}

NODE *get_trig (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);
    
    op_comand op = is_trig (str, code_error);
    $$(NULL);

    if (op != OP_NO)
    {
        NODE *node_r = get_bracket (str, code_error);
        $$(NULL);
        
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

#define str_len(str) sizeof (str) - 1

op_comand is_trig (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (strncmp (*str, "sin", str_len ("sin")) == 0)
    {
        *str += str_len ("sin");

        return SIN;
    }
    else if (strncmp (*str, "cos", str_len ("cos")) == 0)
    {
        *str += str_len ("cos");

        return COS;
    }
    else if (strncmp (*str, "sqrt", str_len ("sqrt")) == 0)
    {
        *str += str_len ("sqrt");

        return SQRT;
    }
    else if (strncmp (*str, "ln", str_len ("ln")) == 0)
    {
        *str += str_len ("ln");

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
        return get_num (str, code_error);
    }
}

NODE *get_num (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    double value = 0;
    int n_read = 0;

    sscanf (*str, "%lf%n", &value, &n_read);

    *str += n_read;

    return create_node_num (value, NULL, NULL, NULL, code_error);
}