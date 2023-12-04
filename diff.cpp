#include "diff.h"

#define NUM_(value) create_node (value, NULL, NUM, NULL, NULL, NULL, code_error)

#define ADD_(node_1, node_2) create_node (ADD, NULL, OP, node_1, node_2, NULL, code_error)

#define SUB_(node_1, node_2) create_node (SUB, NULL, OP, node_1, node_2, NULL, code_error)

#define MUL_(node_1, node_2) create_node (MUL, NULL, OP, node_1, node_2, NULL, code_error)

#define DIV_(node_1, node_2) create_node (DIV, NULL, OP, node_1, node_2, NULL, code_error)

#define DEG_(node_1, deg) create_node (DEG, NULL, OP, node_1, deg, NULL, code_error)

#define SIN_(node) create_node (SIN, NULL, OP, node, NUM_(1), NULL, code_error)

#define COS_(node) create_node (COS, NULL, OP, node, NUM_(1), NULL, code_error)

#define DIF_L diff (node->left, code_error)

#define DIF_R diff (node->right, code_error)

#define COPY_L copy_tree (node->left, NULL, code_error)

#define COPY_R copy_tree (node->right, NULL, code_error)

NODE *diff (NODE *node, int *code_error)
{
    if (node->type == NUM)
    {
        return NUM_(0);
    }
    else if (node->type == VAR)
    {
        return NUM_(1);
    }

    switch ((int) node->data->value)
    {
        case (ADD):
        {
            return ADD_(DIF_L,
                        DIF_R);
        }
        case (SUB):
        {
            return SUB_(DIF_L,
                        DIF_R);
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
            return MUL_(MUL_(COPY_R, DEG_(COPY_L, SUB_(COPY_R, NUM_(1)))), DIF_L);
        }
        case (SIN):
        {
            return MUL_(COS_(COPY_L), DIF_L);
        }
        case (COS):
        {
            return MUL_(SIN_(COPY_L), MUL_(NUM_(-1), DIF_L));
        }
        case (SQRT):
        {
            return MUL_(MUL_(NUM_(0.5), DEG_(COPY_L, NUM_(-0.5))), DIF_L);
        }
        default:
        {
            printf ("sosi xyi;)");
            break;
        }
    }

    return node;
}

NODE *set_parent (NODE *node, NODE *parent)
{
    if (!node)
    {
        return NULL;
    }

    node->parent = parent;

    set_parent (node->left, node);

    set_parent (node->right, node);

    return node;
}

/*NODE *simplific_tree (NODE *node, int *code_error)
{
    if (!node)
    {
        return NULL;
    }

    if (node->type == OP)
    {
        if (node->left->data->value == NUM && node->right->data->value == NUM)
        {
            node->data->value = eval (node->type, node->left->data->value, node->right->data->value);
            node->type = NUM;
        }
        switch ((int) node->data->value)
        {
            case (ADD):
            {
                if (node->left->type == NUM)
                {
                    if (node->left->data->value == 0)
                    {
                        node = hanging_tree (node, node->right, node->parent, code_error);
                    }
                }
                else if (node->right->type == NUM)
                {
                    if (node->right->data->value == 0)
                    {
                        node = hanging_tree (node, node->left, node->parent, code_error);
                    }
                }
                break;
            }
            case (SUB):
            {
                if (node->left->type == NUM)
                {
                    if (node->left->data->value == 0)
                    {
                        node = hanging_tree (node, node->right, node->parent, code_error);
                    }
                }
                else if (node->right->type == NUM)
                {
                    if (node->right->data->value == 0)
                    {
                        node = hanging_tree (node, node->left, node->parent, code_error);
                    }
                }
                break;
            }
            case (MUL):
            {
                if (node->left->type == NUM)
                {
                    if (node->left->data->value == 0)
                    {
                        node->type = NUM;
                        node->data->value = 0;
                    }
                    else if (node->left->data->value == 1)
                    {
                        node->type = NUM;
                        node->data->value = node->right->data->value;
                    }
                }
                break;
            }
        }
    }

    return node;
}

NODE *hanging_tree(NODE *node, NODE *turn_off_node, NODE *parent, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (turn_off_node != NULL, ERR_PTR);

    if (parent->left == node)
    {
        parent->left = turn_off_node;
    }
    else if (parent->right == node)
    {
        parent->right = turn_off_node;
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
}

double eval (int op, double first_value, double second_value)
{
    switch (op)
    {
        case (ADD):
        {
            return first_value + second_value;
        }
        case (SUB):
        {
            return first_value - second_value;
        }
        case (MUL):
        {
            return first_value * second_value;
        }
        case (DIV):
        {
            if (second_value != 0)
            {
                return first_value / second_value;
            }
        }
        case (DEG):
        {
            return pow (first_value, second_value);
        }
        case (SIN):
        {
            return sin (first_value);
        }
        case (COS):
        {
            return sin (first_value);
        }
        case (SQRT):
        {
            return pow (first_value, 0.5);
        }
        default:
        {
            printf ("sosidick\n");
            break;
        }
    }
}*/