#include "tree.h"

#define DEF_CMD(num, name, ...) name,

const char *NAME_OP[] = {
    #include "comand.h"
};

#undef DEF_CMD

int create_tree (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    CHECK_ERROR_RETURN (tree->root = create_node_num (0, NULL, NULL, NULL, code_error), *code_error);

    tree->init_status = INIT;

    tree->info.fp_name_expr = "include/tree.txt";

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

#define calloc_node()                                   \
    NODE *node = (NODE *) calloc (1, sizeof (NODE));    \
    my_assert (node != NULL, ERR_MEM);                  \
    node->left      = left;                             \
    node->right     = right;                            \
    node->parent    = parent;

NODE *create_node_num (ELEMENT value, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    calloc_node ()
    
    node->data.value = value;
    node->type      = NUM;

    return node;
}
NODE *create_node_op (op_command types_op, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    calloc_node ()

    node->data.types_op = types_op;
    node->type          = OP;

    return node;
}

NODE *create_node_var (char var, NODE *left, NODE *right, NODE *parent, int *code_error)
{
    calloc_node ()

    node->data.var = var;
    node->type     = VAR;

    return node;
}

int add_node (NODE *node, ELEMENT value, char var, op_command types_op, int type, const bool side, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);

    NODE *new_node = NULL;

    if (side == LEFT)
    {
        CHECK_TYPE (type,
                    {CHECK_ERROR_RETURN (new_node = create_node_var (var, node->left, NULL, node, code_error), 
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_num (value, node->left, NULL, node, code_error),
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_op (types_op, node->left, NULL, node, code_error),
                                        *code_error);});

        node->left = new_node;
    }
    else
    {
        CHECK_TYPE (type,
                    {CHECK_ERROR_RETURN (new_node = create_node_var (var, NULL, node->right, node, code_error), 
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_num (value, NULL, node->right, node, code_error),
                                        *code_error);},
                    {CHECK_ERROR_RETURN (new_node = create_node_op (types_op, NULL, node->right, node, code_error),
                                        *code_error);});

        node->right = new_node;
    }

    return ERR_NO;
}

NODE *set_parent (NODE *node, NODE *parent)
{
    CHECK_NODE_PTR (NULL);

    node->parent = parent;

    set_parent (node->left, node);
    set_parent (node->right, node);

    return node;
}

int delete_node (NODE *node, int *code_error)
{
    CHECK_NODE_PTR (ERR_NO);

    CHECK_ERROR_RETURN (delete_node (node->left, code_error), *code_error);
    
    CHECK_ERROR_RETURN (delete_node (node->right, code_error), *code_error);

    free (node);

    return ERR_NO;
}

NODE *copy_tree (NODE *node, NODE *parent, int *code_error)
{
    CHECK_NODE_PTR (NULL);

    NODE *copy_node = NULL;

    CHECK_TYPE (node->type,
        {CHECK_ERROR_RETURN (copy_node  = create_node_var (node->data.var, NULL, NULL, parent, code_error),
                            NULL);},
        {CHECK_ERROR_RETURN (copy_node = create_node_num (node->data.value, NULL, NULL, parent, code_error), 
                            NULL);},
        {CHECK_ERROR_RETURN (copy_node = create_node_op (node->data.types_op, NULL, NULL, parent, code_error), 
                            NULL);});


    CHECK_ERROR_RETURN (copy_node->left  = copy_tree (node->left, copy_node, code_error), NULL);
    CHECK_ERROR_RETURN (copy_node->right = copy_tree (node->right, copy_node, code_error), NULL);

    return copy_node;
}

