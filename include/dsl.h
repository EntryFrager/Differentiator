#ifndef DSL_H
#define DSL_H

#define IS_NODE_PTR_NULL(ret_value) if(!node) {return ret_value;}

#define IF_NUM(node_type, expr) if (node_type == NUM) {expr; break;}

#define NUM_(value, parent) create_node_num (value, NULL, NULL, parent, code_error)
#define VAR_(str, parent) create_node_var (str, NULL, NULL, parent, code_error)
#define OP_(types_op, l_side, r_side, parent) create_node_op (types_op, l_side, r_side, parent, code_error)

#define ADD_(node_1, node_2) create_node_op (ADD, node_1, node_2, NULL, code_error)
#define SUB_(node_1, node_2) create_node_op (SUB, node_1, node_2, NULL, code_error)
#define MUL_(node_1, node_2) create_node_op (MUL, node_1, node_2, NULL, code_error)
#define DIV_(node_1, node_2) create_node_op (DIV, node_1, node_2, NULL, code_error)
#define DEG_(node_1, deg) create_node_op (DEG, node_1, deg, NULL, code_error)
#define SIN_(node) create_node_op (SIN, NUM_(1, NULL), node, NULL, code_error)
#define COS_(node) create_node_op (COS, NUM_(1, NULL), node, NULL, code_error)
#define SQRT_(node) create_node_op (SQRT, NUM_(1, NULL), node, NULL, code_error)
#define LN_(node) create_node_op (LN, NUM_(1, NULL), node, NULL, code_error)

#define DIF_L diff (node->left, code_error)
#define DIF_R diff (node->right, code_error)

#define COPY_L copy_tree (node->left, NULL, code_error)
#define COPY_R copy_tree (node->right, NULL, code_error)

#define L_TYPE node->left->type
#define L_VALUE node->left->data.value
#define R_TYPE node->right->type
#define R_VALUE node->right->data.value

#define L_RE_HANGING node = hanging_tree (node, node->left, node->parent, code_error)
#define R_RE_HANGING node = hanging_tree (node, node->right, node->parent, code_error)

#define PRINT_CLOSE_BRACKET(stream) if (is_bracket) {fprintf (stream, ")"); is_bracket = false;}

#define str_len(str) sizeof (str) - 1

#endif //DSL_H