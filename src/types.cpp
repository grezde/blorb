#include "types.hpp"
#include <stdlib.h>
#include <cstring>

void* TypeInfo::newInstance() {
    void* v = malloc(byteSize);
    memset(v, 0, byteSize);
    return v;
}

void TypeInfo::deleteInstance(void* v) {
    free(v);
}

void* TypeInfo::copyInstance(void* v) {
    void* v2 = malloc(byteSize);
    memcpy(v2, v, byteSize);
    return v2;
}

bool TypeInfo::isEquivalent(TypeInfo* t) {
    if(t->type == M_REF)
        return t->isEquivalent(this);
    return this == t;
}

PrimitiveTypeInfo* TypeInfo::asPrimitive() {
    if(type != PRIMITIVE)
        throw string("Tried to call asPrimitive() on non primitive typeinfo");
    return (PrimitiveTypeInfo*)this; 
}

RefTypeInfo* TypeInfo::asRef() {
    if(type != M_REF)
        throw string("Tried to call asRef() on non refference typeinfo");
    return (RefTypeInfo*)this; 
}

RefTypeInfo* TypeInfo::getRef() {
    if(ref == nullptr)
        ref = new RefTypeInfo(this);
    return ref;
}

TypeInfo::~TypeInfo() {
    if(ref != nullptr)
        delete ref;
}

void* PrimitiveTypeInfo::newInstance() {
    if(ptype == STRING)
        return new string;
    void* v = malloc(byteSize);
    memset(v, 0, byteSize);
    return v;
}

void PrimitiveTypeInfo::deleteInstance(void* v) {
    if(ptype == STRING)
        delete (string*)v;
    else
        free(v);
}

void* PrimitiveTypeInfo::copyInstance(void* v) {
    if(ptype == STRING)
        return new string(*(string*)v);
    void* v2 = malloc(byteSize);
    memcpy(v2, v, byteSize);
    return v2;
}

SemanticContext::SemanticContext() {
    PrimitiveTypeInfo::genPrimitives();
    pushScope();
    for(PrimitiveTypeInfo* pt : PrimitiveTypeInfo::primitivePtrs)
        scopes[0].types.push_back(pt);
}

TypeInfo* SemanticContext::getVariable(string name) {
    for(int i=scopes.size()-1; i>=0; i--)
        if(scopes[i].variables.count(name))
            return scopes[i].variables[name];
    return nullptr;
}

bool SemanticContext::hasVarLastScope(string name) {
    return scopes[scopes.size()-1].variables.count(name);
}

void SemanticContext::createVar(TypeInfo* ti, string name) {
    scopes[scopes.size()-1].variables[name] = ti;
}

TypeInfo* SemanticContext::getType(string name) {
    for(int i=scopes.size()-1; i>=0; i--)
        for(TypeInfo* ti : scopes[i].types)
            if(ti->name == name)
                return ti;
    return nullptr;
}

TypeInfo* SemanticContext::getTypeTree(SyntaxNode* sn) {
    if(sn->type == SyntaxNode::TYPE_NAME)
        return getType((TextualSN*)a->text);
    if(sn->type == SyntaxNode::TYPE_UN_OP) {
        UnaryTypeOpSN* node = (UnaryTypeOpSN*)sn;
        TypeInfo* tt = getTypeTree(node->inside);
        if(tt == nullptr)
            return nullptr;
        return new ArrayTypeInfo(tt);
    } 
}

void SemanticContext::pushScope() {
    scopes.push_back(SemanticScope());
}

void SemanticContext::popScope() {
    scopes.pop_back();
}

long long PrimitiveTypeInfo::toLL(void* v) {
    long long l;
    if(ptype == INT8) {
        auto c = *(char*)v;
        l = c;
    }
    else if(ptype == INT16) {
        auto c = *(short int*)v;
        l = c;
    }
    else if(ptype == INT32) {
        auto c = *(int*)v;
        l = c;
    }
    else if(ptype == INT64) {
        auto c = *(long long*)v;
        l = c;
    }
    return l;
}

void PrimitiveTypeInfo::fromLL(long long value, void* ptr) {
    if(ptype == INT8) {
        char c = value;
        memcpy(ptr, &c, byteSize);
    }
    else if(ptype == INT16) {
        short int c = value;
        memcpy(ptr, &c, byteSize);
    }
    else if(ptype == INT32) {
        int c = value;
        memcpy(ptr, &c, byteSize);
    }
    else if(ptype == INT64) {
        long long c = value;
        memcpy(ptr, &c, byteSize);
    }
}

array<PrimitiveTypeInfo*, 13> PrimitiveTypeInfo::primitivePtrs = {
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, 
    nullptr, nullptr
};

void PrimitiveTypeInfo::genPrimitives() {
    static bool gened = false;
    if(!gened) {
        primitivePtrs = {
            new PrimitiveTypeInfo(INT8,    "int8",    1),
            new PrimitiveTypeInfo(INT16,   "int16",   2),
            new PrimitiveTypeInfo(INT32,   "int32",   4),
            new PrimitiveTypeInfo(INT64,   "int64",   8),
            new PrimitiveTypeInfo(UINT8,   "uint8",   1),
            new PrimitiveTypeInfo(UINT16,  "uint16",  2),
            new PrimitiveTypeInfo(UINT32,  "uint32",  4),
            new PrimitiveTypeInfo(UINT64,  "uint64",  8),
            new PrimitiveTypeInfo(BOOL,    "bool",    1),
            new PrimitiveTypeInfo(CHAR,    "char",    1),
            new PrimitiveTypeInfo(STRING,  "string",  1),
            new PrimitiveTypeInfo(FLOAT32, "float32", 4),
            new PrimitiveTypeInfo(FLOAT64, "float64", 8)
        };
        gened = true;
    }
}

Variable::Variable(TypeInfo* typeinfo) {
    this->typeinfo = typeinfo;
    this->value = typeinfo->newInstance();
}

Variable Variable::copy() {
    return Variable(typeinfo, typeinfo->copyInstance(value));
}

Variable Variable::ref() {
    return Variable(typeinfo->getRef(), &value);
}

Variable Variable::unref() {
    return Variable(typeinfo->asRef()->atype, *(void**)value);
}

void Variable::copyFrom(void* value) {
    memcpy(this->value, value, typeinfo->byteSize);
}