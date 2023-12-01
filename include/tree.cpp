#include "tree.h"

int create_tree (TREE *tree, const char *value)
{
    my_assert (tree != NULL);

    tree->root = create_node (value, NULL, NULL, NULL);

    tree->init_status = INIT;

    tree->info.fp_name_base = "include/tree.txt";

#ifdef DEBUG
    tree->info.fp_dump_text_name = "include/file_err_tree.txt";
    tree->info.fp_dot_name       = "include/dump.dot";
    tree->info.fp_name_html      = "include/dot.html";
    tree->info.fp_image          = "dot.svg";

    tree->info.fp_html_dot = fopen (tree->info.fp_name_html, "w+");

    if (tree->info.fp_html_dot == NULL)
    {
        my_strerr (ERR_FOPEN, stderr);
    }
#endif

    assert_tree (tree);

    return ERR_NO;
}

NODE *create_node (const char *value, NODE *left, NODE *right, NODE *parent)
{
    NODE *node = (NODE *) calloc (1, sizeof (NODE));
    my_assert (node != NULL);

    node->value  = value;
    node->left   = left;
    node->right  = right;
    node->parent = parent;

    return node;
}

int input_base (TREE *tree)
{
    my_assert (tree != NULL);

    tree->info.fp_base = fopen (tree->info.fp_name_base, "r + b");

    if (tree->info.fp_base == NULL)
    {
        return ERR_FOPEN;
    }

    tree->info.size_file = get_file_size (tree->info.fp_base);

    tree->info.buf = (char *) calloc (tree->info.size_file + 1, sizeof (char));
    my_assert (tree->info.buf != NULL);

    size_t read_size = fread (tree->info.buf, sizeof (char), tree->info.size_file, tree->info.fp_base);

    if (read_size != tree->info.size_file)
    {
        return ERR_FREAD;
    }

    *(tree->info.buf + tree->info.size_file) = '\0';

    if (fclose (tree->info.fp_base) != 0)
    {
        return ERR_FCLOSE;
    }

    tree->root = split_node (tree, NULL, NULL);

    assert_tree (tree);

    return ERR_NO;
}

NODE *split_node (TREE *tree, NODE *node, NODE *parent)
{
    my_assert (tree != NULL);

    node = create_node (NULL, NULL, NULL, parent);

    if (*(++tree->info.buf) == '(')
    {
        node->left = split_node (tree, node->left, node);
    }
    else if (strncmp (tree->info.buf, "nil", 3) == 0)
    {
        tree->info.buf += 3;
    }

    while (*(tree->info.buf++) != '\"')
    {
    }

    node->value = tree->info.buf;
    
    while (*(tree->info.buf++) != '\"')
    {
    }
    
    *(tree->info.buf - 1) = '\0';

    if (*(++tree->info.buf) == '(')
    {
        node->right = split_node (tree, node->right, node);
    }
    else if (strncmp (tree->info.buf, "nil", 3) == 0)
    {
        tree->info.buf += 3;
    }

    tree->info.buf++;

    return node;
}

int add_node (NODE *node, const char *value, const bool side)
{
    my_assert (node != NULL);
    my_assert (value != NULL);

    if (side == LEFT)
    {
        NODE *new_node = create_node (value, node->left, NULL, node);
        node->left = new_node;
    }
    else
    {
        NODE *new_node = create_node (value, NULL, node->right, node);
        node->right = new_node;
    }

    return ERR_NO;
}

int delete_node (NODE *node)
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

    CHECK_ERROR_RETURN (delete_node (node->left));
    
    CHECK_ERROR_RETURN (delete_node (node->right));

    free (node);
    
    return ERR_NO;
}

int print_tree (NODE *node, FILE *stream)
{
    my_assert (stream != NULL);

    if (!node)
    {
        fprintf (stream, "nil");
        return ERR_NO;
    }

    fprintf (stream, "(");

    print_tree (node->left, stream);

    fprintf (stream, " \"%s\" ", node->value);

    print_tree (node->right, stream);

    fprintf (stream, ")");

    return ERR_NO;
}

int destroy_tree (TREE *tree)
{
    assert_tree (tree);
    
    CHECK_ERROR_RETURN (delete_node (tree->root));
    tree->root = NULL;

    tree->init_status = INIT_NOT;

    tree->info.fp_name_base = NULL;

    free (tree->info.buf);
    tree->info.buf = NULL;

#ifdef DEBUG
    tree->info.fp_dump_text_name = NULL;
    tree->info.fp_name_html = NULL;
    tree->info.fp_dot_name = NULL;

    if (fclose (tree->info.fp_html_dot) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }
#endif

    return ERR_NO;
}

#ifdef DEBUG

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
    fprintf (stream, "\t\t*node[%p] = %s;\n", node, node->value);

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
                create_node (tree->root, fp_dot, -1, 0, RED_COLOR);
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

    tree_dump_html (tree);
}

#undef DUMP_DOT
#undef DUMP_DOT_PARAM

#undef DEBUG

int create_node (NODE *node, FILE *stream, int ip_parent, int ip, char *color)
{
    if (!node)
    {
        return ip - 1;
    }

    fprintf (stream, "\tnode%d [shape = Mrecord, style = filled, fillcolor = %s, "
             "label = \"{"
#ifdef DEBUG 
             "idx: %p | value: %s | left: %p | right: %p | parent: %p}\"];\n",
             ip, color, node, node->value, node->left, node->right, node->parent);
#else
             "%s}\"];\n", ip, color, node->value);
#endif

    if (ip > 0)
    {
        fprintf (stream, "\tnode%d -> node%d [color = %s]\n", ip_parent, ip, color);
    }

    ip_parent = ip;

    ip = create_node (node->left, stream, ip_parent, ip + 1, BLUE_COLOR);
    ip = create_node (node->right, stream, ip_parent, ip + 1, LIGHT_GREEN_COLOR);

    return ip;
}

void tree_dump_html (TREE *tree)
{
    FILE *fp_dot = fopen (tree->info.fp_image, "r");

    if (fp_dot == NULL)
    {
        my_strerr (ERR_FOPEN, stderr);
    }

    size_t size_dot = get_file_size (fp_dot);

    char *data_dot = (char *) calloc (size_dot, sizeof (char));

    fread (data_dot, sizeof (char), size_dot, fp_dot);

    fprintf (tree->info.fp_html_dot, "%s", data_dot);

    if (fclose (fp_dot) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }

    free (data_dot);
}

#endif