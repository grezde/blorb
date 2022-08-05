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
    T_O_PAREN,
    T_C_PAREN
};

typedef struct {
    enum TokenType type;
    const char* str;
    int pos;
} Token;

typedef struct {
    int line;
    int row;
} FullPosition;

Token token_next(const char* str, int* index);
vector tokens_all(const char* str);
void tokens_free(vector* v);

string tokens_print(vector* v);
string token_print(Token* t);

vector token_line_positions(const char* str);
FullPosition token_get_position(vector* linev, int position);
