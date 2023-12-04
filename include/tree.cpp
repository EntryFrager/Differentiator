#include "tree.h"

#define DEF_CMD(num, name, ...) name,

const char *NAME_OP[] = {
    #include "comand.h"
};

#undef DEF_CMD

int create_tree (TREE *tree, DATA *data, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (tree->root = create_node (0, NULL, DEF_TYPE, NULL, NULL, NULL, code_error), *code_error);

    tree->init_status = INIT;

    tree->info.fp_name_base = "include/tree.txt";

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = "include/file_err_tree.txt";
    tree->info.fp_dot_name       = "include/dump.dot";
    tree->info.fp_name_html      = "include/dot.html";
    tree->info.fp_image          = "dot.svg";

    tree->info.fp_html_dot = fopen (tree->info.fp_name_html, "w+");
    my_assert (tree->info.fp_html_dot != NULL, ERR_FOPEN);
#endif

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

NODE *create_node (ELEMENT value, char *var, int type, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    NODE *node = (NODE *) calloc (1, sizeof (NODE));
    my_assert (node != NULL, ERR_MEM);

    node->data = (DATA *) calloc (1, sizeof (DATA));
    my_assert (node->data != NULL, ERR_MEM);

    node->data->value = value;
    node->data->var = var;
    node->type   = type;
    node->left   = left;
    node->right  = right;
    node->parent = parent;

    return node;
}

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

    tree->root = split_node (tree, NULL, NULL, code_error);

    assert_tree (tree, ERR_TREE);

    return ERR_NO;
}

NODE *split_node (TREE *tree, NODE *node, NODE *parent, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (node = create_node (0, NULL, DEF_TYPE, NULL, NULL, parent, code_error), NULL);

    if (*(++tree->info.buf) == '(')
    {
        CHECK_ERROR_RETURN (node->left = split_node (tree, node->left, node, code_error), NULL);
    }

    CHECK_ERROR_RETURN (read_str (tree, node, code_error), NULL);

    char temp_val = *tree->info.buf;    

    if (node->type == VAR)
    {
        *tree->info.buf = '\0';
    }

    if (temp_val == '(')
    {
        CHECK_ERROR_RETURN (node->right = split_node (tree, node->right, node, code_error), NULL);
    }

    tree->info.buf++;

    return node;
}

#define DEF_CMD(num, name, code)                                                              \
    if (strncmp (tree->info.buf, name, sizeof (name) - 1) == 0) {node->data->value = num; {code}} \
    else

void read_str (TREE *tree, NODE *node, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    if (isdigit (*tree->info.buf) != 0)
    {
        char **end_ptr = &tree->info.buf;
        node->data->value = strtod (tree->info.buf, end_ptr);

        node->type = NUM;
        tree->info.buf = *end_ptr;
    }
    else
    {
        #include "comand.h"

        {
            node->data->var = tree->info.buf;
            node->type = VAR;
        }
        
        while (*tree->info.buf != '(' && *tree->info.buf != ')')
        {
            tree->info.buf++;
        }
    }
}

#undef DEF_CMD

int add_node (NODE *node, ELEMENT value, char *var, int type, const bool side, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    if (side == LEFT)
    {
        CHECK_ERROR_RETURN (NODE *new_node = create_node (value, var, type, node->left, NULL, node, code_error), 
                            *code_error);
        node->left = new_node;
    }
    else
    {
        CHECK_ERROR_RETURN (NODE *new_node = create_node (value, var, type, NULL, node->right, node, code_error),
                            *code_error);
        node->right = new_node;
    }

    return ERR_NO;
}

int delete_node (NODE *node, int *code_error)
{
    if (!node)
    {
        return ERR_NO;
    }

    if (node->parent != NULL)
    {
        if (node->parent->left == node)
        {
            node->parent->left = NULL;
        }
        else if (node->parent->right == node)
        {
            node->parent->right = NULL;
        }
    }

    CHECK_ERROR_RETURN (delete_node (node->left, code_error), *code_error);
    
    CHECK_ERROR_RETURN (delete_node (node->right, code_error), *code_error);

    free(node->data);
    node->data = NULL;

    free (node);

    return ERR_NO;
}

