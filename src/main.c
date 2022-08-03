#include <stdio.h>
#include "utils.h"
#include "lexer.h"

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
    
    printf("> FILE %s:\n%s\n", argv[1], cnts);
    print_tokens(cnts);
    return 0;
}
