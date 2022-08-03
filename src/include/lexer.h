#pragma once
#include "utils.h"

enum TOKEN_TYPE {
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
    enum TOKEN_TYPE type;
    char* str;
    int pos;
    char* filename;
} Token;

Token next_token(char** strptr);
string token_pretty_print(Token* t);

void print_tokens(char* str);