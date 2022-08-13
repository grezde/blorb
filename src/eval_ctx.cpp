#include "eval_ctx.hpp"
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

PrimitiveTypeInfo* TypeInfo::asPrimitive() {
    if(type != PRIMITIVE)
        throw "Tried to call asPrimitive() on non primitive typeinfo";
    return (PrimitiveTypeInfo*)this; 
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

string PrimitiveTypeInfo::print(void* value) {
    if(ptype == INT8) {
        short int c = *(char*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(ptype == INT16) {
        short int c = *(short int*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(ptype == INT32) {
        int c = *(short int*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(ptype == INT64) {
        long long c = *(long long*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(ptype == CHAR) {
        char c = *(char*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(ptype == STRING)
        return *(string*)value;
    if(ptype == BOOL) {
        char c = *(bool*)value;
        if(c)
            return "true";
        return "false";
    }
    throw "TYPE WRITING NOT IMPLEMENTED YET";
}

void* PrimitiveTypeInfo::read(std::istream& is) {
    if(ptype == INT8) {
        auto cv = new char;
        short int c;
        is >> c;
        *cv = c;
        return cv;
    }
    else if(ptype == INT16) {
        auto c = new short int;
        is >> *c;
        return c;
    }
    else if(ptype == INT32) {
        auto c = new int;
        is >> *c;
        return c;
    }
    else if(ptype == INT64) {
        auto c = new long long;
        is >> *c;
        return c;
    } 
    else if(ptype == CHAR) {
        auto c = new char;
        is >> *c;
        return c;
    }
    else if(ptype == STRING) {
        auto s = new string;
        is >> *s;
        return s;
    }
    else if(ptype == BOOL) {
        auto b = new bool;
        is >> *b;
        return b;
    }
    throw "TYPE READING NOT IMPLEMENTED YET";
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

Variable PrimitiveTypeInfo::tryCoerce(Variable v, PrimitiveTypeInfo* newtypeinfo) {
    if(v.typeinfo->asPrimitive()->ptype < 4 && newtypeinfo->ptype < 4) {
        Variable nv(newtypeinfo);
        long long value = v.typeinfo->asPrimitive()->toLL(v.value);
        newtypeinfo->fromLL(value, nv.value);
        return nv;
    }
    if(v.typeinfo->asPrimitive()->ptype == CHAR && newtypeinfo->ptype == STRING)
        return Variable(newtypeinfo, new string(1, *(char*)v.value));
    return Variable();
}

Variable PrimitiveTypeInfo::tryUnaryOp(Token::Type opType, Variable v) {
    auto pt = v.typeinfo->asPrimitive()->ptype;
    if(pt == BOOL && opType == Token::NEGATE) {
        Variable vres = v.copy();
        bool b = *(bool*)v.value;
        b = !b;
        vres.copyFrom(&b);
        return vres;
    }
    if(pt >= 4)
        return Variable();
    if(opType == Token::PLUS)
        return v.copy();
    if(opType == Token::MINUS) {
        Variable vres = v.copy();
        if(pt == INT8) {
            auto c = *(char*)v.value;
            c = -c;
            vres.copyFrom(&c);
        }
        else if(pt == INT16) {
            auto c = *(short int*)v.value;
            c = -c;
            vres.copyFrom(&c);
        }
        else if(pt == INT32) {
            auto c = *(int*)v.value;
            c = -c;
            vres.copyFrom(&c);
        }
        else if(pt == INT64) {
            auto c = *(long long*)v.value;
            c = -c;
            vres.copyFrom(&c);
        }
        return vres;  
    }
    return Variable();
}

Variable PrimitiveTypeInfo::tryBinaryOp(Token::Type opType, Variable a, Variable b, TypeInfo* boolType) {
    PrimitiveTypeInfo* ta = a.typeinfo->asPrimitive();
    PrimitiveTypeInfo* tb = b.typeinfo->asPrimitive();
    if(opType == Token::PLUS) {
        if(ta->ptype == CHAR && tb->ptype == STRING) {
            string v = string(*(string*)b.value);
            char c = *(char*)a.value;
            return Variable(tb, new string(c+v));
        }
        if(ta->ptype == STRING && tb->ptype == CHAR) {
            string v = string(*(string*)a.value);
            char c = *(char*)b.value;
            return Variable(ta, new string(v+c));
        }
        if(ta->ptype == STRING && tb->ptype == STRING) {
            string v = string(*(string*)a.value);
            string w = string(*(string*)b.value);
            return Variable(ta, new string(v+w));
        }
    }
    if(opType == Token::EQEQ || opType == Token::NEQ) {
        if(ta->ptype == STRING && tb->ptype == STRING) {
            const string v = *(string*)a.value;
            const string w = *(string*)b.value;
            if(opType == Token::EQEQ)
                return Variable(boolType, new bool(v == w));
            else
                return Variable(boolType, new bool(v != w));
        }
        if(ta->ptype == BOOL && tb->ptype == BOOL) {
            auto v = *(bool*)a.value;
            auto w = *(bool*)b.value;
            if(opType == Token::EQEQ)
                return Variable(boolType, new bool(v == w));
            else
                return Variable(boolType, new bool(v != w));
        }
        if(ta->ptype == CHAR && tb->ptype == CHAR) {
            auto v = *(char*)a.value;
            auto w = *(char*)b.value;
            if(opType == Token::EQEQ)
                return Variable(boolType, new bool(v == w));
            else
                return Variable(boolType, new bool(v != w));
        }
    }
    if(ta->ptype == BOOL && tb->ptype == BOOL) {
        auto v = *(bool*)a.value;
        auto w = *(bool*)b.value;
        if(opType == Token::ANDAND)
            return Variable(boolType, new bool(v && w));
        if(opType == Token::OROR)
            return Variable(boolType, new bool(v || w));
    }
    if(ta->ptype < 4 && tb->ptype < 4) {
        long long ca = ta->toLL(a.value), cb = tb->toLL(b.value), cc;
        if(BinaryOpSN::isBinaryOp(opType)) {
            Variable v(boolType);
            bool res;
            switch(opType) {
                case Token::EQEQ: res = ca == cb; break;
                case Token::NEQ:  res = ca != cb; break;
                case Token::GT:   res = ca >  cb; break;
                case Token::LT:   res = ca <  cb; break;
                case Token::GEQ:  res = ca >= cb; break;
                case Token::LEQ:  res = ca <= cb; break;
                default:          v.free();       return Variable();
            }
            v.copyFrom(&res);
            return v;
        }
        PrimitiveTypeInfo* tc = ta->ptype > tb->ptype ? ta : tb;
        Variable v(tc);
        if(opType == Token::PLUS)
            cc = ca+cb;
        else if(opType == Token::MINUS)
            cc = ca-cb;
        else if(opType == Token::STAR)
            cc = ca*cb;
        else if(opType == Token::DIV)
            cc = ca/cb;
        tc->fromLL(cc, v.value);
        return v;        
    }
    return Variable();
}

Variable::Variable(TypeInfo* typeinfo) {
    this->typeinfo = typeinfo;
    this->value = typeinfo->newInstance();
}

Variable Variable::copy() {
    return Variable(typeinfo, typeinfo->copyInstance(value));
}

void Variable::copyFrom(void* value) {
    memcpy(this->value, value, typeinfo->byteSize);
}

void EvalContext::Scope::free() {
    for(pair<string, Variable>  v : variables)
        v.second.free();
    for(TypeInfo* t : types)
        delete t;
}

EvalContext::EvalContext() {
    pushScope();
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::INT8, "int8", 1));
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::INT16, "int16", 2));
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::INT32, "int32", 4));
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::INT64, "int64", 8));
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::CHAR, "char", 1));
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::BOOL, "bool", 1));
    scopes[0].types.push_back(new PrimitiveTypeInfo(PrimitiveTypeInfo::STRING, "string", sizeof(string)));
}

