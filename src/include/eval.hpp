#include "syntax.hpp"

struct EvalContext {
    struct Error {
        string msg;
        SyntaxNode* sn;
        Error(string msg, SyntaxNode* sn) : msg(msg), sn(sn) {};
    };

    map<string, int> variables;
    Error* error;
    ~EvalContext();
};

int evalExpression(EvalContext& ctx, SyntaxNode* tree);
void evalStatement(EvalContext& ctx, SyntaxNode* tree);