int print_tree (NODE *node, FILE *stream, int *code_error)
{
    my_assert (stream != NULL, ERR_PTR);

    CHECK_NODE_PTR (ERR_NO);

    PRINT_BRACKET (node->left, "(");
    
    CHECK_ERROR_RETURN (print_tree (node->left, stream, code_error), *code_error);

    PRINT_BRACKET (node->left, ")");

    CHECK_TYPE (node->type,
                {fprintf (stream, "%c", node->data.var);},
                {fprintf (stream, "%lf", node->data.value);},
                {fprintf (stream, "%s", NAME_OP[node->data.types_op]);});

    PRINT_BRACKET (node->right, "(");
    
    CHECK_ERROR_RETURN (print_tree (node->right, stream, code_error), *code_error);

    PRINT_BRACKET (node->right, ")");

    return ERR_NO;
}

int destroy_tree (TREE *tree, int *code_error)
{
    assert_tree (tree, ERR_NO);
    
    CHECK_ERROR_RETURN (delete_node (tree->root, code_error), *code_error);
    tree->root = NULL;

    tree->init_status = INIT_NO;

    tree->info.fp_name_expr = NULL;
    tree->info.size_file = 0;

    free (tree->info.buf);
    tree->info.buf = NULL;

#ifdef DEBUG_TREE
    tree->info.fp_dump_text_name = NULL;
    tree->info.fp_dot_name       = NULL;
    tree->info.fp_name_html      = NULL;
    tree->info.fp_image          = NULL;

    FCLOSE_ (tree->info.fp_html_dot);
#endif

    return ERR_NO;
}

#ifdef DEBUG_TREE

static void print_tree_dump (NODE *node, FILE *stream);
static int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip);

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

    CHECK_NODE_PTR (ERR_NO);

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
    CHECK_NODE_PTR ();

    CHECK_TYPE (node->type, 
                {fprintf (stream, "\t\t*node[%p] = %c;\n", node, node->data.var);},
                {fprintf (stream, "\t\t*node[%p] = %lf;\n", node, node->data.value);},
                {fprintf (stream, "\t\t*node[%p] = %s;\n", node, NAME_OP[node->data.types_op]);})

    print_tree_dump (node->left, stream);
    print_tree_dump (node->right, stream);
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
                create_node_dot (tree->root, fp_dot, -1, 0);
            }

            DUMP_DOT_PARAM ("\tlabel = \"tree_dump from function %s, Tree/%s:%d\";}\n", func_err, file_err, line_err);
        }
    }

    if (fclose (fp_dot) != 0)
    {
        my_strerr (ERR_FCLOSE, stderr);
    }

    const char command[] = "dot -Tsvg include/dump.dot -o dot.svg";

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

int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip)
{
    CHECK_NODE_PTR (ip - 1);

    char *color = BLACK_COLOR;

    CHECK_TYPE (node->type, 
                {color = LIGHT_GREEN_COLOR; DUMP_DOT ("%c", node->data.var);},
                {color = BLUE_COLOR; DUMP_DOT ("%lf", node->data.value);},
                {color = PURPLE_COLOR; DUMP_DOT ("%s", NAME_OP[node->data.types_op]);});

    if (ip > 0)
    {
        fprintf (stream, "\tnode%d -> node%d [color = %s];\n", ip_parent, ip, color);
    }

    ip_parent = ip;

    ip = create_node_dot (node->left, stream, ip_parent, ip + 1);
    ip = create_node_dot (node->right, stream, ip_parent, ip + 1);

    return ip;
}

void tree_dump_html (TREE *tree, int *code_error)
{
    FOPEN_ (fp_dot, tree->info.fp_image, "r");
    
    size_t size_dot = get_file_size (fp_dot, code_error);

    if (*code_error != ERR_NO)
    {
        return;
    }

    char *data_dot = (char *) calloc (size_dot, sizeof (char));
    my_assert (data_dot != NULL, ERR_MEM)

    fread (data_dot, sizeof (char), size_dot, fp_dot);

    fprintf (tree->info.fp_html_dot, "%s", data_dot);
    
    FCLOSE_ (fp_dot);

    free (data_dot);
}

#endif