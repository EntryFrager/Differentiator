#include "tree.h"
#include "../diff.h"

#ifdef DEBUG_TREE

#define DEF_CMD(num, name, name_tex) name_tex,

static const char *NAME_OP_TEX[] = {
    #include "comand.h"
};

#undef DEF_CMD

static const size_t STANDARD_SIZE_EXPR_TEX = 20;

static void print_tex_tree (TREE *tree, FILE *fp_tex, size_t n_diff, int *code_error);
static void print_tex_node (NODE *node, FILE *fp_tex, NODE **node_replace, size_t *pos_replace, int *code_error);
static void print_tex_operator (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error);
static void print_tex_def_op (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error);
static void print_tex_div (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error);
static void print_tree_dump (NODE *node, FILE *stream);
static int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip);
static void print_replace (NODE *node, FILE *fp_tex, size_t n_diff, int *code_error);
static void get_node_replace (NODE *node, NODE **node_replace, size_t *pos_replace, int *code_error);
static size_t get_n_replace (NODE *node);
static void print_tex_taylor (TREE *tree, FILE *fp_tex, int *code_error);

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

void print_tex (TREE *tree, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    FOPEN_ (fp_tex, tree->info.fp_tex_name, "w+");

    if (fp_tex != NULL)
    {
        PRINT_TEX ("\\documentclass[12pt, letterpaper]{article}\n"
                    "\\title {Study of the function, the value at a point and its Taylor expansion}\n"
                    "\\author{Pavlov Matvey}\n"
                    "\\date{\\today}\n"
                    "\\begin{document}\n"
                    "\\maketitle\n");

        PRINT_TEX ("Function:");
        print_tex_tree (tree, fp_tex, 0, code_error);

        PRINT_TEX_PARAM ("Function value at a point $%s=%lg$: \\[f(%lg)=%lg\\]\n", 
                         tree->info.var_diff, tree->info.value_point, tree->info.value_point,  
                         eval_tree (tree->root, tree->info.value_point, code_error));
        
        print_tex_taylor (tree, fp_tex, code_error);
        PRINT_TEX ("\\end{document}");
    }

    FCLOSE_ (fp_tex);
}

void print_tex_tree (TREE *tree, FILE *fp_tex, size_t n_diff, int *code_error)
{
    my_assert (tree != NULL, ERR_PTR);

    if (tree->root->tree_size > STANDARD_SIZE_EXPR_TEX)
    {
        print_replace (tree->root, fp_tex, n_diff, code_error);
        ERR_RET ();
    }
    else
    {
        PRINT_TEX_PARAM ("\\[f^{(%d)}(x)=", n_diff);
        print_tex_node (tree->root, fp_tex, NULL, 0, code_error);
        ERR_RET ();

        PRINT_TEX ("\\]\n");
    }
}

void print_tex_node (NODE *node, FILE *fp_tex, NODE **node_replace, size_t *pos_replace, int *code_error)
{
    my_assert (fp_tex != NULL, ERR_PTR);
    IS_NODE_PTR_NULL ();

    if (node_replace != NULL)
    {
        if (node_replace[*pos_replace] == node)
        {
            PRINT_TEX_PARAM ("%c", NAME_REPLACE (*pos_replace));
            *pos_replace += 1;

            return;
        }
    }

    switch (node->type)
    {
        case (NUM):
        {
            print_num (node, fp_tex, code_error);
            ERR_RET ();

            break;
        }
        case (VAR):
        {
            PRINT_TEX_PARAM ("%s", node->data.var);
            break;
        }
        case (OP):
        {
            print_tex_operator (node, PRINT_INFO);
            ERR_RET ();

            break;
        }
        case (DEF_TYPE):
        default:
        {
            break;
        }
    }
}

void print_tex_operator (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    if (node->data.types_op == DIV)
    {
        print_tex_div (node, PRINT_INFO);
        ERR_RET ();
    }
    else
    {
        print_tex_def_op (node, PRINT_INFO);
    }
}

void print_tex_def_op (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    bool is_bracket = false;

    if (node->data.types_op < OP_SEP)
    {
        if (node_replace == NULL || node_replace[*pos_replace] != node->left)
        {
            is_bracket = print_left_node_bracket (node, fp_tex, code_error);
            ERR_RET ();
        }
        
        print_tex_node (node->left, PRINT_INFO);
        ERR_RET ();

        PRINT_CLOSE_BRACKET (fp_tex);
    }

    PRINT_TEX_PARAM ("%s", NAME_OP_TEX[node->data.types_op]);
    
    if (node_replace == NULL || node_replace[*pos_replace] != node->right)
    {
        is_bracket = print_right_node_bracket (node, fp_tex, code_error);
        ERR_RET ();
    }

    print_tex_node (node->right, PRINT_INFO);
    ERR_RET ();

    PRINT_CLOSE_BRACKET (fp_tex);
}

