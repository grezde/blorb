#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

// --------
// SYNTAX UTILS
// --------

SyntaxNode* syntax_new_node(enum SyntaxNodeType type, Token token) {
    SyntaxNode* sn = malloc(sizeof(SyntaxNode));
    sn->type = type;
    sn->token = token;
    sn->child_nodes = vector_new(sizeof(SyntaxNode));
    return sn;
}

SyntaxNode* syntax_new_error_node(const char* message, int position_start, int position_end) {
    SyntaxNode* sn = malloc(sizeof(SyntaxNode));
    sn->type = P_ERROR;
    Token t = { T_ERROR, message, position_end };
    sn->token = t;
    sn->child_nodes = vector_new(sizeof(SyntaxNode));
    return sn;
}

void syntax_node_add_child(SyntaxNode* sn, SyntaxNode* child) {
    vector_push(&(sn->child_nodes), child);
    free(child);
}

static void syntax_free_node_recursive(SyntaxNode* sn) {
    if(sn->type == P_ERROR)
        free((void*)sn->token.str);
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* isn = vector_item(&(sn->child_nodes), i);
        syntax_free_node_recursive(isn);
    }
    vector_free(&(sn->child_nodes));
}

void syntax_free_node(SyntaxNode* sn) {
    if(sn == NULL)
        return;
    syntax_free_node_recursive(sn);
    free(sn);
}

static const const char* SYNTAX_PRETTY_PRINTS[] = { "ERROR", " NUMBER", " UN OP", 
" BIN OP", "LIST", " VAR DECL", " VAR SET", "PRINT", " VARIABLE" };
static void syntax_print_tree_recursive(string* s, SyntaxNode* sn, int indent) {
    for(int i=0; i<indent; i++)
        string_pushs(s, "   ");
    if(sn == NULL) {
        string_pushs(s, "NULL\n");
        return;   
    }
    const char* pp = SYNTAX_PRETTY_PRINTS[sn->type];
    if(pp[0] == ' ') {
        string_pushs(s, pp+1);
        string_pushs(s, " (");
        string_pushs(s, sn->token.str);
        string_pushs(s, ")");
    }
    else
        string_pushs(s, pp);
    if(sn->child_nodes.length > 0)
        string_pushs(s, ":");
    string_pushs(s, "\n");
    for(int i=0; i<sn->child_nodes.length; i++) {
        SyntaxNode* child = vector_item(&sn->child_nodes, i);
        syntax_print_tree_recursive(s, child, indent+1);
    }
}

string syntax_print_tree(SyntaxNode* sn) {
    string s = string_new();
    syntax_print_tree_recursive(&s, sn, 0);
    return s;
}

// --------
// PARSING
// --------

int syntax_op_precedence(enum TokenType tt) {
    switch(tt) {
        case T_PLUS: case T_MINUS:
            return 1;
        case T_STAR: case T_DIV:
            return 2;
        default:
            return -1;
    }
}

Token* tokens_next(vector* tokens, int* index) {
    if(*index >= tokens->length)
        return vector_item(tokens, tokens->length+1);
    Token* t = vector_item(tokens, *index);
    (*index)++;
    return t;
}

SyntaxNode* syntax_try_parse(vector* tokens, int* index, enum TokenType tt, enum SyntaxNodeType st) {
    Token* token = vector_item(tokens, *index);
    if(token->type != tt)
        return NULL;
    (*index)++;
    SyntaxNode* sn = syntax_new_node(st, *token);
    return sn;
}

SyntaxNode* syntax_parse_number(vector* tokens, int* index) {
    return syntax_try_parse(tokens, index, T_NUM_LIT, P_NUMBER);
}

