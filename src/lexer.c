#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// --------
// LEXING
// --------

int is_ok_var_start(char c) {
    return  
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

Token token_next(const char* str, int* index, vector* ers) {
    Token a;
    a.type = T_ERROR;
    int i = *index;
    while(str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
        i++;
    a.pos = i;
    if(str[i] == '\0') {
        a.type = T_EOF;
        a.str = "\0";
    }
    /*
    else if(str[i] == '\n') {
        a.type = T_NEWLINE;
        a.str = "\\n";
        i++;
    } //*/
    else if(str[i] >= '0' && str[i] <= '9') {
        string lit = string_new();
        while(str[i] >= '0' && str[i] <= '9') {
            string_pushc(&lit, str[i]);    
            i++;
        }
        a.type = T_NUM_LIT;
        a.str = lit.cstr;
    }
    else if(is_ok_var_start(str[i])) {
        string ident = string_new();
        while(is_ok_var_start(str[i]) || (str[i] >= '0' && str[i] <= '9')) {
            string_pushc(&ident, str[i]);
            i++;
        }
        for(int j=0; j<TOKEN_KEYWORDS_LEN; j++)
            if(strcmp(ident.cstr, TOKEN_KEYWORDS[j]) == 0) {
                a.type = T_KEYWORD;
                a.str = TOKEN_KEYWORDS[j];
                string_free(&ident);
                break;
            }
        if(a.type == T_ERROR) {
            a.type = T_IDENTIFIER;
            a.str = ident.cstr;
        }
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
    else if(str[i] == '=') {
        a.type = T_EQ;
        a.str = "=";
        i++;
    } else if(str[i] == ';') {
        a.type = T_SC;
        a.str = ";";
        i++;
    } else if(str[i] == ',') {
        a.type = T_COMMA;
        a.str = ",";
        i++;
    } else {
        string s = string_new();
        string_pushs(&s, "Unexpected character ");
        string_pushc(&s, str[i]);
        Error e;
        e.type = E_LEX;
        e.start = e.end = i;
        e.msg = s.cstr;
        vector_push(ers, &e);
        i++;
    }
    *index = i;
    return a;
}

vector tokens_all(const char* str, vector* ers) {
    vector v = vector_new(sizeof(Token));
    Token t;
    int i = 0;
    do {
        t = token_next(str, &i, ers);
        if(ers->length > 0)
            return v;
        vector_push(&v, &t);
    } while(t.type != T_EOF);
    return v;
}

// --------
// LEXING UTILS
// --------

static const char* ERROR_PRETTY_PRINTS[] = { "UNKNOWN", "COMPILER", 
"LEXICAL", "SYNTACTIC", "SEMANTIC", "EVALUATION" };
void error_print(Error* e) {
    const char* type = ERROR_PRETTY_PRINTS[e->type];
    string s = string_from_lit(type);
    string_pushs(&s, " ERROR: ");
    string_pushs(&s, e->msg);
    printf("%s\n", s.cstr);
    string_free(&s);
}

void errors_print(vector* v) {
    for(int i=0; i<v->length; i++)
        error_print(vector_item(v, i));
}

void errors_free(vector* v) {
    for(int i=0; i<v->length; i++) {
        Error* e = vector_item(v, i);
        free(e->msg);
    }
}

void token_free(Token* t) {
    if(t->type == T_NUM_LIT || t->type == T_ERROR || t->type == T_IDENTIFIER)
        if(t->str != NULL)
            free((void*)t->str);
}

void tokens_free(vector* v) {
    for(int i=0; i<v->length; i++)
        token_free(vector_item(v, i));
    vector_free(v);
}

static const char* TOKEN_PRETTY_PRINTS[] = { "ERROR", "NEW LINE", " NUMBER", 
" KEYWORD", " IDENT", " PLUS", " MINUS", " STAR", " DIV", "END OF FILE", 
" OPEN PAREN", " CLOSING PAREN", " EQUALS", " SEMICOLON", " COMMA" };
string token_print(Token* t) {
    const char* s = TOKEN_PRETTY_PRINTS[t->type];
    if(s[0] != ' ')
        return string_from_lit(s);
    string str = string_from_lit(s+1);
    string_pushc(&str, ' ');
    string_pushs(&str, t->str);
    return str;
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

vector token_line_positions(const char* str) {
    vector v = vector_new(sizeof(int));
    int i=0;
    vector_push(&v, &i);
    for(i=0; str[i] != '\0'; i++) {
        if(str[i] == '\n') {
            i++;
            vector_push(&v, &i);
            i--;
        }
    }
    free(vector_pop(&v));
    return v;
}

FullPosition token_get_position(vector* linev, int position) {
    // TODO binary search
    int i=0;
    while(i < linev->length && *(int*)vector_item(linev, i) < position)
        i++;
    i--;
    int line_pos = *(int*)vector_item(linev, i);
    FullPosition fp = { i+1, position-line_pos+1 };
    return fp;
}