void print_tex_div (NODE *node, FILE *fp_tex, NODE** node_replace, size_t *pos_replace, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    PRINT_TEX ("\\frac{");
    print_tex_node (node->left, PRINT_INFO);
    PRINT_TEX ("}{");
    print_tex_node (node->right, PRINT_INFO);
    PRINT_TEX ("}");
}

void print_replace (NODE *node, FILE *fp_tex, size_t n_diff, int *code_error)
{
    my_assert (node != NULL, ERR_PTR);
    my_assert (fp_tex != NULL, ERR_PTR);

    size_t n_replace = get_n_replace (node);

    NODE **node_replace = (NODE **) calloc (n_replace, sizeof (NODE *));
    my_assert (node_replace != NULL, ERR_MEM);

    size_t pos = 0;

    get_node_replace (node, node_replace, &pos, code_error);
    ERR_RET ();

    pos = 0;

    PRINT_TEX_PARAM ("\\[f^{(%d)}(x)=", n_diff);
    print_tex_node (node, fp_tex, node_replace, &pos, code_error);
    ERR_RET ();
    PRINT_TEX ("\\]\n");

    if (n_replace > 0)
    {
        PRINT_TEX ("Substitutions:\n");
    }

    for (size_t i = 0; i < n_replace; i++)
    {
        PRINT_TEX_PARAM ("\\[%c=", NAME_REPLACE (i));
        print_tex_node (node_replace[i], fp_tex, NULL, 0, code_error);
        ERR_RET ();

        PRINT_TEX ("\\]\n");
    }

    free (node_replace);
}

void get_node_replace (NODE *node, NODE **node_replace, size_t *pos_replace, int *code_error)
{
    IS_NODE_PTR_NULL ();

    get_node_replace (node->left, node_replace, pos_replace, code_error);
    get_node_replace (node->right, node_replace, pos_replace, code_error);

    if (node->parent != NULL)
    {
        if (node->tree_size >= STANDARD_SIZE_EXPR_TEX && 
            node->left->tree_size <= STANDARD_SIZE_EXPR_TEX &&
            node->right->tree_size <= STANDARD_SIZE_EXPR_TEX)
        {
            node_replace[*pos_replace] = node;
            *pos_replace += 1;

            return;
        }
    }
}

size_t get_n_replace (NODE *node)
{
    IS_NODE_PTR_NULL (0);

    size_t n_replace = 0;

    n_replace += get_n_replace (node->left);
    n_replace += get_n_replace (node->right);

    if (node->parent != NULL)
    {
        if (node->tree_size >= STANDARD_SIZE_EXPR_TEX && 
            node->left->tree_size <= STANDARD_SIZE_EXPR_TEX &&
            node->right->tree_size <= STANDARD_SIZE_EXPR_TEX)
        {
            return 1;
        }
    }
    
    return n_replace;
}

void print_tex_taylor (TREE *tree, FILE *fp_tex, int *code_error)
{
    my_assert (fp_tex != NULL, ERR_PTR);

    PRINT_TEX ("Taylor decomposition:\n");
    PRINT_TEX ("\\[f(x)=");

    PRINT_TEX ("f^{(0)}(x)");

    for (int i = 1; i <= tree->info.pow_taylor; i++)
    {
        PRINT_TEX_PARAM ("+ \\frac {1}{%d!} \\cdot f^{(%d)}(x)", i, i);
    }

    PRINT_TEX_PARAM ("+o(x^%d)", tree->info.pow_taylor);

    PRINT_TEX ("\\]\n");
    PRINT_TEX ("Substitutions for Taylor:\n");

    print_tex_tree (tree, fp_tex, 0, code_error);
    ERR_RET ();

    for (int i = 1; i <= tree->info.pow_taylor; i++)
    {
        n_diff (tree, 1, code_error);
        ERR_RET ();

        print_tex_tree (tree, fp_tex, i, code_error);
        ERR_RET ();
    }
}

#undef PRINT_TEX
#undef PRINT_TEX_PARAM

#endif