SyntaxNode* syntax_parse_expr_start(vector* tokens, int* index) {
    int i = *index;
    printf ("INDEX %d\n", i);
    Token* ct = tokens_next(tokens, &i);
    SyntaxNode* val = NULL;
    if(ct->type == T_PLUS || ct->type == T_MINUS) {
        SyntaxNode* inside = syntax_parse_expr_start(tokens, &i);
        printf("INSIDE %s\n", syntax_print_tree(inside).cstr);
        if(inside->type == P_ERROR)
            return inside;
        val = syntax_new_node(P_UNARY_OP, *ct);
        syntax_node_add_child(val, inside);
        *index = i;
        return val;
    }
    else if(ct->type == T_NUM_LIT)
        val = syntax_new_node(P_NUMBER, *ct);
    else if(ct->type == T_IDENTIFIER)
        val = syntax_new_node(P_VAR_NAME, *ct);
    else if(ct->type == T_O_PAREN) {
        val = syntax_parse_expr(tokens, &i);
        if(val->type == P_ERROR)
            return val;
        ct = tokens_next(tokens, &i);
        if(ct->type != T_C_PAREN) {
            syntax_free_node(val);
            const char* s = string_from_lit("Opening parenthesis has no matching expression and closing parenthesis.").cstr;
            return syntax_new_error_node(s, *index, *index+1);
        }
    } else {
        string s = string_from_lit("Expected expression, instead got token ");
        string ts = token_print(ct);
        string_pushs(&s, ts.cstr);
        string_pushs(&s, ".");
        string_free(&ts);
        return syntax_new_error_node(s.cstr, *index, *index);
    }
    *index = i;
    return val;
}

void syntax_node_vector_free(vector* sns) {
    for(int i=0; i<sns->length; i++) {
        SyntaxNode** snp = vector_item(sns, i);
        syntax_free_node(*snp);
    }
    vector_free(sns);
}

SyntaxNode* syntax_parse_expr_recursive(vector* tokens, int* index, int precedence, SyntaxNode* starting_node) {
    //printf("STARTING TERM: %s", syntax_print_tree(starting_node).cstr);

    int i = *index;
    // i is on operator
    Token* ct = tokens_next(tokens, &i);
    // i is on expression start
    int cprec = syntax_op_precedence(ct->type);
    if(cprec < precedence)
        return starting_node;
    vector terms = vector_new(sizeof(SyntaxNode*));
    vector legs = vector_new(sizeof(SyntaxNode*));
    vector_push(&terms, &starting_node);
    while(cprec >= precedence) {
        if(cprec > precedence) {
            i--;
            SyntaxNode** snp = vector_pop(&terms);
            //printf("POPING A TERM: %s", syntax_print_tree(*snp).cstr);
            SyntaxNode* term = syntax_parse_expr_recursive(tokens, &i, cprec, *snp);
            if(term->type == P_ERROR) {
                free(snp);
                syntax_node_vector_free(&terms);
                syntax_node_vector_free(&legs);
                return term;
            }
            vector_push(&terms, &term);
            free(snp);
        } else {
            SyntaxNode* newleg = syntax_new_node(P_BINARY_OP, *ct);
            vector_push(&legs, &newleg);
            //printf("ADDING NEW LEG: %s", syntax_print_tree(newleg).cstr);
            SyntaxNode* term = syntax_parse_expr_start(tokens, &i); //gets the number
            if(term->type == P_ERROR) {
                string msg = string_from_string(term->token.str);
                string_pushs(&msg, " Binary operator ");
                string_pushs(&msg, ct->str);
                string_pushs(&msg, " has no second expression.");
                term->token.str = msg.cstr;
                syntax_node_vector_free(&terms);
                syntax_node_vector_free(&legs);
                return term;
            }
            //printf("ADDING NEW TERM: %s", syntax_print_tree(term).cstr);
            vector_push(&terms, &term);
        }
        ct = tokens_next(tokens, &i);
        cprec = syntax_op_precedence(ct->type);
    }
    *index = i-1;

    /* DEBUG
    printf("--PREC %d\n", precedence);
    printf("-LEGS %d\n", legs.length);
    for(int j=0; j<legs.length; j++)
        printf("%s", syntax_print_tree(*(SyntaxNode**)vector_item(&legs, j)).cstr);
    printf("-TERMS %d\n", terms.length);
    for(int j=0; j<terms.length; j++)
        printf("%s", syntax_print_tree(*(SyntaxNode**)vector_item(&terms, j)).cstr);
    printf("\n");
    //*/

    if(legs.length == 0) {
        SyntaxNode* sn = *(SyntaxNode**)vector_item(&terms, 0);
        vector_free(&terms);
        vector_free(&legs);
        return sn;
    }
    SyntaxNode* red = *(SyntaxNode**)vector_item(&legs, 0);
    syntax_node_add_child(red, *(SyntaxNode**)vector_item(&terms, 0));
    syntax_node_add_child(red, *(SyntaxNode**)vector_item(&terms, 1));
    for(int j=1; j<legs.length; j++) {
        SyntaxNode* newred = *(SyntaxNode**)vector_item(&legs, j);
        syntax_node_add_child(newred, red);
        syntax_node_add_child(newred, *(SyntaxNode**)vector_item(&terms, j+1));
        red = newred;
    }
    vector_free(&terms);
    vector_free(&legs);
    //printf("EXITING\n");
    return red;
}

