#include "diff.h"

static NODE *add_simplific (NODE *node, int *code_error);
static NODE *sub_simplific (NODE *node, int *code_error);
static NODE *mul_simplific (NODE *node, int *code_error);
static NODE *div_simplific (NODE *node, int *code_error);
static NODE *pow_simplific (NODE *node, int *code_error);
static void get_var_diff (TREE *tree, int *code_error);
static void get_pow_taylor (TREE *tree, int *code_error);
static void get_val_point (TREE *tree, int *code_error);
static void check_var_diff (TREE *tree, int *code_error);
static void check_pow_taylor (TREE *tree, int *code_error);

NODE *n_diff (TREE *tree, size_t n, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    for (size_t i = 0; i < n; i++)
    {
        tree->root = diff (tree->root, tree->info.var_diff, code_error);
        ERR_RET (NULL);

        tree->root = tree_simplific (tree->root, code_error);
        ERR_RET (NULL);

        set_parent (tree->root, NULL);
    }
    
    tree->root->tree_size = get_tree_size (tree->root);

    return tree->root;
}

NODE *diff (NODE *node, char *var, int *code_error)
{
    IS_NODE_PTR_NULL (NULL);

    switch (node->type)
    {
        case (NUM):
        {
            return NUM_(0, NULL);
        }
        case (VAR):
        {
            if (strcmp (node->data.var, var) == 0)
            {
                return NUM_(1, NULL);
            }
            else
            {
                return NUM_(0, NULL);
            }
        }
        case (OP):
        {
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    return ADD_(DIF_L, DIF_R);
                }
                case (SUB):
                {
                    return SUB_(DIF_L, DIF_R);
                }
                case (MUL):
                {
                    return ADD_(MUL_(DIF_L, COPY_R), MUL_(COPY_L, DIF_R));
                }
                case (DIV):
                {
                    return DIV_(SUB_(MUL_(DIF_L, COPY_R), MUL_(COPY_L, DIF_R)), MUL_(COPY_R, COPY_R));
                }
                case (POW):
                {
                    if (L_TYPE == NUM && R_TYPE == VAR)
                    {
                        return MUL_(LN_(COPY_L), MUL_(POW_(COPY_L, COPY_R), DIF_R));
                    }

                    return MUL_(MUL_(COPY_R, POW_(COPY_L, SUB_(COPY_R, NUM_(1, NULL)))), DIF_L);
                }
                case (SIN):
                {
                    return MUL_(COS_(COPY_R), DIF_R);
                }
                case (COS):
                {
                    return MUL_(SIN_(COPY_R), MUL_(NUM_(-1, NULL), DIF_R));
                }
                case (SQRT):
                {
                    return MUL_(MUL_(NUM_(0.5, NULL), POW_(COPY_R, NUM_(-0.5, NULL))), DIF_R);
                }
                case (LN):
                {
                    return MUL_(DIV_(NUM_(1, NULL), COPY_R), DIF_R);
                }
                case (OP_NO):
                case (OP_SEP):
                case (OPEN_BRACKET):
                case (CLOSE_BRACKET):
                default:
                {
                    break;
                }
            }
        }
        case (DEF_TYPE): {}
        default:
        {
            break;
        }
    }

    return node;
}

NODE *tree_simplific (NODE *node, int *code_error)
{
    IS_NODE_PTR_NULL (NULL);

    if (node->type != OP)
    {
        return node;
    }

    node->left  = tree_simplific (node->left, code_error);
    node->right = tree_simplific (node->right, code_error);

    if (node->type == OP)
    {
        if (L_TYPE == NUM && R_TYPE == NUM)
        {
            ELEMENT value = eval_node (node->data.types_op, L_VALUE, R_VALUE, code_error);
            node = NUM_ (value, node->parent);
        }
        else
        {
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    node = add_simplific (node, code_error);
                    ERR_RET (NULL);
                    
                    break;
                }
                case (SUB):
                {
                    node = sub_simplific (node, code_error);
                    ERR_RET (NULL);

                    break;
                }
                case (MUL):
                {
                    node = mul_simplific (node, code_error);
                    ERR_RET (NULL);

                    break;
                }
                case (DIV):
                {
                    node = div_simplific (node, code_error);
                    ERR_RET (NULL);

                    break;
                }
                case (POW):
                {
                    node = pow_simplific (node, code_error);
                    ERR_RET (NULL);

                    break;
                }
                case (OP_NO):
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
    }

    ERR_RET (NULL);

    return node;
}

NODE *add_simplific (NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    IF_NUM (L_TYPE, {if (is_zero (L_VALUE)) {R_RE_HANGING;}})
    IF_NUM (R_TYPE, {if (is_zero (R_VALUE)) {L_RE_HANGING;}})

    return node;
}

NODE *sub_simplific (NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    IF_NUM (R_TYPE, {if (is_zero (R_VALUE)) {L_RE_HANGING;}})

    return node;
}

