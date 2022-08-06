#include "parser.h"

typedef struct {
    const char* name;
    int value;
} Variable;

typedef struct {
    vector vars;
} SemContext;

Variable* eval_var_from_context(SemContext* ctx, const char* name);
void eval_context_set_var(SemContext* ctx, const char* name, int value);
void eval_context_set_error(SemContext* ctx, const char* msg);
const char* eval_context_get_error(SemContext* ctx);

void eval_statement_list(SyntaxNode* sn);

int eval_expr(SyntaxNode* sn, SemContext* ctx);
vector eval_expr_list(SyntaxNode* sn);
