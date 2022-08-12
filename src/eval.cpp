#include "eval.hpp"

void eval::varDeclStm(EvalContext& ctx, SyntaxNode* tree) {
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
                Variable aux = PrimitiveTypeInfo::tryCoerce(expr, ti->asPrimitive());
                expr.free();
                expr = aux;
            }
            if(expr.typeinfo == nullptr) {
                expr.free();
                ctx.error = new EvalContext::Error(string("Variable declaration type '") + typexp->text + string("' and expression type do not match"), tree);
                return;
            }
            ctx.createVar(schild->name, ti, expr.value);
        }
    }
}

void eval::varSetStm(EvalContext& ctx, SyntaxNode* tree) {
    SetSN* sn = (SetSN*)tree;
    if(!ctx.hasVar(sn->name)) {
        ctx.error = new EvalContext::Error(string("Variable '") + sn->name + string("' was not declared or is out of scope"), sn);
        return;
    }
    Variable& vtarget = ctx.getVar(sn->name);
    Variable expr = expression(ctx, sn->value);
    if(ctx.error != nullptr) {
        expr.free();
        return;
    }
    if(vtarget.typeinfo != expr.typeinfo) {
        Variable aux = PrimitiveTypeInfo::tryCoerce(expr, vtarget.typeinfo->asPrimitive());
        expr.free();
        expr = aux;
    }
    if(expr.typeinfo == nullptr) {
        ctx.error = new EvalContext::Error(string("Cannot convert variable"), tree);
        return;
    }
    vtarget.free();
    vtarget.value = expr.value;
}

void eval::statement(EvalContext& ctx, SyntaxNode* tree) {
    if(ctx.error != nullptr)
        return;
    if(tree->type == SyntaxNode::STM_PRINT) {
        OneChildSN* sn = (OneChildSN*)tree;
        Variable val = expression(ctx, sn->child);
        if(ctx.error != nullptr)
            return;
        cout << val.typeinfo->asPrimitive()->print(val.value) << endl;
    }
    else if(tree->type == SyntaxNode::STM_SCAN) {
        TextualSN* sn = (TextualSN*)tree;
        if(!ctx.hasVar(sn->text)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->text + string("' was not declared"), tree);
            return;
        }
        Variable& vn = ctx.getVar(sn->text);
        vn.value = vn.typeinfo->asPrimitive()->read(std::cin);
    } 
    else if(tree->type == SyntaxNode::VAR_DECL)
        varDeclStm(ctx, tree);
    else if(tree->type == SyntaxNode::VAR_SET)
        varSetStm(ctx, tree);
    else if(tree->type == SyntaxNode::STM_LIST) {
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
    Variable vres;
    if(ctx.error != nullptr)
        return vres;
    if(tree->type == SyntaxNode::EXPR_VAR) {
        TextualSN* sn = (TextualSN*)tree;
        if(ctx.hasVar(sn->text)) {
            return ctx.getVar(sn->text).copy();
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
    } 
    else if(tree->type == SyntaxNode::EXPR_CHAR_LIT) {
        TextualSN* sn = (TextualSN*)tree;
        vres.typeinfo = ctx.getTypeInfo("char");
        char* c2 = new char;
        *c2 = sn->text[0];
        vres.value = c2;
        return vres;
    } else if(tree->type == SyntaxNode::EXPR_STRING_LIT) {
        TextualSN* sn = (TextualSN*)tree;
        vres.typeinfo = ctx.getTypeInfo("string");
        vres.value = new string(sn->text);
        return vres;
    }
    else if(tree->type == SyntaxNode::EXPR_UN_OP) {
        UnaryOpSN* sn = (UnaryOpSN*)tree;
        Variable val = expression(ctx, sn->inside);
        if(ctx.error != nullptr)
            return vres;
        Variable res = PrimitiveTypeInfo::tryUnaryOp(sn->opType, val);
        val.free();
        if(res.value == nullptr) {
            ctx.error = new EvalContext::Error(string("Cannot apply unary operator to value of incorrect type"), tree);
            return vres;
        }
        return res;
    } 
    else if(tree->type == SyntaxNode::EXPR_BIN_OP) {
        BinaryOpSN* sn = (BinaryOpSN*)tree;
        Variable first = expression(ctx, sn->first);
        if(ctx.error != nullptr)
            return vres;
        Variable second = expression(ctx, sn->second);
        if(ctx.error != nullptr) {
            first.free();
            return vres;
        }
        Variable res = PrimitiveTypeInfo::tryBinaryOp(sn->opType, first, second);
        first.free();
        second.free();
        if(res.value == nullptr) {
            ctx.error = new EvalContext::Error(string("Cannot apply binary operator to value of incorrect type"), tree);
            return vres;
        }
        return res;
    }
    return vres;
}