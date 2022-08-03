#include "lexer.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

Token next_token(char** strptr) {
    Token a;
    char* str = *strptr;
    while(str[0] == ' ')
        str++;
    if(str[0] == '\0') {
        a.type = T_EOF;
        a.str = NULL;
    }
    if(str[0] == '\n') {
        a.type = T_NEWLINE;
        a.str = "\\n";
        str++;
    }
    else if(str[0] >= '0' && str[0] <= '9') {
        int id = 0;
        string lit = string_new();
        while(str[0] >= '0' && str[0] <= '9') {
            string_pushc(&lit, str[0]);    
            str++;
        }
        a.type = T_NUM_LIT;
        a.str = lit.cstr;
    }
    else if(str[0] == '+') {
        a.type = T_PLUS;
        a.str = "+";
        str++;
    }
    else if(str[0] == '-') {
        a.type = T_MINUS;
        a.str = "-";
        str++;
    }
    else if(str[0] == '*') {
        a.type = T_STAR;
        a.str = "*";
        str++;
    }
    else if(str[0] == '/') {
        a.type = T_DIV;
        a.str = "/";
        str++;
    }
    *strptr = str;
    return a;
}

void print_tokens(char* str) {
    Token t;
    do {
        t = next_token(&str);
        string s = token_pretty_print(&t);
        printf("TOKEN: %s\n", s.cstr);
        string_free(&s);
    } while(t.type != T_EOF);
}

const char* TOKEN_PRETTY_PRINTS[] = { "ERROR", "NEW LINE", " NUMBER", " PLUS", " MINUS", " STAR", " DIV", "END OF FILE" };

string token_pretty_print(Token* t) {
    const char* s = TOKEN_PRETTY_PRINTS[t->type];
    if(s[0] != ' ')
        return string_from_lit(s);
    string str = string_from_lit(s+1);
    string_pushc(&str, ' ');
    string_pushs(&str, t->str);
    return str;
}