NODE *mul_simplific (NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    IF_NUM (L_TYPE,
            if (is_zero (L_VALUE))
            {
                L_RE_HANGING;
            }
            else if (is_one (L_VALUE))
            {
                R_RE_HANGING;
            })
    IF_NUM (R_TYPE,
            if (is_zero (R_VALUE))
            {
                R_RE_HANGING;
            }
            else if (is_one (R_VALUE))
            {
                L_RE_HANGING;
            })
    return node;
}

NODE *div_simplific (NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    IF_NUM (L_TYPE,
            if (is_zero (L_VALUE))
            {
                L_RE_HANGING;
            })
    IF_NUM (R_TYPE,
            if (is_one (R_VALUE))
            {
                R_RE_HANGING;
            })

    return node;
}

NODE *pow_simplific (NODE *node, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    IF_NUM (L_TYPE,
            if (is_zero (L_VALUE) || is_one (L_VALUE))
            {
                L_RE_HANGING;
            })
    IF_NUM (R_TYPE,
            if (is_zero (R_VALUE))
            {
                R_VALUE = 1;
                R_RE_HANGING;
            }
            else if (is_one (R_VALUE))
            {
                L_RE_HANGING;
            })

    return node;
}

NODE *hanging_tree (NODE *node, NODE *hanging_node, NODE *parent, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (hanging_node != NULL, ERR_PTR);

    if (parent != NULL)
    {
        if (parent->left == node)
        {
            parent->left = hanging_node;
        }
        else if (parent->right == node)
        {
            parent->right = hanging_node;
        }
    }

    hanging_node->parent = parent;

    if (hanging_node == node->left)
    { 
        delete_node (node->right);
    }
    else if (hanging_node == node->right)
    {
        delete_node (node->left);
    }

    free (node);

    ERR_RET (NULL);

    return hanging_node;
}

ELEMENT eval_tree (NODE *node, ELEMENT var_value, int *code_error)
{
    IS_NODE_PTR_NULL (ERR_NO);

    switch (node->type)
    {
        case (NUM):
        {
            return node->data.value;
        }
        case (VAR):
        {
            return var_value;
        }
        case (OP):
        {
            return eval_node (node->data.types_op, 
                              eval_tree (node->left, var_value, code_error), 
                              eval_tree (node->right, var_value, code_error), 
                              code_error);
        }
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }

    return ERR_NO;
}

#define DEF_EVAL(op) first_value op second_value

ELEMENT eval_node (int op, ELEMENT first_value, ELEMENT second_value, int *code_error)
{
    switch (op)
    {
        case (ADD):
        {
            return DEF_EVAL (+);
        }
        case (SUB):
        {
            return DEF_EVAL (-);
        }
        case (MUL):
        {
            return DEF_EVAL (*);
        }
        case (DIV):
        {
            if (!is_zero (second_value))
            {
                return DEF_EVAL (/);
            }
            else
            {
                *code_error |= ERR_DIV_ZERO;
            }
        }
        case (POW):
        {
            return pow (first_value, second_value);
        }
        case (SIN):
        {
            return sin (second_value);
        }
        case (COS):
        {
            return cos (second_value);
        }
        case (SQRT):
        {
            if (second_value >= 0)
            {
                return pow (second_value, 0.5);
            }
            else
            {
                *code_error |= ERR_SQRT_NEGAT;
            }
        }
        case (LN):
        {
            if (second_value > 0)
            {
                return log (second_value);
            }
            else
            {
                *code_error |= ERR_LOG_NEGAT;
            }
        }
        default:
        {
            break;
        }
    }

    return ERR_NO;
}

#undef my_eval

void get_user_config (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);
    
    get_var_diff (tree, code_error);
    ERR_RET ();

    get_pow_taylor (tree, code_error);
    ERR_RET ();

    get_val_point (tree, code_error);
    ERR_RET ();
}

void get_var_diff (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    printf ("What variable do you want to differentiate on? ");
    tree->info.var_diff = get_str (stdin, code_error);
    ERR_RET ();

    check_var_diff (tree, code_error);
    ERR_RET ();
}

void get_pow_taylor (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    printf ("What degree do you want the Taylor expansion to be? ");

    while (scanf ("%d", &tree->info.pow_taylor) != 1)
    {
        printf ("Enter a number: ");
    }

    check_pow_taylor (tree, code_error);
    ERR_RET ();
}

void get_val_point (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    printf ("Enter the value of the point at which you want to calculate the value: ");

    while (scanf ("%lf", &tree->info.value_point) != 1)
    {
        printf ("Enter a number: ");
    }
}

void check_var_diff (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    for (size_t i = 0; i < tree->table_name.n_var; i++)
    {
        if (strcmp (tree->info.var_diff, tree->table_name.vars[i]) == 0)
        {
            break;
        }
        else if (i == tree->table_name.n_var - 1)
        {
            printf ("There is no such variable, try again.\n");
            get_var_diff (tree, code_error);
            ERR_RET ();

            check_var_diff (tree, code_error);
            ERR_RET ();
        }
    }
}

void check_pow_taylor (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    while (tree->info.pow_taylor < 0)
    {
        printf ("Enter a positive number.\n");

        get_pow_taylor (tree, code_error);
        ERR_RET ();
    }
}