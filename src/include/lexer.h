#pragma once
#include "utils.h"

enum TokenType {
    T_ERROR,
    T_NEWLINE,
    T_NUM_LIT,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_DIV,
    T_EOF,
};

typedef struct {
    enum TokenType type;
    char* str;
    int pos;
} Token;

Token token_next(char* str, int* index);
vector tokens_all(char* str);
void tokens_free(vector* v);

string tokens_print(vector* v);
string token_print(Token* t);