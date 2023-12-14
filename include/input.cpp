#include "input.h"

static TOKEN *get_token (char *str, int *code_error);
static size_t get_n_tokens (char *str, int *code_error);
static NODE *get_add_sub (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_mul_div (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_pow (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_bracket (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_trig (TOKEN *token, size_t *pos, int *code_error);
static op_comand is_func (char **str, int *code_error);
static NODE *get_var (TOKEN *token, size_t *pos, int *code_error);
static NODE *get_num (TOKEN *token, size_t *pos, int *code_error);

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

    tree->info.buf[tree->info.size_file] = '\0';

    FCLOSE_ (tree->info.fp_expr);

    tree->info.buf = skip_isspace (tree->info.buf, code_error);
    tree->token = get_token (tree->info.buf, code_error);
    
    tree->root = get_expr (tree->token, code_error);
    tree->root = set_parent (tree->root, NULL);

    assert_tree (tree, ERR_TREE);

    $$(*code_error);

    return ERR_NO;
}

TOKEN *get_token (char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    size_t n_token = get_n_tokens (str, code_error);
    TOKEN *token = (TOKEN *) calloc (n_token + 1, sizeof (TOKEN));
    my_assert (token != NULL, ERR_MEM);

    op_comand op = OP_NO;

    for (size_t i = 0; i < n_token; i++)
    {
        if ((op = is_func (&str, code_error)) != OP_NO)
        {
            token[i].type = OP;
            token[i].data.types_op = op;
        }
        else if (isdigit (*str))
        {
            token[i].type = NUM;
            token[i].data.var = str;

            while (isdigit (*str))
            {
                str++;
            }
        }
        else if (isalpha (*str))
        {
            token[i].type = VAR;
            token[i].data.var = str;
            
            while (isalpha (*str))
            {
                str++;
            }
        }
        else
        {
            token[i].type = OP;

            switch (*str)
            {
                case ('+'):
                {
                    token[i].data.types_op = ADD;
                    break;
                }
                case ('-'):
                {
                    token[i].data.types_op = SUB;
                    break;
                }
                case ('*'):
                {
                    token[i].data.types_op = MUL;
                    break;
                }
                case ('/'):
                {
                    token[i].data.types_op = DIV;
                    break;
                }
                case ('^'):
                {
                    token[i].data.types_op = DEG;
                    break;
                }
                case ('('):
                {
                    token[i].data.types_op = OPEN_BRACKET;
                    break;
                }
                case (')'):
                {
                    token[i].data.types_op = CLOSE_BRACKET;
                    break;
                }
                default:
                {
                    break;
                }
            }

            *str++ = '\0';
        }
    }

    token[n_token].type = DEF_TYPE;

    return token;
}

size_t get_n_tokens (char *str, int *code_error)
{
    size_t n_tokens = 0;

    while (*str != '\0')
    {
        if (*str == '+' || *str == '-' || *str == '*' || *str == '/' || *str == '(' || *str == ')')
        {
            n_tokens++;
            str++;
        }
        else if (is_func (&str, code_error) != OP_NO)
        {
            n_tokens++;
        }
        else if (isdigit (*str) || isalpha (*str))
        {
            n_tokens++;
            str++;
        }
    }
    
    return n_tokens;
}

NODE *get_expr (TOKEN *token, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    size_t *pos = (size_t *) calloc (1, sizeof (size_t));
    my_assert (pos != NULL, ERR_MEM);

    NODE *node = get_add_sub (token, pos, code_error);

    printf ("%d\n", *pos);
    syntax_assert (token[*pos].type == DEF_TYPE);

    $$(NULL);

    free (pos);

    return node;
}

NODE *get_add_sub (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    
    NODE *node = get_mul_div (token, pos, code_error);

    while (token[*pos].type == OP && (token[*pos].data.types_op == ADD || token[*pos].data.types_op == SUB))
    {
        op_comand op = token[*pos].data.types_op;
        (*pos)++;
        NODE *node_r = get_mul_div (token, pos, code_error);
        NODE *node_l = copy_tree (node, NULL, code_error);
        delete_node (node);

        switch (op)
        {
            case (ADD):
            {
                node = create_node_op (ADD, node_l, node_r, NULL, code_error);
                break;
            }
            case (SUB):
            {
                node = create_node_op (SUB, node_l, node_r, NULL, code_error);
                break;
            }
            case (OP_NO): {}
            case (MUL): {}
            case (DIV): {}
            case (DEG): {}
            case (OP_SEP): {}
            case (SIN): {}
            case (COS): {}
            case (SQRT): {}
            case (LN): {}
            case (OPEN_BRACKET): {}
            case (CLOSE_BRACKET): {}
            default:
            {
                break;
            }
        }
    }

    $$(NULL);

    return node;
}

NODE *get_mul_div (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    NODE *node = get_pow (token, pos, code_error);

    while (token[*pos].type == OP && (token[*pos].data.types_op == MUL || token[*pos].data.types_op == DIV))
    {
        op_comand op = token[*pos].data.types_op;
        (*pos)++;
        NODE *node_r = get_pow (token, pos, code_error);
        NODE *node_l = copy_tree (node, NULL, code_error);
        delete_node (node);

        switch (op)
        {
            case (MUL):
            {
                node = create_node_op (MUL, node_l, node_r, NULL, code_error);
                break;
            }
            case (DIV):
            {
                node = create_node_op (DIV, node_l, node_r, NULL, code_error);
                break;
            }
            case (OP_NO): {}
            case (ADD): {}
            case (SUB): {}
            case (DEG): {}
            case (OP_SEP): {}
            case (SIN): {}
            case (COS): {}
            case (SQRT): {}
            case (LN): {}
            case (OPEN_BRACKET): {}
            case (CLOSE_BRACKET): {}
            default:
            {
                break;
            }
        }
    }

    $$(NULL);

    return node;
}

NODE *get_pow (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    NODE *node = get_bracket (token, pos, code_error);

    while (token[*pos].type == OP && token[*pos].data.types_op == DEG)
    {
        (*pos)++;

        NODE *node_r = get_bracket (token, pos, code_error);
        NODE *node_l = copy_tree (node, NULL, code_error);
        delete_node (node);

        node = create_node_op (DEG, node_l, node_r, NULL, code_error);
    }

    $$(NULL);

    return node;
}

NODE *get_bracket (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    if (token[*pos].type == OP && token[*pos].data.types_op == OPEN_BRACKET)
    {
        (*pos)++;

        NODE *node = get_add_sub (token, pos, code_error);

        syntax_assert (token[*pos].type == OP && token[*pos].data.types_op == DEG);

        (*pos)++;

        $$(NULL);

        return node;
    }

    $$(NULL);

    return get_trig (token, pos, code_error);
}

NODE *get_trig (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    if (token[*pos].type == OP)
    {
        NODE *node_r = get_bracket (token, pos, code_error);
        $$(NULL);
        
        switch (token[*pos].data.types_op)
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
            case (OP_NO): {}
            case (ADD): {}
            case (SUB): {}
            case (MUL): {}
            case (DIV): {}
            case (DEG): {}
            case (OP_SEP): {}
            case (OPEN_BRACKET): {}
            case (CLOSE_BRACKET): {}
            default:
            {
                break;
            }
        }
    }
    else
    {
        return get_var (token, pos, code_error);
    }

    return NULL;
}

NODE *get_var (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    switch (token[*pos].type)
    {
        case (VAR):
        {
            return create_node_var (token[(*pos)++].data.var, NULL, NULL, NULL, code_error);
        }
        case (NUM):
        {
            return get_num (token, pos, code_error);
        }
        case (DEF_TYPE): {}
        case (OP): {}
        default:
        {
            return NULL;
        }
    }
}

NODE *get_num (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    double value = 0;

    sscanf (token[*pos].data.var, "%lf", &value);

    (*pos)++;

    return create_node_num (value, NULL, NULL, NULL, code_error);
}

op_comand is_func (char **str, int *code_error)
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