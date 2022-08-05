#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

SyntaxNode* syntax_new_node(enum SyntaxNodeType type, Token token) {
    SyntaxNode* sn = malloc(sizeof(SyntaxNode));
    sn->type = type;
    sn->token = token;
    sn->child_nodes = vector_new(sizeof(SyntaxNode));
    return sn;
}

void syntax_free_node(SyntaxNode* sn) {
    if(sn == NULL)
        return;
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* isn = vector_item(&(sn->child_nodes), i);
        syntax_free_node(isn);
    }
    vector_free(&(sn->child_nodes));
}

SyntaxNode* syntax_parse_number(vector* tokens, int* index) {
    Token* token = vector_item(tokens, *index);
    if(token->type != T_NUM_LIT)
        return NULL;
    (*index)++;
    SyntaxNode* sn = syntax_new_node(P_NUMBER, *token);
    return sn;
}

SyntaxNode* syntax_parse_sum(vector* tokens, int* index) {
    int i = *index;
    SyntaxNode* sn1 = syntax_parse_number(tokens, &i);
    if(sn1 == NULL)
        return NULL;
    Token* token = vector_item(tokens, i);
    if(token->type != T_PLUS && token->type != T_MINUS)
        return NULL;
    i++;
    SyntaxNode* sn2 = syntax_parse_number(tokens, &i);
    if(sn2 == NULL)
        return NULL;
    SyntaxNode* sn = syntax_new_node(P_BINARY_OP, *token);
    vector_push(&(sn->child_nodes), sn1);
    vector_push(&(sn->child_nodes), sn2);
    free(sn1);
    free(sn2);
    *index = i;
    return sn;
}

SyntaxNode* syntax_parse_all(vector* tokens) {
    return NULL;
}

static const const char* SYNTAX_PRETTY_PRINTS[] = { "ERROR", " NUMBER", " UN OP", " BIN OP", "LIST" };

void syntax_print_node(string* s, SyntaxNode* sn, int indent) {
    for(int i=0; i<indent; i++)
        string_pushs(s, "   ");
    const char* pp = SYNTAX_PRETTY_PRINTS[sn->type];
    if(pp[0] == ' ') {
        string_pushs(s, pp+1);
        string_pushs(s, " (");
        string_pushs(s, sn->token.str);
        string_pushs(s, ")");
    }
    else
        string_pushs(s, pp);
    if(sn->child_nodes.length > 0)
        string_pushs(s, ":");
    string_pushs(s, "\n");
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* child = vector_item(&sn->child_nodes, i);
        syntax_print_node(s, child, indent+1);
    }
}

string syntax_print_tree(SyntaxNode* sn) {
    string s = string_new();
    syntax_print_node(&s, sn, 0);
    return s;
}