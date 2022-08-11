#include "eval.hpp"
#include <stdlib.h>
#include <cstring>

string TypeInfo::print(void* value) {
    if(name == string("int8")) {
        short int c = *(char*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(name == string("int16")) {
        short int c = *(short int*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(name == string("int32")) {
        int c = *(short int*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(name == string("int64")) {
        long long c = *(long long*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(name == string("char")) {
        char c = *(char*)value;
        osstream ss;
        ss << c;
        return ss.str();
    }
    if(name == string("bool")) {
        char c = *(bool*)value;
        if(c)
            return "true";
        return "false";
    }
}

Variable Variable::copy(Variable v) {
    Variable v2;
    v2.typeinfo = v.typeinfo;
    v2.value = malloc(v.typeinfo->byteSize);
    memcpy(v2.value, v.value, v.typeinfo->byteSize);
    return v2;
}

void EvalContext::Scope::free() {
    for(pair<string, Variable>  v : variables)
        delete v.second.value;
    for(TypeInfo* t : types)
        delete t;
}

EvalContext::EvalContext() {
    pushScope();
    scopes[0].types.push_back(new TypeInfo("int8", 1));
    scopes[0].types.push_back(new TypeInfo("int16", 2));
    scopes[0].types.push_back(new TypeInfo("int32", 4));
    scopes[0].types.push_back(new TypeInfo("int64", 8));
    scopes[0].types.push_back(new TypeInfo("char", 1));
    scopes[0].types.push_back(new TypeInfo("bool", 1));
    scopes[0].types.push_back(new TypeInfo("string", sizeof(char*)));

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

void eval::statement(EvalContext& ctx, SyntaxNode* tree) {
    if(ctx.error != nullptr)
        return;
    if(tree->type == SyntaxNode::STM_PRINT) {
        OneChildSN* sn = (OneChildSN*)tree;
        Variable val = expression(ctx, sn->child);
        if(ctx.error != nullptr)
            return;
        cout << val.typeinfo->print(val.value) << endl;
    }
    else if(tree->type == SyntaxNode::STM_SCAN) {
        TextualSN* sn = (TextualSN*)tree;
        if(!ctx.hasVar(sn->text)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->text + string("' was not declared"), tree);
            return;
        }
        Variable& vn = ctx.getVar(sn->text);
        vn.value = vn.typeinfo->read();
    } else if(tree->type == SyntaxNode::VAR_DECL) {
        VarDeclSN* sn = (VarDeclSN*)tree;
        TextualSN* typexp = (TextualSN*)sn->typexp;
        TypeInfo* ti = ctx.getTypeInfo(typexp->text);
        if(ti == nullptr) {
            ctx.error = new EvalContext::Error(string("Type '") + typexp->text + string("' does not exist."), typexp);
            return;
        }
        for(SyntaxNode* tchild : sn->assignements) {
            if(tchild->type == SyntaxNode::EXPR_VAR) {
                TextualSN* schild = (TextualSN*)tchild;
                if(ctx.hasVarLastScope(schild->text)) {
                    ctx.error = new EvalContext::Error(string("Variable '") + schild->text + string("' was already declared in this scope"), tree);
                    return;
                }
                ctx.createVar(schild->text, ti);
            } else if(tchild->type == SyntaxNode::VAR_SET) {
                SetSN* schild = (SetSN*)tchild;
                if(ctx.hasVarLastScope(schild->name)) {
                    ctx.error = new EvalContext::Error(string("Variable '") + schild->name + string("' was already declared in this scope"), tchild);
                    return;
                }
                Variable expr = expression(ctx, schild->value);
                if(ctx.error != nullptr)
                    return;
                if(ti != expr.typeinfo) {
                    ctx.error = new EvalContext::Error(string("Variable declaration type '") + typexp->text + string("' and expression type do not match"), tree);
                    delete expr.value;
                    return;
                }
                ctx.createVar(schild->name, ti, expr.value);
            }
        }
    } else if(tree->type == SyntaxNode::VAR_SET) {
        SetSN* sn = (SetSN*)tree;
        if(!ctx.hasVar(sn->name)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->name + string("' was not declared or is out of scope"), sn);
            return;
        }
        Variable vtarget = ctx.getVar(sn->name);
        Variable expr = expression(ctx, sn->value);
        if(ctx.error != nullptr) {
            delete expr.value;
            return;
        }
        if(vtarget.typeinfo != expr.typeinfo) {
            delete expr.value;
            ctx.error = new EvalContext::Error(string("Cannot convert variable"), tree);
            return;
        }
        delete vtarget.value;
        vtarget.value = expr.value;
    } else if(tree->type == SyntaxNode::STM_LIST) {
        ListSN* sn = (ListSN*)tree;
        ctx.pushScope();
        for(SyntaxNode* c : sn->children) {
            statement(ctx, c);
            if(ctx.error != nullptr)
                return;
        }
        ctx.popScope();
    }
};

Variable eval::expression(EvalContext& ctx, SyntaxNode* tree) {
    Variable vres = { nullptr, nullptr };
    if(ctx.error != nullptr)
        return vres;
    if(tree->type == SyntaxNode::EXPR_VAR) {
        TextualSN* sn = (TextualSN*)tree;
        if(ctx.hasVar(sn->text)) {
            return Variable::copy(ctx.getVar(sn->text));
        }
        ctx.error = new EvalContext::Error(string("Variable '") + sn->text + "' was not declared", tree);
        return vres;
    }
    else if(tree->type == SyntaxNode::EXPR_NUM_LIT) {
        TextualSN* sn = (TextualSN*)tree;
        vres.typeinfo = ctx.getTypeInfo("int32");
        int val1, *val2 = new int;
        isstream(sn->text) >> val1;
        *val2 = val1;
        vres.value = val2;
        return vres;
    } else if(tree->type == SyntaxNode::EXPR_CHAR_LIT) {
        TextualSN* sn = (TextualSN*)tree;
        vres.typeinfo = ctx.getTypeInfo("char");
        char* c2 = new char;
        *c2 = sn->text[0];
        vres.value = c2;
        return vres;
    }
    else if(tree->type == SyntaxNode::EXPR_UN_OP) {
        UnaryOpSN* sn = (UnaryOpSN*)tree;
        Variable val = expression(ctx, sn->inside);
        if(val.typeinfo != ctx.getTypeInfo("int32")) {
            ctx.error = new EvalContext::Error(string("Cannot apply unary operator to value of incorrect type"), tree);
            return vres;
        }
        if(ctx.error != nullptr)
            return vres;
        int intinside = *(int*)val.value;
        switch(sn->opType) {
            case Token::PLUS:  intinside = intinside; break;
            case Token::MINUS: intinside = -intinside; break;
            default: throw string("UNEXPECTED OP TYPE ") + string(Token::PRINTS[sn->opType]);
        };
        *(int*)(val.value) = intinside;
        return val;
    } else if(tree->type == SyntaxNode::EXPR_BIN_OP) {
        BinaryOpSN* sn = (BinaryOpSN*)tree;
        Variable first = expression(ctx, sn->first);
        if(ctx.error != nullptr)
            return vres;
        Variable second = expression(ctx, sn->second);
        if(ctx.error != nullptr) {
            delete first.value;
            return vres;
        }
        if(first.typeinfo != ctx.getTypeInfo("int32") || second.typeinfo != ctx.getTypeInfo("int32")) {
            ctx.error = new EvalContext::Error("binary operations are only possible on int", tree);
            return vres;
        }
        int ifirst = *(int*)first.value, isecond = *(int*)second.value;
        int* ires = new int;
        switch(sn->opType) {
            case Token::PLUS:   *ires = ifirst + isecond; break;
            case Token::MINUS:  *ires = ifirst - isecond; break;
            case Token::STAR:   *ires = ifirst * isecond; break;
            case Token::DIV:    *ires = ifirst / isecond; break;
            default: throw string("UNEXPECTED OP TYPE ") + string(Token::PRINTS[sn->opType]);
        }
        vres.value = ires;
        vres.typeinfo = ctx.getTypeInfo("int32");
        delete first.value; delete second.value;
        return vres;
    }
    return vres;
}