SyntaxNode* syntax_parse_expr(vector* tokens, int* index) {
    SyntaxNode* start = syntax_parse_expr_start(tokens, index);
    if(start->type == P_ERROR)
        return start;
    return syntax_parse_expr_recursive(tokens, index, MIN_PRECEDENCE, start);
}

SyntaxNode* syntax_parse_print_decl(vector* tokens, int* index) {
    int i = *index;
    Token* t = tokens_next(tokens, &i);
    if(t->type == T_KEYWORD && strcmp(t->str, "print") == 0) {
        Token tt = *t;
        SyntaxNode* expr = syntax_parse_expr(tokens, &i);
        if(expr->type == T_ERROR)
            return expr;
        t = tokens_next(tokens, &i);
        if(t->type != T_SC) {
            string s = string_from_lit("Expected SEMICOLON (;) in print statement, but instead got token ");
            string_concat(&s, token_print(t));
            string_pushs(&s, ".");
            return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
        }
        SyntaxNode* sn = syntax_new_node(P_PRINT_DECL, tt);
        syntax_node_add_child(sn, expr);
        *index = i;
        return sn;
    }
    string s = string_from_lit("Expected KEYWORD (print) for print declaration, but instead got token ");
    string_concat(&s, token_print(t));
    string_pushs(&s, ".");
    return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
}

SyntaxNode* syntax_parse_var_set(vector* tokens, int* index) {
    int i= *index;
    Token* t = tokens_next(tokens, &i);
    if(t->type != T_IDENTIFIER) {
        string s = string_from_lit("Expected variable name, but instead got token ");
        string_concat(&s, token_print(t));
        string_pushs(&s, ".");
        return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
    }
    Token vart = *t;
    t = tokens_next(tokens, &i);
    if(t->type != T_EQ) {
        string s = string_from_lit("Expected EQUALS (=) in variable asssignement, but instead got token ");
        string_concat(&s, token_print(t));
        string_pushs(&s, ".");
        return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
    }
    SyntaxNode* expr = syntax_parse_expr(tokens, &i);
    if(expr->type == P_ERROR)
        return expr;
    t = tokens_next(tokens, &i);
    if(t->type != T_SC) {
        string s = string_from_lit("Expected SEMICOLON (;) in variable asssignement, but instead got token ");
        string_concat(&s, token_print(t));
        string_pushs(&s, ".");
        syntax_free_node(expr);
        return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
    }
    SyntaxNode* sn = syntax_new_node(P_VAR_SET, vart);
    syntax_node_add_child(sn, expr);
    *index = i;
    return sn;
}

