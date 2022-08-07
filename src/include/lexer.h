#pragma once
#include "utils.h"

enum ErrorType {
    E_MISC,
    E_COMPILER,
    E_LEX,
    E_SYNTAX,
    E_SEM,
    E_EVAL
};

typedef struct {
    enum ErrorType type;
    char* msg;
    int start;
    int end;
} Error;

enum TokenType {
    T_ERROR,
    T_NEWLINE,
    T_NUM_LIT,
    T_KEYWORD,
    T_IDENTIFIER,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_DIV,
    T_EOF,
    T_O_PAREN,
    T_C_PAREN,
    T_EQ,
    T_SC,
    T_COMMA
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

static const int TOKEN_KEYWORDS_LEN = 2;
static const char* TOKEN_KEYWORDS[] = { "var", "print" };

Token token_next(const char* str, int* index, vector* ers);
vector tokens_all(const char* str, vector* ers);
void tokens_free(vector* v);

string tokens_print(vector* v);
string token_print(Token* t);

vector token_line_positions(const char* str);
FullPosition token_get_position(vector* linev, int position);

void error_print(Error* e);
void errors_print(vector* v);
void errors_free(vector* v);
