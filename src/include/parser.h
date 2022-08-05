#pragma once
#include "lexer.h"

enum SyntaxNodeType {
    P_ERROR,
    P_NUMBER,
    P_UNARY_OP,
    P_BINARY_OP,
    P_LIST
};

typedef struct {
    enum SyntaxNodeType type;
    Token token;
    vector child_nodes;
} SyntaxNode;


SyntaxNode* syntax_new_node(enum SyntaxNodeType type, Token token);
void syntax_node_add_child(SyntaxNode* sn, SyntaxNode* child);
void syntax_free_node(SyntaxNode* sn);
string syntax_print_tree(SyntaxNode* sn);

static const int MAX_PRECEDENCE = 3;
static const int MIN_PRECEDENCE = 0; 
int syntax_op_precedence(enum TokenType tt);
SyntaxNode* syntax_try_parse(vector* tokens, int* index, enum TokenType tt, enum SyntaxNodeType st);
Token* tokens_next(vector* tokens, int* index);

SyntaxNode* syntax_parse_number(vector* tokens, int* index);
SyntaxNode* syntax_parse_expr_recursive(vector* tokens, int* index, int precedence, SyntaxNode* staring_node);
SyntaxNode* syntax_parse_start(vector* tokens, int* index);
SyntaxNode* syntax_parse_expr(vector* tokens, int* index);

SyntaxNode* syntax_parse_all(vector* tokens);
