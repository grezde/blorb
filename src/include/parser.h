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
void syntax_free_node(SyntaxNode* sn);

SyntaxNode* syntax_parse_number(vector* tokens, int* index);
SyntaxNode* syntax_parse_sum(vector* tokens, int* index);

SyntaxNode* syntax_parse_all(vector* tokens);

void syntax_print_node(string* s, SyntaxNode* sn, int indent);
string syntax_print_tree(SyntaxNode* sn);