SyntaxNode* syntax_parse_var_decl(vector* tokens, int* index) {
    int i = *index;
    Token* t = tokens_next(tokens, &i);
    if(t->type == T_KEYWORD && strcmp("var", t->str) == 0) {
        t = tokens_next(tokens, &i);
        if(t->type != T_IDENTIFIER) {
            string s = string_from_lit("Expected identifier after var keyword, but instead got token ");
            string_concat(&s, token_print(t));
            string_pushs(&s, ".");
            return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
        }
        Token idtoken = *t;
        t = tokens_next(tokens, &i);
        if(t->type != T_EQ) {
            string s = string_from_lit("Expected EQUALS (=) in variable declaration, but instead got token ");
            string_concat(&s, token_print(t));
            string_pushs(&s, ".");
            return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
        }
        SyntaxNode* sn = syntax_parse_expr(tokens, &i);
        if(sn->type == P_ERROR) {
            string s = string_from_string(sn->token.str);
            string_pushs(&s, " Variable declaration incomplete.");
            sn->token.str = s.cstr;
            return sn;   
        }
        t = tokens_next(tokens, &i);
        if(t->type != T_SC) {
            string s = string_from_lit("Expected SEMICOLON (;) in variable declaration, but instead got token ");
            string_concat(&s, token_print(t));
            string_pushs(&s, ".");
            syntax_free_node(sn);
            return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
        }
        SyntaxNode* vardecl = syntax_new_node(P_VAR_DECL, idtoken);
        syntax_node_add_child(vardecl, sn);
        *index = i;
        return vardecl;
    }
    string s = string_from_lit("Expected KEYWORD (var) for variable declaration, but instead got token ");
    string_concat(&s, token_print(t));
    string_pushs(&s, ".");
    return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
}

SyntaxNode* syntax_parse_statement(vector* tokens, int* index) {
    int i = *index;
    SyntaxNode* sn = syntax_parse_var_decl(tokens, &i);
    if(sn->type != P_ERROR) {
        *index = i;
        return sn;
    }
    syntax_free_node(sn);
    sn = syntax_parse_var_set(tokens, &i);
    if(sn->type != P_ERROR) {
        *index = i;
        return sn;
    }
    syntax_free_node(sn);
    sn = syntax_parse_print_decl(tokens, &i);
    if(sn->type != P_ERROR) {
        *index = i;
        return sn;
    }
    syntax_free_node(sn);
    Token* t = tokens_next(tokens, index);
    string s = string_from_lit("Expected a statement, none was found, but instead got token ");
    string_concat(&s, token_print(t));
    string_pushs(&s, ".");
    return syntax_new_error_node(s.cstr, t->pos, t->pos + strlen(t->str) - 1);
}

SyntaxNode* syntax_parse_statement_file(vector* tokens) {
    int i=0;
    static Token t0 = { T_EOF, "\0", -1 };
    SyntaxNode* daddy = syntax_new_node(P_LIST, t0);
    Token* t = vector_item(tokens, i);
    while(t->type != T_EOF) {
        SyntaxNode* st = syntax_parse_statement(tokens, &i);
        SyntaxNode* exp = vector_item(&(st->child_nodes), 0);
        printf("%s", syntax_print_tree(st).cstr);
        if(st->type == P_ERROR) {
            syntax_free_node(daddy);
            return st;
        }
        syntax_node_add_child(daddy, st);
        t = vector_item(tokens, i);
    }
    return daddy;
} 

SyntaxNode* syntax_parse_expr_list(vector* tokens) {
    int i = 0;
    static Token t0 = { T_NEWLINE, "\n", -1 };
    SyntaxNode* daddy = syntax_new_node(P_LIST, t0);
    SyntaxNode* sa;
    Token* t;
    do {
        Token* nt = vector_item(tokens, i);
        if(nt->type == T_NEWLINE) {
            t = nt; 
            i++;
            continue;
        } else if(nt->type == T_EOF)
            return daddy;
        sa = syntax_parse_expr(tokens, &i);
        if(sa->type == P_ERROR) {
            syntax_free_node(daddy);
            return sa;
        }
        syntax_node_add_child(daddy, sa);
        t = tokens_next(tokens, &i);
    } while(t->type == T_NEWLINE);
    return daddy;
}