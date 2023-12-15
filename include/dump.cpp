#include "tree.h"

#ifdef DEBUG_TREE

#define DEF_CMD(num, name, name_tex) name_tex,

static const char *NAME_OP_TEX[] = {
    #include "comand.h"
};

#undef DEF_CMD

static const size_t STANDARD_SIZE_EXPR_TEX = 10;

static void print_tree_dump (NODE *node, FILE *stream);
static int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip);
static void print_replace (NODE *node, FILE *fp_tex, int *code_error);
static void get_node_replace (NODE *node, NODE **node_replace, size_t *pos_replace, int *code_error);
static size_t get_n_replace (NODE *node);

#define DUMP_LOG(str) fprintf (fp_err, str "\n");
#define DUMP_LOG_PARAM(str, ...) fprintf (fp_err, str "\n", __VA_ARGS__);

void tree_dump_text (TREE *tree, const int *code_error,
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

        my_strerr (*code_error, fp_err);

        if (tree == NULL)
        {
            DUMP_LOG_PARAM ("tree[NULL] \"tree\" called from %s(%d) %s", file_err, line_err, func_err);
        }
        else
        {
            DUMP_LOG_PARAM ("tree[%p] \"tree\" called from %s(%d) %s", tree, file_err, line_err, func_err);
            DUMP_LOG ("{");
            DUMP_LOG_PARAM ("\tis_init = %d", tree->is_init);
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
    IS_NODE_PTR_NULL ();

    switch (node->type)
    {
        case (NUM):
        {
            fprintf (stream, "\t\t*node[%p] = %lg;\n", node, node->data.value);
            break;
        }
        case (OP):
        {
            fprintf (stream, "\t\t*node[%p] = %s;\n", node, NAME_OP_TEX[node->data.types_op]);
            break;
        }
        case (VAR):
        {
            fprintf (stream, "\t\t*node[%p] = %s;\n", node, node->data.var);
            break;
        }
        case (DEF_TYPE): {}
        default:
        {
            break;
        }
    }

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

    const char comand[] = "dot -Tsvg include/dump.dot -o dot.svg";

    system (comand);

    int code_error = 0;

    tree_dump_html (tree, &code_error);

    my_strerr (code_error, stdout);
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
    IS_NODE_PTR_NULL (ip - 1);

    const char *color = BLACK_COLOR;

    switch (node->type)
    {
        case (NUM):
        {
            color = BLUE_COLOR;
            DUMP_DOT ("%lg", node->data.value);
            break;
        }
        case (OP):
        {
            color = PURPLE_COLOR; 
            DUMP_DOT ("%s", NAME_OP_TEX[node->data.types_op]);
            break;
        }
        case (VAR):
        {
            color = LIGHT_GREEN_COLOR; 
            DUMP_DOT ("%s", node->data.var);
            break;
        }
        case (DEF_TYPE): {}
        default:
        {
            break;
        }
    }

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
    FOPEN_ (fp_html_dot, tree->info.fp_name_html, "w+");
    
    size_t size_dot = get_file_size (fp_dot, code_error);

    if (*code_error != ERR_NO)
    {
        return;
    }

    char *data_dot = (char *) calloc (size_dot, sizeof (char));
    my_assert (data_dot != NULL, ERR_MEM)

    fread (data_dot, sizeof (char), size_dot, fp_dot);

    fprintf (fp_html_dot, "%s", data_dot);
    
    FCLOSE_ (fp_dot);
    FCLOSE_ (fp_html_dot);

    free (data_dot);
}

#define PRINT_TEX(str) fprintf (fp_tex, str);
#define PRINT_TEX_PARAM(str, ...) fprintf (fp_tex, str, __VA_ARGS__);

void print_tex_tree (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    FOPEN_ (fp_tex, tree->info.fp_tex_name, "w+");

    if (fp_tex != NULL)
    {
        PRINT_TEX ("\\documentclass[12pt, letterpaper]{article}\n"
                         "\\title {Deriving the derivative of a given expression}\n"
                         "\\author{Pavlov Matvey}\n"
                         "\\date{\\today}\n"
                         "\\begin{document}\n"
                         "\\maketitle\n");

        if (tree->root->tree_size > STANDARD_SIZE_EXPR_TEX)
        {
            print_replace (tree->root, fp_tex, code_error);
        }
        else
        {
            PRINT_TEX ("\\[f(x)=");
            print_tex_node (tree->root, fp_tex, NULL, 0, code_error);
            $$ ();
            PRINT_TEX ("\\]\n");
        }

        PRINT_TEX ("\\end{document}");
    }

    FCLOSE_ (fp_tex);
}

void print_tex_node (NODE *node, FILE *fp_tex, NODE **node_replace, size_t *pos_replace, int *code_error)
{
    my_assert (fp_tex != NULL, ERR_PTR);
    IS_NODE_PTR_NULL ();

    if (node_replace != NULL)
    {
        if (node_replace[*pos_replace] == node)
        {
            char name_replace = (char) (65 + (int) (*pos_replace));
            PRINT_TEX_PARAM ("%c", name_replace);
            *pos_replace += 1;

            return;
        }
    }

    switch (node->type)
    {
        case (NUM):
        {
            print_num (node, fp_tex, code_error);
            $$ ();

            break;
        }
        case (VAR):
        {
            PRINT_TEX_PARAM ("%s", node->data.var);
            break;
        }
        case (OP):
        {
            if (node->data.types_op == DIV)
            {
                print_tex_div (node, fp_tex, node_replace, pos_replace, code_error);
                $$ ();
            }
            else
            {
                bool is_bracket = false;

                if (node->data.types_op < OP_SEP)
                {
                    is_bracket = print_left_node_bracket (node, fp_tex, code_error);
                    $$ ();
                    
                    print_tex_node (node->left, fp_tex, node_replace, pos_replace, code_error);
                    $$ ();

                    PRINT_CLOSE_BRACKET (fp_tex);
                }

                print_tex_operator (node, fp_tex, code_error);
                
                is_bracket = print_right_node_bracket (node, fp_tex, code_error);
                $$ ();

                print_tex_node (node->right, fp_tex, node_replace, pos_replace, code_error);
                $$ ();

                PRINT_CLOSE_BRACKET (fp_tex);
            }

            break;
        }
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }
}

void print_tex_div (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    PRINT_TEX ("\\frac{");
    print_tex_node (node, fp_tex, node_replace, pos_replace, code_error);
    PRINT_TEX ("}{");
    print_tex_node (node, fp_tex, node_replace, pos_replace, code_error);
    PRINT_TEX ("}");
}

void print_tex_operator (NODE *node, FILE *fp_tex, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    PRINT_TEX_PARAM ("%s", NAME_OP_TEX[node->data.types_op]);
}

void print_replace (NODE *node, FILE *fp_tex, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    size_t n_replace = get_n_replace (node);

    NODE **node_replace = (NODE **) calloc (n_replace, sizeof (NODE *));
    my_assert (node_replace != NULL, ERR_MEM);

    size_t pos = 0;
    get_node_replace (node, node_replace, &pos, code_error);

    pos = 0;
    PRINT_TEX ("\\[f(x)=");
    print_tex_node (node, fp_tex, node_replace, &pos, code_error);
    PRINT_TEX ("\\]\n");

    for (size_t i = 0; i < n_replace; i++)
    {
        char name_replace = (char) (65 + (int) i);
        PRINT_TEX_PARAM ("\\[%c=", name_replace);
        print_tex_node (node_replace[i], fp_tex, NULL, 0, code_error);
        PRINT_TEX ("\\]\n");
    }

    free (node_replace);
}

void get_node_replace (NODE *node, NODE **node_replace, size_t *pos_replace, int *code_error)
{
    IS_NODE_PTR_NULL ();

    if (node->tree_size > STANDARD_SIZE_EXPR_TEX && 
        node->left->tree_size < STANDARD_SIZE_EXPR_TEX &&
        node->right->tree_size < STANDARD_SIZE_EXPR_TEX)
    {
        node_replace[*pos_replace] = node;
        *pos_replace += 1;

        return;
    }

    get_node_replace (node->left, node_replace, pos_replace, code_error);
    get_node_replace (node->right, node_replace, pos_replace, code_error);
}

size_t get_n_replace (NODE *node)
{
    IS_NODE_PTR_NULL (0);

    size_t n_replace = 0;

    if (node->tree_size > STANDARD_SIZE_EXPR_TEX && 
        node->left->tree_size < STANDARD_SIZE_EXPR_TEX &&
        node->right->tree_size < STANDARD_SIZE_EXPR_TEX)
    {
        return 1;
    }

    n_replace += get_n_replace (node->left);
    n_replace += get_n_replace (node->right);

    return n_replace;
}

#undef PRINT_TEX
#undef PRINT_TEX_PARAM

#endif