NODE *copy_tree (NODE *node, NODE *parent, int *code_error)
{
    if (!node)
    {
        return NULL;
    }

    CHECK_ERROR_RETURN (NODE *copy_node  = create_node (node->data->value, node->data->var, node->type, NULL, NULL, parent, code_error), NULL);

    CHECK_ERROR_RETURN (copy_node->left  = copy_tree (node->left, copy_node, code_error), NULL);

    CHECK_ERROR_RETURN (copy_node->right = copy_tree (node->right, copy_node, code_error), NULL);

    return copy_node;
}

int print_tree (NODE *node, FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    if (!node)
    {
        return ERR_NO;
    }

    fprintf (stream, "(");

    CHECK_ERROR_RETURN (print_tree (node->left, stream, code_error), *code_error);

    if (node->type == NUM)
    {
        fprintf (stream, "%lf", node->data->value);
    }
    else if (node->type == VAR)
    {
        fprintf (stream, "%s", node->data->var);
    }
    else
    {
        fprintf (stream, "%s", NAME_OP[(int) node->data->value]);
    }

    CHECK_ERROR_RETURN (print_tree (node->right, stream, code_error), *code_error);

    fprintf (stream, ")");

    return ERR_NO;
}

int destroy_tree (TREE *tree, int *code_error)
{
    assert_tree (tree, ERR_NO);
    
    CHECK_ERROR_RETURN (delete_node (tree->root, code_error), *code_error);
    tree->root = NULL;

    tree->init_status = INIT_NOT;

    tree->info.fp_name_base = NULL;

    free (tree->info.buf);
    tree->info.buf = NULL;

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = NULL;
    tree->info.fp_name_html = NULL;
    tree->info.fp_dot_name = NULL;

    my_assert (fclose (tree->info.fp_html_dot) == 0, ERR_FCLOSE);
#endif

    return ERR_NO;
}

#ifdef DEBUG_TREE

static void print_tree_dump (NODE *node, FILE *stream);

#define VERIF_EXPR(expr, name_error) if (!(expr)) {code_error |= name_error;}

int tree_verificator (TREE *tree)
{
    int code_error = 0;

    VERIF_EXPR (tree != NULL, TREE_ERR_PTR)

    VERIF_EXPR (tree->init_status == INIT, TREE_INIT)

    VERIF_EXPR (tree->root != NULL, TREE_ERR_ROOT_PTR)

    return code_error;
}

int node_verificator (NODE *node)
{
    int code_error = 0;

    if (!node)
    {
        return ERR_NO;
    }

    if (node->parent != NULL)
    {
        VERIF_EXPR (node->parent->left == node || node->parent->right == node, NODE_ERR)
    }

    VERIF_EXPR (node->data != NULL, NODE_DATA_ERR_PTR)

    code_error |= node_verificator (node->left);

    code_error |= node_verificator (node->right);

    return code_error;
}

#undef VERIF_EXPR

#define DUMP_LOG(str) fprintf (fp_err, str "\n");
#define DUMP_LOG_PARAM(str, ...) fprintf (fp_err, str "\n", __VA_ARGS__);

void tree_dump_text (TREE *tree, const int code_error,
                     const char *file_err, const char *func_err,
                     const int line_err)
{
    FILE *fp_err = fopen (tree->info.fp_dump_text_name, "a");

    if (fp_err == NULL)
    {
        my_strerr (ERR_FOPEN, stderr);
    }
    else
    {
        DUMP_LOG ("--------------------------------------------------------------------------------------------------------");

        my_strerr (code_error, fp_err);

        if (tree == NULL)
        {
            DUMP_LOG_PARAM ("tree[NULL] \"tree\" called from %s(%d) %s", file_err, line_err, func_err);
        }
        else
        {
            DUMP_LOG_PARAM ("tree[%p] \"tree\" called from %s(%d) %s", tree, file_err, line_err, func_err);
            DUMP_LOG ("{");
            DUMP_LOG_PARAM ("\tinit_status = %d", tree->init_status);
            DUMP_LOG ("\t{");

            if (tree->root != NULL)
            {
                print_tree_dump (tree->root, fp_err);
            }
            else
            {
                DUMP_LOG ("\t\t*root[NULL]");
            }

            DUMP_LOG ("\t}");
            DUMP_LOG ("}");
        }

        DUMP_LOG ("--------------------------------------------------------------------------------------------------------\n\n");
    }

    if (fclose (fp_err) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }
}

