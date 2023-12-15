#include "input.h"

static void get_str_file (TREE *tree, int *code_error);
static void get_token (TREE *tree, int *code_error);
static size_t get_n_tokens (char *str, int *code_error);
static op_comand get_op (char str);
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

    get_str_file (tree, code_error);
    ERR_RET (*code_error);

    tree->info.buf = skip_isspace (tree->info.buf, code_error);
    ERR_RET (*code_error);

    get_token (tree, code_error);
    ERR_RET (*code_error);
    
    tree->root = get_expr (tree->token, code_error);
    ERR_RET (*code_error);

    tree->root = set_parent (tree->root, NULL);
    ERR_RET (*code_error);
    
    tree->root->tree_size = get_tree_size (tree->root);

    get_user_config (tree, code_error);
    ERR_RET (*code_error);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

void get_str_file (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    tree->info.fp_expr = fopen (tree->info.fp_name_expr, "r + b");
    my_assert (tree->info.fp_expr != NULL, ERR_FOPEN);

    tree->info.size_file = get_file_size (tree->info.fp_expr, code_error);
    ERR_RET ();

    tree->info.buf = (char *) calloc (tree->info.size_file + 1, sizeof (char));
    my_assert (tree->info.buf != NULL, ERR_MEM);

    size_t read_size = fread (tree->info.buf, sizeof (char), tree->info.size_file, tree->info.fp_expr);
    my_assert (read_size == tree->info.size_file, ERR_FREAD);

    tree->info.buf[tree->info.size_file] = '\0';

    FCLOSE_ (tree->info.fp_expr);
}

void get_token (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    char *str = tree->info.buf;

    tree->n_token = get_n_tokens (str, code_error);
    ERR_RET ();

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

            size_t n_read = 0;
            sscanf (str, "%lf%n", &tree->token[i].data.value, &n_read);
            str += n_read;
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

            tree->token[i].data.types_op = get_op (*str);

            *str++ = '\0';
        }
    }

    tree->token[tree->n_token].type = DEF_TYPE;

    get_table_name (tree, code_error);
}

size_t get_n_tokens (char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);
    
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

#define OP_CMP(op_name, op_code) case (op_name): {return op_code;}

op_comand get_op (char str)
{
    switch (str)
    {
        OP_CMP ('+', ADD)
        OP_CMP ('-', SUB)
        OP_CMP ('*', MUL)
        OP_CMP ('/', DIV)
        OP_CMP ('^', POW)
        OP_CMP ('(', OPEN_BRACKET)
        OP_CMP (')', CLOSE_BRACKET)
        default:
        {
            return OP_NO;
        }
    }
}

#undef OP_CMP

void get_table_name (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_MEM);

    tree->table_name.vars = (char **) calloc (tree->table_name.n_var, sizeof (char *));
    my_assert (tree->table_name.vars != NULL, ERR_MEM);

    size_t count_var = 0;

    for (size_t i = 0; i < tree->n_token; i++)
    {
        if (tree->token[i].type == VAR)
        {
            for (size_t var_pos = 0; var_pos <= count_var; var_pos++)
            {
                if (tree->table_name.vars[var_pos] == NULL)
                {
                    tree->table_name.vars[var_pos] = tree->token[i].data.var;
                    count_var++;

                    break;
                }
                else if (strcmp (tree->table_name.vars[var_pos], tree->token[i].data.var) == 0)
                {                    
                    break;
                }
            }
        }
    }
    
    tree->table_name.n_var = count_var;

    tree->table_name.vars = (char **) realloc (tree->table_name.vars, tree->table_name.n_var * sizeof (char *));
    my_assert (tree->table_name.vars != NULL, ERR_MEM);
}

NODE *get_expr (TOKEN *token, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    size_t pos = 0;

    NODE *node = get_add_sub (token, &pos, code_error);

    syntax_assert (token[pos].type == DEF_TYPE);

    ERR_RET (NULL);

    return node;
}