EvalContext::~EvalContext() {
    if(error != nullptr)
        delete error;
}

TypeInfo* EvalContext::getTypeInfo(string typenm) {
    for(int i=scopes.size()-1; i>=0; i--)
        for(TypeInfo* typeinfo : scopes[i].types) {
            if(typenm == typeinfo->name)
                return typeinfo;
        }
    return nullptr;
}

bool EvalContext::hasVar(string varname) {
    for(int i=scopes.size()-1; i>=0; i--)
        if(scopes[i].variables.count(varname))
            return true;
    return false;
}

bool EvalContext::hasVarLastScope(string varname) {
    return scopes[scopes.size()-1].variables.count(varname);
}

Variable& EvalContext::getVar(string varname) {
    for(int i=scopes.size()-1; i>=0; i--)
        if(scopes[i].variables.count(varname))
            return scopes[i].variables[varname];
    throw "CHECK THE VAR FIRST";
}

void EvalContext::createVar(string varname, TypeInfo* typeinfo) {
    Variable v;
    v.typeinfo = typeinfo;
    v.value = malloc(v.typeinfo->byteSize);
    memset(v.value, 0, typeinfo->byteSize);
    scopes[scopes.size()-1].variables[varname] = v;
}

void EvalContext::createVar(string varname, TypeInfo* typeinfo, void* data) {
    Variable v;
    v.typeinfo = typeinfo;
    v.value = data;
    scopes[scopes.size()-1].variables[varname] = v;
}

void EvalContext::pushScope() {
    scopes.push_back(Scope());
}

void EvalContext::popScope() {
    scopes[scopes.size()-1].free();
    scopes.pop_back();
}
