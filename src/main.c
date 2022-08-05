#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "eval.h"

static bool show_headers = false;
static bool show_file_contents = false;
static bool show_tokens  = false;
static bool show_syntax  = false;
static bool show_eval    = true;

int eval_file(const char* filename) {
    const char* cnts = read_file(filename);
    if(cnts == NULL) {
        printf("COMPILER ERROR: compiler input: File %s could not be read.\n", filename);
        return 1;
    }
    if(show_file_contents) {
        if(show_headers)
            printf("-- FILE CONTENTS --\n");
        printf("file %s:\n%s\n", filename, cnts);
    }
    
    vector lns = token_line_positions(cnts);
    vector v = tokens_all(cnts); 
    Token* t = vector_item(&v, v.length-1);
    if(t->type == T_ERROR) {
        FullPosition fp = token_get_position(&lns, t->pos);
        printf("LEXER ERROR: file %s, line %d, row %d: %s\n", filename, fp.line, fp.row, t->str);
        tokens_free(&v);
        vector_free(&lns);
        free((void*)cnts);
        return 2;
    }
    if(show_tokens) {
        if(show_headers)
            printf("-- TOKENS --\n");
        string s = tokens_print(&v);
        printf("%s", s.cstr);
        string_free(&s);
    }

    SyntaxNode* sn = syntax_parse_all(&v);
    if(sn->type == P_ERROR) {
        FullPosition fp = token_get_position(&lns, sn->token.pos);
        printf("PARSER ERROR: file %s, line %d, row %d: %s\n", filename, fp.line, fp.row, sn->token.str);
        syntax_free_node(sn);
        tokens_free(&v);
        vector_free(&lns);
        free((void*)cnts);
        return 3;
    }
    if(show_syntax) {
        if(show_headers)
            printf("-- SYNTAX TREE --\n");
        string s = syntax_print_tree(sn);
        printf("%s", s.cstr);
        string_free(&s);
    }


    vector results = eval_list(sn);
    if(show_eval) {
        if(show_headers)
            printf("-- EVALUATION --\n");
        for(int i=0; i<results.length; i++) {
            int* result = vector_item(&results, i);
            printf("%d\n", *result);
        }
    } 

    syntax_free_node(sn);
    tokens_free(&v);
    vector_free(&lns);
    free((void*)cnts);
}

int main(int argc, char** argv) {
    
    if(argc != 2) {
        printf("COMPILER ERROR: compiler input: Wrong usage. Correct usage is ./blorbc $(filename)\n");
        return 1;
    }
    int res = eval_file(argv[1]);
    return 0;
}
