#include "syntax.hpp"

struct PrimitiveTypeInfo;

struct TypeInfo {
    enum Type {
        PRIMITIVE, ALIAS,
        STRUCTURE
    };
    Type type;
    string name;
    int byteSize;
    TypeInfo(Type type, string name, int byteSize) : type(type), name(name), byteSize(byteSize) {};
    PrimitiveTypeInfo* asPrimitive();
    virtual void* newInstance();
    virtual void deleteInstance(void* v);
    virtual void* copyInstance(void* v);
};

struct Variable {
    void* value;
    TypeInfo* typeinfo;
    Variable() { typeinfo = nullptr; value = nullptr; };
    Variable(TypeInfo* typeinfo);
    Variable(TypeInfo* typeinfo, void* value) : typeinfo(typeinfo), value(value) {};
    void copyFrom(void* value);
    inline void free() { typeinfo->deleteInstance(value); };
    Variable copy();
};

struct PrimitiveTypeInfo : TypeInfo {
    enum PrimitiveType {
        INT8,  INT16,  INT32,  INT64,
        UINT8, UINT16, UINT32, UINT64,
        BOOL, CHAR, STRING,
        FLOAT32, FLOAT64
    };
    PrimitiveType ptype;
    PrimitiveTypeInfo(PrimitiveType pt, string name, int byteSize) : TypeInfo(PRIMITIVE, name, byteSize), ptype(pt) {};
    void* newInstance();
    void deleteInstance(void* v);
    void* copyInstance(void* v);

    string print(void* value);
    void* read(std::istream& is);
    long long toLL(void* v);
    void fromLL(long long value, void* ptr);
    static Variable tryCoerce(Variable v, PrimitiveTypeInfo* newtypeinfo);
    static Variable tryUnaryOp(Token::Type opType, Variable v);
    static Variable tryBinaryOp(Token::Type opType, Variable a, Variable b, TypeInfo* booltype);
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
    bool hasVarLastScope(string varname);
    bool isTypename(string typenm);  
    Variable& getVar(string varname);
    TypeInfo* getTypeInfo(string typenm);

    void createVar(string varname, TypeInfo* typeinfo);
    void createVar(string varname, TypeInfo* typeinfo, void* data);
    void pushScope();
    void popScope();
};

