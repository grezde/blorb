#include "syntax.hpp"

struct TypeInfo {
    string name;
    int byteSize;
    // there will be a pointer to a tree for non-primitive types
    TypeInfo(string name, int byteSize) : name(name), byteSize(byteSize) {};
    string print(void* value);
    void* read() { return new int; };
};

struct Variable {
    void* value;
    TypeInfo* typeinfo;
    static Variable copy(Variable v);
};

struct EvalContext {

    struct Scope {
        map<string, Variable> variables;
        vector<TypeInfo*> types;
        void free();
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
    bool isTypename(string typenm);  
    Variable& getVar(string varname);
    TypeInfo* getTypeInfo(string typenm);

    inline void createVar(string varname, TypeInfo* typeinfo);
    inline void createVar(string varname, TypeInfo* typeinfo, void* data);
    inline void pushScope();
    inline void popScope();
};

namespace eval {

    Variable expression(EvalContext& ctx, SyntaxNode* tree);
    void statement(EvalContext& ctx, SyntaxNode* tree);
}


