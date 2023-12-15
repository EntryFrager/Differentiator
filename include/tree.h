#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <TXLib.h>

#include "dsl.h"
#include "error.h"
#include "utils.h"

#define BLACK_COLOR "\"#000000\""
#define BLUE_COLOR "\"#00BFFF\""
#define PURPLE_COLOR "\"#8B00FF\""
#define RED_COLOR "\"#ff0000\""
#define LIGHT_GREEN_COLOR "\"#ccff99\""
#define BACK_GROUND_COLOR "\"#696969\""

#define FONTNAME "\"Times-New-Roman\""

#define DEBUG_TREE

#ifdef DEBUG_TREE
    #define TREE_LOG(tree, code_error)                                          \
    {                                                                           \
        tree_dump_text (tree, &code_error, __FILE__, __func__, __LINE__);       \
        tree_dump_graph_viz (tree, __FILE__, __func__, __LINE__);               \
    }

    #define assert_tree(tree, ret_value)                                                            \
    {                                                                                               \
        if ((*code_error |= (tree_verificator (tree) | node_verificator (tree->root))) != ERR_NO)   \
        {                                                                                           \
            TREE_LOG (tree, *code_error)                                                            \
            return ret_value;                                                                       \
        }                                                                                           \
    }
#else
    #define CALL_DUMP(...)
    #define assert_tree(...)
#endif

enum op_comand {
    OP_NO = -1,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    OP_SEP,
    SIN,
    COS,
    SQRT,
    LN,
    OPEN_BRACKET,
    CLOSE_BRACKET
};

enum types {
    DEF_TYPE,
    NUM,
    OP,
    VAR
};

const bool LEFT  = false;
const bool RIGHT = true;

typedef double ELEMENT;

union DATA {
    ELEMENT value;
    op_comand types_op;
    char *var;
};

typedef struct NODE {
    NODE *left   = NULL;
    NODE *right  = NULL;
    NODE *parent = NULL;
    
    size_t tree_size = 0;

    types type = DEF_TYPE;
    DATA data = {};
} NODE;

typedef struct TOKEN {
    types type = DEF_TYPE;
    DATA data = {};
} TOKEN;

typedef struct {
    char **vars = NULL;
    size_t n_var = 0;
} TABLE_NAME;

typedef struct {
    const char *fp_name_expr = NULL;
    FILE *fp_expr      = NULL;

    size_t size_file = 0;

    char *buf = NULL;

    char *var_diff = NULL;
    int pow_taylor = 0;
    ELEMENT value_point = 0;

#ifdef DEBUG
    const char *fp_dump_text_name = NULL;
    const char *fp_dot_name       = NULL;
    const char *fp_name_html      = NULL;
    const char *fp_image          = NULL;
    const char *fp_tex_name       = NULL;
#endif
} INFO;

typedef struct {
    NODE *root = NULL; 

    TOKEN *token = NULL;
    size_t n_token = 0;

    bool is_init = false;

    INFO info = {};
    TABLE_NAME table_name = {};
} TREE;

int create_tree (TREE *tree, int argc, char *argv[], int *code_error);

NODE *create_node_num (ELEMENT value, NODE *left, NODE *right, NODE *parent, int *code_error);

NODE *create_node_op (op_comand types_op, NODE *left, NODE *right, NODE *parent, int *code_error);

NODE *create_node_var (char *var, NODE *left, NODE *right, NODE *parent, int *code_error);

NODE *set_parent (NODE *node, NODE *parent);

int delete_node (NODE *node);

NODE *copy_tree (NODE *node, NODE *parent, int *code_error);

size_t get_tree_size (NODE *node);

void print_tree (NODE *node, FILE *stream, int *code_error);

void print_num (NODE *node, FILE *stream, int *code_error);

void print_operator (NODE *node, FILE *stream, int *code_error);

bool print_left_node_bracket (NODE *node, FILE *stream, int *code_error);

bool print_right_node_bracket (NODE *node, FILE *stream, int *code_error);

bool print_bracket (NODE *node, NODE *node_side, FILE *stream, int *code_error);

int destroy_tree (TREE *tree, int *code_error);

#ifdef DEBUG
    int tree_verificator (TREE *tree);

    int node_verificator (NODE *node);

    void tree_dump_text (TREE *tree, const int *code_error, 
                         const char *file_err, const char *func_err, 
                         const int line_err);

    void tree_dump_graph_viz (TREE *tree, const char *file_err, 
                              const char *func_err, const int line_err);

    void tree_dump_html (TREE *tree, int *code_error);

    void print_tex (TREE *tree, int *code_error);
#endif

#endif //TREE_H