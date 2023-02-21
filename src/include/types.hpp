#pragma once
#include "syntax.hpp"

struct PrimitiveTypeInfo;
struct RefTypeInfo;
struct Variable;

struct TypeInfo {
    enum Type {
        PRIMITIVE, ALIAS,
        STRUCTURE, NTUPLE,
        FUNCTION, M_REF, M_ARRAY
    };
    Type type;
    string name;
    int byteSize;
    RefTypeInfo* ref;
    TypeInfo(Type type, string name, int byteSize) : type(type), name(name), byteSize(byteSize), ref(nullptr) {};
    ~TypeInfo();
    PrimitiveTypeInfo* asPrimitive();
    virtual RefTypeInfo* asRef();
    virtual RefTypeInfo* getRef();
    virtual void* newInstance();
    virtual void deleteInstance(void* v);
    virtual void* copyInstance(void* v);
    virtual bool isEquivalent(TypeInfo* tp);
};

struct RefTypeInfo : TypeInfo {
    TypeInfo* atype;
    RefTypeInfo(TypeInfo* atype) : TypeInfo(TypeInfo::M_REF, atype->name+string("&"), sizeof(void*)), atype(atype) {};
    void deleteInstance(void* v) {};
    void* newInstance() { return nullptr; };
    void* copyInstance(void* v) { return v; };
};

struct ArrayTypeInfo : TypeInfo {
    TypeInfo* atype;
    ListTypeInfo(TypeInfo* atype) : TypeInfo(TypeInfo::M_ARRAY, atype->name+string("[]"), sizeof(void*)), atype(atype) {};
    void deleteInstance(void* v) {};
    void* newInstance() { return nullptr; };
    void* copyInstance(void* v) { return v; };
};

struct PrimitiveTypeInfo : TypeInfo {
    enum PrimitiveType {
        INT8,  INT16,  INT32,  INT64,
        UINT8, UINT16, UINT32, UINT64,
        BOOL, CHAR, STRING,
        FLOAT32, FLOAT64
    };
    static array<PrimitiveTypeInfo*, 13> primitivePtrs;
    static void genPrimitives();
    
    PrimitiveType ptype;
    PrimitiveTypeInfo(PrimitiveType pt, string name, int byteSize) : TypeInfo(PRIMITIVE, name, byteSize), ptype(pt) {};
    
    void* newInstance();
    void deleteInstance(void* v);
    void* copyInstance(void* v);

    static bool canCoerce(PrimitiveTypeInfo* toType, PrimitiveTypeInfo* fromType);
    static Variable tryCoerce(Variable v, PrimitiveTypeInfo* newtypeinfo);
    static TypeInfo* canUnaryOp(Token::Type opType, PrimitiveTypeInfo* type);
    static Variable tryUnaryOp(Token::Type opType, Variable v);
    static TypeInfo* canBinaryOp(Token::Type opType, PrimitiveTypeInfo* atype, PrimitiveTypeInfo* btype);
    static Variable tryBinaryOp(Token::Type opType, Variable a, Variable b);
    static bool canScan(PrimitiveTypeInfo* type);
    void* read(std::istream& is);
    static bool canPrint(PrimitiveTypeInfo* type);
    string print(void* value);

    long long toLL(void* v);
    void fromLL(long long value, void* ptr);
};

struct SemanticScope {
    vector<TypeInfo*> types;
    map<string, TypeInfo*> variables;
};

struct SemanticContext {

    struct Error {
        string msg;
        SyntaxNode* node;
        Error(string msg, SyntaxNode* node) : msg(msg), node(node) {};
    };

    vector<SemanticScope> scopes;
    vector<Error> ers;

    SemanticContext();
    TypeInfo* getVariable(string name);
    TypeInfo* getType(string name);
    TypeInfo* getTypeTree(SyntaxNode* name);
    bool hasVarLastScope(string name);
    void createVar(TypeInfo* ti, string name);
    void pushScope();
    void popScope();
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
    Variable ref();
    Variable unref();
};