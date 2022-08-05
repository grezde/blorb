#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"

static bool show_tokens = false;
static bool show_syntax = true;

int main(int argc, char** argv) {
    
    /*
    printf("ARGS: %d\n", argc);
    for(int i=0; i<argc; i++)
        printf("%s\n", argv[i]);
    //*/
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
    int i=0;
    SyntaxNode* sn = syntax_parse_sum(&v, &i);

    if(show_tokens) {
        string s = tokens_print(&v);
        printf("%s", s.cstr);
        string_free(&s);
    }
    if(show_syntax) {
        string s = syntax_print_tree(sn);
        printf("%s", s.cstr);
        string_free(&s);
    }

    syntax_free_node(sn);
    tokens_free(&v);
    return 0;
}
