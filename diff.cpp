#include "diff.h"

NODE *n_diff (TREE *tree, size_t n, int *code_error)
{
    my_assert (tree->root != NULL, ERR_PTR);

    for (size_t i = 0; i < n; i++)
    {
        tree->root = diff (tree->root, code_error);

        tree->root = tree_simplific (tree->root, code_error);

        $$(NULL);

        set_parent (tree->root, NULL);
    }

    return tree->root;
}

NODE *diff (NODE *node, int *code_error)
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
            return NUM_(1, NULL);
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
                case (DEG):
                {
                    if (L_TYPE == NUM && R_TYPE == VAR)
                    {
                        return MUL_(LN_(COPY_L), MUL_(DEG_(COPY_L, COPY_R), DIF_R));
                    }

                    return MUL_(MUL_(COPY_R, DEG_(COPY_L, SUB_(COPY_R, NUM_(1, NULL)))), DIF_L);
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
                    return MUL_(MUL_(NUM_(0.5, NULL), DEG_(COPY_R, NUM_(-0.5, NULL))), DIF_R);
                }
                case (LN):
                {
                    return MUL_(DIV_(NUM_(1, NULL), COPY_R), DIF_R);
                }
                case (OP_NO): {}
                case (OP_SEP): {}
                case (OPEN_BRACKET): {}
                case (CLOSE_BRACKET): {}
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
            node = create_node_num (value, NULL, NULL, node->parent, code_error);
        }
        else
        {
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    IF_NUM (L_TYPE, {if (compare (L_VALUE, 0) == 0) {R_RE_HANGING;}})
                }
                case (SUB):
                {
                    IF_NUM (R_TYPE, {if (compare (R_VALUE, 0) == 0) {L_RE_HANGING;}})
                    break;
                }
                case (MUL):
                {
                    IF_NUM (L_TYPE,
                            if (compare (L_VALUE, 0) == 0)
                            {
                                L_RE_HANGING;
                            }
                            else if (!compare (L_VALUE, 1))
                            {
                                R_RE_HANGING;
                            })
                    IF_NUM (R_TYPE,
                            if (compare (R_VALUE, 0) == 0)
                            {
                                R_RE_HANGING;
                            }
                            else if (compare (R_VALUE, 1) == 0)
                            {
                                L_RE_HANGING;
                            })
                    break;
                }
                case (DIV):
                {
                    IF_NUM (L_TYPE,
                            if (compare (L_VALUE, 0) == 0)
                            {
                                L_RE_HANGING;
                            })
                    IF_NUM (R_TYPE,
                            if (compare (R_VALUE, 1) == 0)
                            {
                                R_RE_HANGING;
                            })
                    break;
                }
                case (DEG):
                {
                    IF_NUM (L_TYPE,
                            if (compare (L_VALUE, 0) == 0 || compare (L_VALUE, 1) == 0)
                            {
                                L_RE_HANGING;
                            })
                    IF_NUM (R_TYPE,
                            if (compare (R_VALUE, 0) == 0)
                            {
                                R_VALUE = 1;
                                R_RE_HANGING;
                            }
                            else if (compare (R_VALUE, 1) == 0)
                            {
                                L_RE_HANGING;
                            })
                    break;
                }
                case (OP_NO): {}
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
    }

    $$(NULL);

    return node;
}

NODE *hanging_tree(NODE *node, NODE *hanging_node, NODE *parent, int *code_error)
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

    $$(NULL);

    return hanging_node;
}

ELEMENT eval_tree (NODE *node, ELEMENT var_value, int *code_error)
{
    IS_NODE_PTR_NULL (ERR_NO);
    $$(*code_error);

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
                              eval_tree (node->left, var_value, code_error), 
                              code_error);
        }
        case (DEF_TYPE): {}
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
            if (!compare (second_value, 0))
            {
                return DEF_EVAL (/);
            }
            else
            {
                *code_error |= ERR_DIV_ZERO;
            }
        }
        case (DEG):
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