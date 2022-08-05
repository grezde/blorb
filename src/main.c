#include <stdio.h>
#include <stdbool.h>
#include "eval.h"

static bool show_tokens = false;
static bool show_syntax = true;
static bool show_eval   = true;

int main(int argc, char** argv) {
    
    if(argc != 2) {
        printf("Wrong usage. Correct usage is ./blorbc $(filename)\n");
        return 1;
    }

    char* cnts = read_file(argv[1]);
    if(cnts == NULL) {
        printf("File could not be read.\n");
        return 1;
    }
    
    vector v = tokens_all(cnts);
    if(show_tokens) {
        printf("-- TOKENS --\n");
        string s = tokens_print(&v);
        printf("%s", s.cstr);
        string_free(&s);
    }

    int i=0;
    SyntaxNode* sn = syntax_parse_all(&v);
    if(show_syntax) {
        printf("-- SYNTAX TREE --\n");
        string s = syntax_print_tree(sn);
        printf("%s", s.cstr);
        string_free(&s);
    }

    vector results = eval_list(sn);
    if(show_eval) {
        printf("-- EVALUATION --\n");
        for(int i=0; i<results.length; i++) {
            int* result = vector_item(&results, i);
            printf("%d\n", *result);
        }
    }

    syntax_free_node(sn);
    tokens_free(&v);
    return 0;
}
