#pragma once
#include "types.hpp"

struct EvalScope {
    map<string, Variable> variables;
    vector<TypeInfo*> types;
    void free();
};

struct EvalContext {

    struct Error {
        string msg;
        SyntaxNode* sn;
        Error(string msg, SyntaxNode* sn) : msg(msg), sn(sn) {};
    };

    vector<EvalScope> scopes;
    Error* error;
    EvalContext();
    ~EvalContext();

    bool hasVar(string varname);
    bool hasVarLastScope(string varname);
    bool isTypename(string typenm);  
    Variable& getVar(string varname);
    TypeInfo* getTypeInfo(string typenm);

    void createVar(string varname, TypeInfo* typeinfo);
    void createVar(string varname, TypeInfo* typeinfo, void* data);
    void pushScope();
    void popScope();
};

