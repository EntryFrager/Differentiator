#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <TXLib.h>

#include "error.h"
#include "utils.h"

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
        tree_dump_text (tree, code_error, __FILE__, __func__, __LINE__);        \
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

enum op_command {
    ADD,
    SUB,
    MUL,
    DIV,
    DEG,
    SIN,
    COS,
    SQRT,
};

enum types {
    DEF_TYPE,
    NUM,
    OP,
    VAR
};

const bool LEFT  = false;
const bool RIGHT = true;

const bool INIT = true;
const bool INIT_NOT = false;

typedef double ELEMENT;

union DATA {
    char *var = NULL;
    ELEMENT value;
};

typedef struct NODE {
    NODE *left   = NULL;
    NODE *right  = NULL;
    NODE *parent = NULL;

    int type = 0;
    DATA *data = NULL;
} NODE;

typedef struct {
    char *fp_name_base = NULL;
    FILE *fp_base      = NULL;

    size_t size_file = 0;

    char *buf = NULL;

#ifdef DEBUG
    const char *fp_dump_text_name = NULL;
    const char *fp_dot_name       = NULL;
    const char *fp_name_html      = NULL;
    const char *fp_image          = NULL;

    FILE *fp_html_dot = NULL;
#endif
} INFO;

typedef struct {
    NODE *root = NULL;

    bool init_status = false;

    INFO info = {};
} TREE;

int create_tree (TREE *tree, DATA *data, int *code_error);

NODE *create_node (ELEMENT value, char *var, int type, NODE *left, NODE *right, NODE *parent, int *code_error);

int input_base (TREE *tree, int *code_error);

NODE *split_node (TREE *tree, NODE *node, NODE *parent, int *code_error);

void read_str (TREE *tree, NODE *node, int *code_error);

int add_node (NODE *node, ELEMENT value, char *var, int type, const bool side, int *code_error);

int delete_node (NODE *node, int *code_error);

NODE *copy_tree (NODE *node, NODE *parent, int *code_error);

int print_tree (NODE *node, FILE *stream, int *code_error);

int destroy_tree (TREE *tree, int *code_error);

#ifdef DEBUG
    int tree_verificator (TREE *tree);

    int node_verificator (NODE *node);

    void tree_dump_text (TREE *tree, const int code_error, 
                         const char *file_err, const char *func_err, 
                         const int line_err);

    void tree_dump_graph_viz (TREE *tree, const char *file_err, 
                              const char *func_err, const int line_err);

    int create_node_dot (NODE *node, FILE *stream, int ip_parent, int ip, char *color);

    void tree_dump_html (TREE *tree, int *code_error);
#endif

#endif //TREE_H