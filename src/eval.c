#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Variable* eval_var_from_context(SemContext* ctx, const char* name) {
    if(ctx == NULL)
        return NULL;
    for(int i=0; i<ctx->vars.length; i++) {
        Variable* v = vector_item(&(ctx->vars), i);
        if(strcmp(name, v->name) == 0)
            return v;
    }
    return NULL;
}

void eval_context_set_error(SemContext* ctx, const char* msg) {
    if(ctx == NULL) {
        free((void*)msg);
        return;
    }
    const char* er = eval_context_get_error(ctx);
    if(er != NULL)
        return;
    Variable v = { msg, 0 };
    vector_push(&(ctx->vars), &v);
}

const char* eval_context_get_error(SemContext* ctx) {
    if(ctx == NULL)
        return NULL;
    for(int i=0; i<ctx->vars.length; i++) {
        Variable* v = vector_item(&(ctx->vars), i);
        if(v->name[0] == '#')
            return v->name;
    }
    return NULL;
}

void eval_context_set_var(SemContext* ctx, const char* name, int value) {
    if(ctx == NULL)
        return;
    Variable* vp = eval_var_from_context(ctx, name);
    if(vp != NULL)
        vp->value = value;
    else {
        Variable v = { name, value };
        vector_push(&(ctx->vars), &v);
    }
}

int eval_expr(SyntaxNode* sn, SemContext* ctx) {
    if(sn->type == P_NUMBER) {
        const char* s = sn->token.str;
        int x = 0, i=1;
        for(int i=0; s[i] != '\0'; i++)
            x = 10*x + (s[i] - '0');
        return x;
    }
    if(sn->type == P_VAR_NAME) {
        Variable* v = eval_var_from_context(ctx, sn->token.str);
        if(v == NULL) {
            string s = string_from_lit("#Variable ");
            string_pushs(&s, sn->token.str);
            string_pushs(&s, " was not declared.");
            eval_context_set_error(ctx, s.cstr);
            return 0;
        }
        return v->value;
    }
    if(sn->type == P_BINARY_OP) {
        SyntaxNode* sa = vector_item(&(sn->child_nodes), 0);
        SyntaxNode* sb = vector_item(&(sn->child_nodes), 1);
        int a = eval_expr(sa, ctx);
        int b = eval_expr(sb, ctx);
        switch(sn->token.type) {
            case T_PLUS: return a+b;
            case T_MINUS: return a-b;
            case T_DIV: 
                if(b == 0) {
                    eval_context_set_error(ctx, string_from_lit("#Division by 0.").cstr);
                    return 0;
                }
                return a/b;
            case T_STAR: return a*b;
        }
    }
    if(sn->type == P_UNARY_OP) {
        SyntaxNode* ss = vector_item(&(sn->child_nodes), 0);
        int s = eval_expr(ss, ctx);
        switch(sn->token.type) {
            case T_PLUS: return s;
            case T_MINUS: return -s;
        }
    }
}

vector eval_expr_list(SyntaxNode* sn) {
    vector v = vector_new(sizeof(int));
    int res;
    if(sn->type != P_LIST)  
        return v;
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* expr = vector_item(&(sn->child_nodes), i);
        res = eval_expr(expr, NULL);
        vector_push(&v, &res);
    }
    return v;
}

void eval_statement_file(SyntaxNode* sn) {
    SemContext sm;
    if(sn->type != P_LIST)
        return;
    sm.vars = vector_new(sizeof(Variable));
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* stm = vector_item(&(sn->child_nodes), i);
        if(stm->type == P_SCAN_DECL) {
            SyntaxNode* varname = vector_item(&(stm->child_nodes), 0);
            Variable* v = eval_var_from_context(&sm, varname->token.str);
            if(v == NULL) {
                printf("EVALUATION ERROR: Variable %s was not declared.\n", varname->token.str);
                vector_free(&sm.vars);
                return;
            }
            int d;
            scanf("%d", &d);
            eval_context_set_var(&sm, varname->token.str, d);
        }
        else if(stm->type == P_VAR_DECL) {
            for(int j=0; j<stm->child_nodes.length; j++) {
                SyntaxNode* stm2 = vector_item(&(stm->child_nodes), j);
                if(stm2->type == P_VAR_NAME) {
                    Variable* v = eval_var_from_context(&sm, stm2->token.str);
                    if(v != NULL) {
                        printf("EVALUATION ERROR: Variable %s already declared.\n", stm2->token.str);
                        vector_free(&sm.vars);
                        return;
                    }
                    eval_context_set_var(&sm, stm2->token.str, 0);
                }
                else {
                    SyntaxNode* varname = vector_item(&(stm2->child_nodes), 0);
                    SyntaxNode* expr = vector_item(&(stm2->child_nodes), 1);
                    int a = eval_expr(expr, &sm);
                    const char* er = eval_context_get_error(&sm);
                    if(er != NULL) {
                        printf("EVALUATION ERROR: %s\n", er+1);
                        free((void*)er);
                        vector_free(&sm.vars);
                        return;
                    }
                    Variable* v = eval_var_from_context(&sm, stm2->token.str);
                    if(v != NULL) {
                        printf("EVALUATION ERROR: Variable %s already declared.\n", stm2->token.str);
                        vector_free(&sm.vars);
                        return;
                    }
                    eval_context_set_var(&sm, varname->token.str, a);
                }
            }
        }
        else if(stm->type == P_PRINT_DECL) {
            SyntaxNode* expr = vector_item(&(stm->child_nodes), 0);
            int a = eval_expr(expr, &sm);
            const char* er = eval_context_get_error(&sm);
            if(er != NULL) {
                printf("EVALUATION ERROR: %s\n", er);
                free((void*)er);
                vector_free(&sm.vars);
                return;
            }
            printf("%d\n", a);
        }
        else if(stm->type == P_VAR_SET) {
            SyntaxNode* varname = vector_item(&(stm->child_nodes), 0);
            SyntaxNode* expr = vector_item(&(stm->child_nodes), 1);
            int a = eval_expr(expr, &sm);
            const char* er = eval_context_get_error(&sm);
            if(er != NULL) {
                printf("EVALUATION ERROR: %s\n", er);
                free((void*)er);
                vector_free(&sm.vars);
                return;
            }
            Variable* v = eval_var_from_context(&sm, varname->token.str);
            if(v == NULL) {
                printf("EVALUATION ERROR: Variable %s was not declared.\n", varname->token.str);
                vector_free(&sm.vars);
                return;
            }
            eval_context_set_var(&sm, varname->token.str, a);
        }
    }
    vector_free(&sm.vars);
}

