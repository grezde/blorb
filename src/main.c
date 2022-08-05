#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"

static bool show_tokens = true;
static bool show_syntax = true;

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
    SyntaxNode* sn = syntax_parse_expr(&v, &i, 1);
    if(show_syntax) {
        printf("-- SYNTAX TREE --\n");
        string s = syntax_print_tree(sn);
        printf("%s", s.cstr);
        string_free(&s);
    }

    syntax_free_node(sn);
    tokens_free(&v);
    return 0;
}
