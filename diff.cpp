#include "diff.h"

NODE *n_diff (NODE *node, size_t n, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    for (size_t i = 0; i < n; i++)
    {
        CHECK_ERROR_RETURN (diff (node, code_error), NULL);

        CHECK_ERROR_RETURN (node = tree_simplific (node, code_error), NULL);
    }

    return node;
}

NODE *diff (NODE *node, int *code_error)
{
    CHECK_TYPE (node->type,
                {return NUM_(1, NULL);},
                {return NUM_(0, NULL);},
                {switch (node->data.types_op)
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
                    default:
                    {
                        break;
                    }
                }})

    return node;
}

NODE *tree_simplific (NODE *node, int *code_error)
{
    CHECK_NODE_PTR (NULL);

    if (node->type != OP)
    {
        return node;
    }

    node->left  = tree_simplific (node->left, code_error);
    node->right = tree_simplific (node->right, code_error);

    if (node->type == OP && node->data.types_op != SIN &&
        node->data.types_op != COS && node->data.types_op != SQRT)
    {
        if (L_TYPE == NUM && R_TYPE == NUM)
        {
            CHECK_ERROR_RETURN (ELEMENT value = eval_node (node->data.types_op, L_VALUE, R_VALUE, code_error), NULL);
            node = create_node_num (value, NULL, NULL, node->parent, code_error);
        }
        else
        { 
            switch (node->data.types_op)
            {
                case (ADD):
                {
                    CHECK_NUM (L_TYPE, {if (is_zero (L_VALUE, code_error)) {R_RE_HANGING;}})
                }
                case (SUB):
                {
                    CHECK_NUM (R_TYPE, {if (is_zero (R_VALUE, code_error)) {L_RE_HANGING;}})
                    break;
                }
                case (MUL):
                {
                    CHECK_NUM (L_TYPE,
                            if (is_zero (L_VALUE, code_error))
                            {
                                L_RE_HANGING;
                            }
                            else if (!compare (L_VALUE, 1))
                            {
                                R_RE_HANGING;
                            })
                    CHECK_NUM (R_TYPE,
                            if (is_zero (R_VALUE, code_error))
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
                    CHECK_NUM (L_TYPE,
                            if (is_zero (L_VALUE, code_error))
                            {
                                L_RE_HANGING;
                            })
                    CHECK_NUM (R_TYPE,
                            if (compare (R_VALUE, 1) == 0)
                            {
                                R_RE_HANGING;
                            })
                    break;
                }
                case (DEG):
                {
                    CHECK_NUM (L_TYPE,
                            if (is_zero (L_VALUE, code_error) || compare (L_VALUE, 1) == 0)
                            {
                                L_RE_HANGING;
                            })
                    CHECK_NUM (R_TYPE,
                            if (is_zero (R_VALUE, code_error))
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
                default:
                {
                    break;
                }
            }
        }
    }

    return node;
}

NODE *hanging_tree(NODE *node, NODE *turn_off_node, NODE *parent, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (turn_off_node != NULL, ERR_PTR);

    if (parent != NULL)
    {
        if (parent->left == node)
        {
            parent->left = turn_off_node;
        }
        else if (parent->right == node)
        {
            parent->right = turn_off_node;
        }
    }

    turn_off_node->parent = parent;

    if (turn_off_node == node->left)
    { 
        delete_node (node->right, code_error);
    }
    else if (turn_off_node == node->right)
    {
        delete_node (node->left, code_error);
    }

    free (node);

    return turn_off_node;
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

    return 0;
}

#undef my_eval