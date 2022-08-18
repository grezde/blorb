#include "eval_ctx.hpp"
#include <stdlib.h>

void EvalScope::free() {
    for(pair<string, Variable>  v : variables)
        v.second.free();
    for(TypeInfo* t : types)
        delete t;
}

EvalContext::EvalContext() {
    pushScope();
    PrimitiveTypeInfo::genPrimitives();
    for(PrimitiveTypeInfo* pti : PrimitiveTypeInfo::primitivePtrs)
        scopes[0].types.push_back(pti);
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
    scopes[scopes.size()-1].variables[varname] = Variable(typeinfo);
}

void EvalContext::createVar(string varname, TypeInfo* typeinfo, void* data) {
    Variable v;
    v.typeinfo = typeinfo;
    v.value = data;
    scopes[scopes.size()-1].variables[varname] = v;
}

void EvalContext::pushScope() {
    scopes.push_back(EvalScope());
}

void EvalContext::popScope() {
    if(scopes.size() < 2)
        throw "Scope is already empty";
    scopes[scopes.size()-1].free();
    scopes.pop_back();
}
