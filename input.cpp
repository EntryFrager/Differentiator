#include <stdio.h>
#include <stdlib.h>

#include "include/tree.h"

size_t p = 0;

#define syntax_assert(expr) if (!expr) {printf ("SYNTAX ERROR: expected \"" #expr "\""); abort();}

NODE *get_g (const char *str, int *code_error);
NODE *get_e (const char *str, int *code_error);
NODE *get_t (const char *str, int *code_error);
NODE *get_p (const char *str, int *code_error);
NODE *get_n (const char *str, int *code_error);

int input_base (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    tree->info.fp_base = fopen (tree->info.fp_name_base, "r + b");
    my_assert (tree->info.fp_base != NULL, ERR_FOPEN);

    CHECK_ERROR_RETURN (tree->info.size_file = get_file_size (tree->info.fp_base, code_error), *code_error);

    tree->info.buf = (char *) calloc (tree->info.size_file + 1, sizeof (char));
    my_assert (tree->info.buf != NULL, ERR_MEM);

    size_t read_size = fread (tree->info.buf, sizeof (char), tree->info.size_file, tree->info.fp_base);
    my_assert (read_size == tree->info.size_file, ERR_FREAD);

    *(tree->info.buf + tree->info.size_file) = '\0';

    FCLOSE_ (tree->info.fp_base);

    tree->root = get_g (tree->info.buf, code_error);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

NODE *get_g (const char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    const char *s = str;
    p = 0;

    NODE *node = get_e (str, code_error);

    syntax_assert (s[p] == '\0');

    return node;
}

NODE *get_e (const char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    NODE *node = get_t (str, code_error);

    while (str[p] == '+' || str[p] == '-')
    {
        char op = str[p];
        p++;
        NODE *node_temp = get_t (str, code_error);

        switch (op)
        {
            case ('+'):
            {
                node = create_node (ADD, NULL, OP, node, node_temp, NULL, code_error);
                break;
            }
            case ('-'):
            {
                node = create_node (ADD, NULL, OP, node, node_temp, NULL, code_error);
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

NODE *get_t (const char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    NODE *node = get_p (str, code_error);

    while (str[p] == '*' || str[p] == '/')
    {
        char op = str[p];
        p++;
        NODE *node_temp = get_p (str, code_error);

        switch (op)
        {
            case ('*'):
            {
                node = create_node (MUL, NULL, OP, node, node_temp, NULL, code_error);
                break;
            }
            case ('/'):
            {
                syntax_assert (node_temp->data->value != 0);
                node = create_node (MUL, NULL, OP, node, node_temp, NULL, code_error);
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

NODE *get_p (const char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (str[p] == '(')
    {
        p++;

        NODE *node = get_e (str, code_error);

        syntax_assert (str[p] == ')');

        p++;

        return node;
    }
    else
    {
        return get_n (str, code_error);
    }
}

NODE get_trig (const char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    if (strncmp (str, "sin", 3) == 0)
    {
        str += 3;  
    }
    else if (strncmp (str, "cos", 3) == 0)
    {
        str += 3;
    }
    else if (strncmp (str, "tg", 2) == 0)
    {
        str += 2;
    }
    else if (strncmp (str, "ctg", 3) == 0)
    {
        str += 3;
    }
}

NODE *get_n (const char *str, int *code_error)
{
    my_assert (str != NULL, ERR_PTR);

    int value = 0;

    while ('0' <= str[p] && str[p] <= '9')
    {
        value = value * 10 + str[p] - '0';
        p++;
    }

    return create_node (value, NULL, NUM, NULL, NULL, NULL, code_error);
}