NODE *get_add_sub (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);
    
    NODE *node = get_mul_div (token, pos, code_error);

    while (CUR_TOK.type == OP && (IS_OP (ADD) || IS_OP (SUB)))
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_mul_div (token, pos, code_error);
        NODE *node_l = node;

        switch (op)
        {
            case (ADD):
            {
                node = ADD_ (node_l, node_r);
                break;
            }
            case (SUB):
            {
                node = SUB_ (node_l, node_r);
                break;
            }
            case (OP_NO):
            case (MUL):
            case (DIV):
            case (POW):
            case (OP_SEP):
            case (SIN):
            case (COS):
            case (SQRT):
            case (LN):
            case (OPEN_BRACKET):
            case (CLOSE_BRACKET):
            default:
            {
                break;
            }
        }
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_mul_div (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    NODE *node = get_pow (token, pos, code_error);

    while (CUR_TOK.type == OP && (IS_OP (MUL) || IS_OP (DIV)))
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_pow (token, pos, code_error);
        NODE *node_l = node;

        switch (op)
        {
            case (MUL):
            {
                node = MUL_ (node_l, node_r);
                break;
            }
            case (DIV):
            {
                node = DIV_ (node_l, node_r);
                break;
            }
            case (OP_NO):
            case (ADD):
            case (SUB):
            case (POW):
            case (OP_SEP):
            case (SIN):
            case (COS):
            case (SQRT):
            case (LN):
            case (OPEN_BRACKET):
            case (CLOSE_BRACKET):
            default:
            {
                break;
            }
        }
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_pow (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    NODE *node = get_bracket (token, pos, code_error);

    while (CUR_TOK.type == OP && IS_OP (POW)) 
    {
        INCREM;

        NODE *node_r = get_bracket (token, pos, code_error);
        NODE *node_l = node;

        node = POW_ (node_l, node_r);
    }

    ERR_RET (NULL);

    return node;
}

NODE *get_bracket (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    if (CUR_TOK.type == OP && IS_OP (OPEN_BRACKET))
    {
        INCREM;

        NODE *node = get_add_sub (token, pos, code_error);

        syntax_assert (CUR_TOK.type == OP && IS_OP (CLOSE_BRACKET));
        INCREM;

        ERR_RET (NULL);

        return node;
    }

    ERR_RET (NULL);

    return get_trig (token, pos, code_error);
}

NODE *get_trig (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    if (token[*pos].type == OP)
    {
        op_comand op = CUR_TOK.data.types_op;
        INCREM;

        NODE *node_r = get_bracket (token, pos, code_error);
        ERR_RET (NULL);
        
        switch (op)
        {
            case (SIN):
            {
                return SIN_ (node_r);
            }
            case (COS):
            {
                return COS_ (node_r);
            }
            case (SQRT):
            {
                return SQRT_ (node_r);
            }
            case (LN):
            {
                return LN_ (node_r);
            }
            case (OP_NO):
            case (ADD):
            case (SUB):
            case (MUL):
            case (DIV):
            case (POW):
            case (OP_SEP):
            case (OPEN_BRACKET):
            case (CLOSE_BRACKET):
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
            return VAR_ (token[INCREM].data.var, NULL);
        }
        case (NUM):
        {
            return get_num (token, pos, code_error);
        }
        case (DEF_TYPE):
        case (OP):
        default:
        {
            return NULL;
        }
    }
}

NODE *get_num (TOKEN *token, size_t *pos, int *code_error)
{
    my_assert (token != NULL, ERR_PTR);

    return NUM_ (token[INCREM].data.value, NULL);
}

#define FUNC_CMP(func_name, func_code)                          \
    if (strncmp (*str, func_name, str_len (func_name)) == 0)    \
    {                                                           \
        *str += str_len (func_name);                            \
        return func_code;                                       \
    } else                                                      \

op_comand is_func (char **str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    FUNC_CMP ("sin", SIN)
    FUNC_CMP ("cos", COS)
    FUNC_CMP ("SQRT", SQRT)
    FUNC_CMP ("ln", LN)
    {}
 
    return OP_NO;
}

#undef FUNC_CMP