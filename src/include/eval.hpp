#include "syntax.hpp"

struct EvalContext {

    struct Scope {
        map<string, int> variables;
    };

    struct Error {
        string msg;
        SyntaxNode* sn;
        Error(string msg, SyntaxNode* sn) : msg(msg), sn(sn) {};
    };

    vector<Scope> scopes;
    Error* error;
    EvalContext();
    ~EvalContext();
    bool hasVar(string varname);
    inline bool hasVarLastScope(string varname);
    int& getVar(string varname);
    inline void createVar(string varname, int value);
    inline void pushScope();
    inline void popScope();
};

int evalExpression(EvalContext& ctx, SyntaxNode* tree);
void evalStatement(EvalContext& ctx, SyntaxNode* tree);

