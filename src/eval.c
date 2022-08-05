#include "eval.h"

int eval_expr(SyntaxNode* sn) {
    if(sn->type == P_NUMBER) {
        const char* s = sn->token.str;
        int x = 0, i=1;
        for(int i=0; s[i] != '\0'; i++)
            x = 10*x + (s[i] - '0');
        return x;
    }
    if(sn->type == P_BINARY_OP) {
        SyntaxNode* sa = vector_item(&(sn->child_nodes), 0);
        SyntaxNode* sb = vector_item(&(sn->child_nodes), 1);
        int a = eval_expr(sa);
        int b = eval_expr(sb);
        switch(sn->token.type) {
            case T_PLUS: return a+b;
            case T_MINUS: return a-b;
            case T_DIV: return a/b;
            case T_STAR: return a*b;
        }
    }
}

vector eval_list(SyntaxNode* sn) {
    vector v = vector_new(sizeof(int));
    int res;
    if(sn->type != P_LIST)  
        return v;
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* expr = vector_item(&(sn->child_nodes), i);
        res = eval_expr(expr);
        vector_push(&v, &res);
    }
    return v;
}


