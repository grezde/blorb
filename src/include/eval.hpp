#pragma once
#include "eval_ctx.hpp"

namespace eval {

    Variable expression(EvalContext& ctx, SyntaxNode* tree);

    void ifElseStm(EvalContext& ctx, SyntaxNode* tree);
    void varDeclStm(EvalContext& ctx, SyntaxNode* tree);
    void varSetStm(EvalContext& ctx, SyntaxNode* tree);
    void whileStm(EvalContext& ctx, SyntaxNode* tree);
    void statement(EvalContext& ctx, SyntaxNode* tree);

}

