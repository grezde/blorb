#pragma once
#include "types.hpp"

namespace typecheck {

    void statement(SemanticContext& ctx, SyntaxNode* tree);
    TypeInfo* expression(SemanticContext& ctx, SyntaxNode* tree);

}