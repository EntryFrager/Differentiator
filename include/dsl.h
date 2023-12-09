#ifndef DSL_H
#define DSL_H

#define CHECK_NODE_PTR(ret_value) if(!node) {return ret_value;}

#define CHECK_TYPE(type, expr_var, expr_num, expr_op)   \
    if (type == VAR) {expr_var}                         \
    else if (type == NUM) {expr_num}                    \
    else {expr_op}

#define NUM_(value, parent) create_node_num (value, NULL, NULL, parent, code_error)
#define VAR_(str, parent) create_node_var (str, NULL, NULL, parent, code_error)
#define OP_(types_op, l_side, r_side, parent) create_node_op (types_op, l_side, r_side, parent, code_error)

#define ADD_(node_1, node_2) create_node_op (ADD, node_1, node_2, NULL, code_error)
#define SUB_(node_1, node_2) create_node_op (SUB, node_1, node_2, NULL, code_error)
#define MUL_(node_1, node_2) create_node_op (MUL, node_1, node_2, NULL, code_error)
#define DIV_(node_1, node_2) create_node_op (DIV, node_1, node_2, NULL, code_error)
#define DEG_(node_1, deg) create_node_op (DEG, node_1, deg, NULL, code_error)
#define SIN_(node) create_node_op (SIN, NULL, node, NULL, code_error)
#define COS_(node) create_node_op (COS, NULL, node, NULL, code_error)

#define DIF_L diff (node->left, code_error)
#define DIF_R diff (node->right, code_error)

#define COPY_L copy_tree (node->left, NULL, code_error)
#define COPY_R copy_tree (node->right, NULL, code_error)

#define L_TYPE node->left->type
#define L_VALUE node->left->data.value
#define R_TYPE node->right->type
#define R_VALUE node->right->data.value

#define L_RE_HANGING CHECK_ERROR_RETURN (node = hanging_tree (node, node->left, node->parent, code_error), NULL)
#define R_RE_HANGING CHECK_ERROR_RETURN (node = hanging_tree (node, node->right, node->parent, code_error), NULL)

#define CHECK_NUM(node_type, expr) if (node_type == NUM) {expr; break;}

#define PRINT_BRACKET(node_d, str) if (node_d != NULL && node_d->type == OP)    \
    {fprintf (stream, str);}

#endif //DSL_H