#undef DUMP_LOG
#undef DUMP_LOG_PARAM

void print_tree_dump (NODE *node, FILE *stream)
{
    if (node->type == NUM)
    {
        fprintf (stream, "\t\t*node[%p] = %lf;\n", node, node->data->value);
    }
    else if (node->type == VAR)
    {
        fprintf (stream, "\t\t*node[%p] = %s;\n", node, node->data->var);
    }
    else
    {
        fprintf (stream, "\t\t*node[%p] = %s;\n", node, NAME_OP[(int) node->data->value]);
    }

    if (node->left != NULL)
    {
        print_tree_dump (node->left, stream);
    }
    if (node->right != NULL)
    {
        print_tree_dump (node->right, stream);
    }
}

#define DUMP_DOT(str) fprintf (fp_dot, str "\n");
#define DUMP_DOT_PARAM(str, ...) fprintf (fp_dot, str "\n", __VA_ARGS__);

void tree_dump_graph_viz (TREE *tree, const char *file_err, 
                          const char *func_err, const int line_err)
{
    FILE *fp_dot = fopen (tree->info.fp_dot_name, "w+");

    if (fp_dot == NULL)
    {
        my_strerr (ERR_FOPEN, stderr);
    }
    else
    {
        if (tree != NULL)
        {
            DUMP_DOT ("digraph List {");
            DUMP_DOT ("\trankdir = HR;");
            DUMP_DOT ("\tbgcolor = " BACK_GROUND_COLOR ";");

            if (tree->root != NULL)
            {
                create_node_dot (tree->root, fp_dot, -1, 0, RED_COLOR);
            }

            DUMP_DOT_PARAM ("\tlabel = \"tree_dump from function %s, Tree/%s:%d\";}\n", func_err, file_err, line_err);
        }
    }

    if (fclose (fp_dot) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }

    char *command = "dot -Tsvg include/dump.dot -o dot.svg";

    system (command);

    int code_error = 0;

    CHECK_ERROR_PRINT (tree_dump_html (tree, &code_error));
}

#undef DUMP_DOT
#undef DUMP_DOT_PARAM

#undef DEBUG_TREE

#ifdef DEBUG_TREE
    #define DUMP_DOT(specifier, value) fprintf (stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, " \
                 "label = \"{idx: %p | value: " specifier " | left: %p | right: %p | parent: %p}\"];\n",              \
                ip, color, node, value, node->left, node->right, node->parent);
#else
    #define DUMP_DOT(specifier, value) fprintf (stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, " \
                 "label = \"{" specifier "}\"];\n", ip, color, value);
#endif

int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip, char *color)
{
    if (!node)
    {
        return ip - 1;
    }

    if (node->type == NUM)
    {
        DUMP_DOT ("%lf", node->data->value);
    }
    else if (node->type == VAR)
    {
        DUMP_DOT ("%s", node->data->var);
    }
    else
    {
        DUMP_DOT ("%s", NAME_OP[(int) node->data->value]);
    }

    if (ip > 0)
    {
        fprintf (stream, "\tnode%d -> node%d [color = %s]\n", ip_parent, ip, color);
    }

    ip_parent = ip;

    ip = create_node_dot (node->left, stream, ip_parent, ip + 1, BLUE_COLOR);
    ip = create_node_dot (node->right, stream, ip_parent, ip + 1, LIGHT_GREEN_COLOR);

    return ip;
}

void tree_dump_html (TREE *tree, int *code_error)
{
    FILE *fp_dot = fopen (tree->info.fp_image, "r");
    my_assert (fp_dot != NULL, ERR_FOPEN);
    
    size_t size_dot = get_file_size (fp_dot, code_error);
    
    if (*code_error != ERR_NO)
    {
        return;
    }

    char *data_dot = (char *) calloc (size_dot, sizeof (char));
    my_assert (data_dot != NULL, ERR_MEM)

    fread (data_dot, sizeof (char), size_dot, fp_dot);

    fprintf (tree->info.fp_html_dot, "%s", data_dot);
    my_assert (fclose (fp_dot) == 0, ERR_FCLOSE);

    free (data_dot);
}

#endif