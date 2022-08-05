#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>

Token token_next(char* str, int* index) {
    Token a;
    int i = *index;
    a.pos = i;
    while(str[i] == ' ')
        i++;
    if(str[i] == '\0') {
        a.type = T_EOF;
        a.str = NULL;
    }
    if(str[i] == '\n') {
        a.type = T_NEWLINE;
        a.str = "\\n";
        i++;
    }
    else if(str[i] >= '0' && str[i] <= '9') {
        string lit = string_new();
        while(str[i] >= '0' && str[i] <= '9') {
            string_pushc(&lit, str[i]);    
            i++;
        }
        a.type = T_NUM_LIT;
        a.str = lit.cstr;
    }
    else if(str[i] == '+') {
        a.type = T_PLUS;
        a.str = "+";
        i++;
    }
    else if(str[i] == '-') {
        a.type = T_MINUS;
        a.str = "-";
        i++;
    }
    else if(str[i] == '*') {
        a.type = T_STAR;
        a.str = "*";
        i++;
    }
    else if(str[i] == '/') {
        a.type = T_DIV;
        a.str = "/";
        i++;
    }
    else if(str[i] == '(') {
        a.type = T_O_PAREN;
        a.str = "(";
        i++;
    }
    else if(str[i] == ')') {
        a.type = T_C_PAREN;
        a.str = ")";
        i++;
    }
    *index = i;
    return a;
}

vector tokens_all(char* str) {
    vector v = vector_new(sizeof(Token));
    Token t;
    int i = 0;
    do {
        t = token_next(str, &i);
        vector_push(&v, &t);
    } while(t.type != T_EOF);
    return v;
}

string tokens_print(vector* v) {
    string s = string_new();
    for(int i=0; i<v->length; i++) {
        Token* t = vector_item(v, i);
        string ts = token_print(t);
        char buffer[1000];
        snprintf(buffer, 995, "T%02d at %d: %s\n", i, t->pos, ts.cstr);
        string_free(&ts);
        string_pushs(&s, buffer);
    }
    return s;
}

void tokens_free(vector* v) {
    for(int i=0; i<v->length; i++) {
        Token* t = vector_item(v, i);
        if(t->type == T_NUM_LIT)
            free(t->str);
    }
    vector_free(v);
}

static const char* TOKEN_PRETTY_PRINTS[] = { "ERROR", "NEW LINE", " NUMBER", 
" PLUS", " MINUS", " STAR", " DIV", "END OF FILE", " OPEN PAREN", " CLOSING PAREN" };
string token_print(Token* t) {
    const char* s = TOKEN_PRETTY_PRINTS[t->type];
    if(s[0] != ' ')
        return string_from_lit(s);
    string str = string_from_lit(s+1);
    string_pushc(&str, ' ');
    string_pushs(&str, t->str);
    return str;
}