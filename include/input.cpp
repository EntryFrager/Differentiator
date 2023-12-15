#include "input.h"

static void get_token (TREE *tree, int *code_error);
static size_t get_n_tokens (char *str, int *code_error);
static void get_table_name (TREE *tree, int *code_error);
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
    get_token (tree, code_error);
    
    tree->root = get_expr (tree->token, code_error);
    tree->root = set_parent (tree->root, NULL);

    assert_tree (tree, ERR_TREE);

    $$ (*code_error);

    return ERR_NO;
}

void get_token (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    char *str = tree->info.buf;

    tree->n_token = get_n_tokens (str, code_error);
    $$ ();

    tree->token = (TOKEN *) calloc (tree->n_token + 1, sizeof (TOKEN));
    my_assert (tree->token != NULL, ERR_MEM);

    op_comand op = OP_NO;

    for (size_t i = 0; i < tree->n_token; i++)
    {
        if ((op = is_func (&str, code_error)) != OP_NO)
        {
            tree->token[i].type = OP;
            tree->token[i].data.types_op = op;
        }
        else if (isdigit (*str))
        {
            tree->token[i].type = NUM;
            tree->token[i].data.var = str;

            while (isdigit (*str))
            {
                str++;
            }
        }
        else if (isalpha (*str))
        {
            tree->token[i].type = VAR;
            tree->token[i].data.var = str;
            tree->table_name.n_var++;
            
            while (isalpha (*str))
            {
                str++;
            }
        }
        else
        {
            tree->token[i].type = OP;

            switch (*str)
            {
                case ('+'):
                {
                    tree->token[i].data.types_op = ADD;
                    break;
                }
                case ('-'):
                {
                    tree->token[i].data.types_op = SUB;
                    break;
                }
                case ('*'):
                {
                    tree->token[i].data.types_op = MUL;
                    break;
                }
                case ('/'):
                {
                    tree->token[i].data.types_op = DIV;
                    break;
                }
                case ('^'):
                {
                    tree->token[i].data.types_op = DEG;
                    break;
                }
                case ('('):
                {
                    tree->token[i].data.types_op = OPEN_BRACKET;
                    break;
                }
                case (')'):
                {
                    tree->token[i].data.types_op = CLOSE_BRACKET;
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

    tree->token[tree->n_token].type = DEF_TYPE;

    get_table_name (tree, code_error);
}

size_t get_n_tokens (char *str, int *code_error)
{
    size_t n_tokens = 0;

    while (*str != '\0')
    {
        if (*str == '+' || *str == '-' || *str == '*' || *str == '^' || *str == '/' || *str == '(' || *str == ')')
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

            while (isdigit (*str) || isalpha (*str))
            {
                str++;
            }
        }
    }
    
    return n_tokens;
}

void get_table_name (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_MEM);

    tree->table_name.vars = (char **) calloc (tree->table_name.n_var, sizeof (char *));
    my_assert (tree->table_name.vars != NULL, ERR_MEM);

    for (size_t i = 0; i < tree->n_token; i++)
    {
        if (tree->token[i].type == VAR)
        {
            for (size_t var_pos = 0; var_pos < tree->table_name.n_var; var_pos++)
            {
                if (tree->table_name.vars[var_pos] == NULL)
                {
                    tree->table_name.vars[var_pos] = tree->token[i].data.var;
                    break;
                }
                else if (strcmp (tree->table_name.vars[var_pos], tree->token[i].data.var) == 0)
                {
                    break;
                }
            }
        }
    }
}

NODE *get_expr (TOKEN *token, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    size_t *pos = (size_t *) calloc (1, sizeof (size_t));
    my_assert (pos != NULL, ERR_MEM);

    NODE *node = get_add_sub (token, pos, code_error);

    syntax_assert (token[*pos].type == DEF_TYPE);

    $$ (NULL);

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

    $$( NULL);

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

    $$( NULL);

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

    $$ (NULL);

    return node;
}

NODE *get_bracket (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    if (token[*pos].type == OP && token[*pos].data.types_op == OPEN_BRACKET)
    {
        (*pos)++;

        NODE *node = get_add_sub (token, pos, code_error);

        syntax_assert (token[*pos].type == OP && token[*pos].data.types_op == CLOSE_BRACKET);

        (*pos)++;

        $$ (NULL);

        return node;
    }

    $$ (NULL);

    return get_trig (token, pos, code_error);
}

NODE *get_trig (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    if (token[*pos].type == OP)
    {
        op_comand op = token[(*pos)++].data.types_op;

        NODE *node_r = get_bracket (token, pos, code_error);
        $$ (NULL);
        
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