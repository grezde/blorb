#include "typecheck.hpp"
#include "typelogic.hpp"

namespace typecheck {

    void varDeclStm(SemanticContext& ctx, SyntaxNode* tree) {
        VarDeclSN* sn = (VarDeclSN*)tree;
        TextualSN* typexp = (TextualSN*)sn->typexp;
        TypeInfo* ti = ctx.getType(typexp->text);
        if(ti == nullptr) {
            ctx.ers.push_back(SemanticContext::Error(string("Type '") + typexp->text + string("' does not exist."), typexp));
            return;
        }
        for(SyntaxNode* tchild : sn->assignements) {
            if(tchild->type == SyntaxNode::EXPR_VAR) {
                TextualSN* schild = (TextualSN*)tchild;
                if(ctx.hasVarLastScope(schild->text)) {
                    ctx.ers.push_back(SemanticContext::Error(string("Variable '") + schild->text + string("' was already declared in this scope"), tree));
                    continue;
                }
                ctx.createVar(ti, schild->text);
            } else if(tchild->type == SyntaxNode::VAR_SET) {
                SetSN* schild = (SetSN*)tchild;
                if(ctx.hasVarLastScope(schild->name)) {
                    ctx.ers.push_back(SemanticContext::Error(string("Variable '") + schild->name + string("' was already declared in this scope"), tchild));
                    continue;
                }
                TypeInfo* expr = expression(ctx, schild->value);
                if(!typelogic::canCoerce(expr, ti)) {
                    ctx.ers.push_back(SemanticContext::Error(string("Variable declaration type '") + typexp->text + string("' and expression type '") + expr->name + ("' do not match"), tree));
                    continue;
                }
                ctx.createVar(ti, schild->name);
            }
        }
    }

    void statements(SemanticContext& ctx, SyntaxNode* tree) {
        ListSN* sn = (ListSN*)tree;
        ctx.pushScope();
        for(SyntaxNode* child : sn->children)
            statement(ctx, child);
        ctx.popScope();
        return;
    }

    void varSetStm(SemanticContext& ctx, SyntaxNode* tree) {
        SetSN* sn = (SetSN*)tree;
        TypeInfo* ti = ctx.getVariable(sn->name);
        if(ti == nullptr) {
            ctx.ers.push_back(SemanticContext::Error(string("Variable '") + sn->name + string("' was not declared or is out of scope"), sn));
            return;
        }
        TypeInfo* expr = expression(ctx, sn->value);
        if(!typelogic::canCoerce(expr, ti))
            ctx.ers.push_back(SemanticContext::Error(string("Variable declaration type '") + ti->name + string("' and expression type '") + expr->name + ("' do not match"), tree));
    }

    void statement(SemanticContext& ctx, SyntaxNode* tree) {
        if(tree->type == SyntaxNode::STM_LIST)
            statements(ctx, tree);
        else if(tree->type == SyntaxNode::VAR_DECL)
            varDeclStm(ctx, tree);
        else if(tree->type == SyntaxNode::VAR_SET)
            varSetStm(ctx, tree);
        else if(tree->type == SyntaxNode::STM_EXPR) {
            OneChildSN* sn = (OneChildSN*)tree;
            expression(ctx, sn->child);
        }
        else if(tree->type == SyntaxNode::STM_PRINT) {
            ListSN* sn = (ListSN*)tree;
            for(SyntaxNode* child : sn->children) {
                TypeInfo* val = expression(ctx, child);
                if(!typelogic::canPrint(val))
                    ctx.ers.push_back(SemanticContext::Error(string("Cannot print variable of type ") + val->name, tree));
            }
        }
        else if(tree->type == SyntaxNode::STM_SCAN) {
            TextualSN* sn = (TextualSN*)tree;
            TypeInfo* v = ctx.getVariable(sn->text);
            if(v == nullptr)
                ctx.ers.push_back(SemanticContext::Error(string("Variable ") + sn->text + string(" is not defined or out of scope"), tree));
            else if(!typelogic::canScan(v))
                ctx.ers.push_back(SemanticContext::Error(string("Cannot scan variable of type ") + v->name, tree));
            
        } 
        else if(tree->type == SyntaxNode::IF_ELSE) {
            IfElseSN* sn = (IfElseSN*)tree;
            TypeInfo* expr = expression(ctx, sn->expr);
            if(expr != PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL])
                ctx.ers.push_back(SemanticContext::Error(string("If statement checks expression which is not boolean"), sn->expr));
            statement(ctx, sn->ifStm);
            if(sn->elseStm != nullptr)
                statement(ctx, sn->elseStm);
        }
        else if(tree->type == SyntaxNode::WHILE) {
            WhileSN* sn = (WhileSN*)tree;
            TypeInfo* expr = expression(ctx, sn->expr);
            if(expr != PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL])
                ctx.ers.push_back(SemanticContext::Error(string("While statement checks expression which is not boolean"), sn->expr));
            statement(ctx, sn->stm);
        }
    }

    TypeInfo* expression(SemanticContext& ctx, SyntaxNode* tree) {
        if(tree->type == SyntaxNode::EXPR_VAR) {
            TextualSN* sn = (TextualSN*)tree;
            TypeInfo* ti = ctx.getVariable(sn->text);
            if(ti != nullptr)
                return ti->getRef();
            ctx.ers.push_back(SemanticContext::Error(string("Variable ") + sn->text + string(" is not defined or out of scope"), tree));
            return nullptr;
        }
        else if(tree->type == SyntaxNode::EXPR_NUM_LIT)
            return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::INT32];
        else if(tree->type == SyntaxNode::EXPR_CHAR_LIT)
            return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::CHAR];
        else if(tree->type == SyntaxNode::EXPR_STRING_LIT)
            return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::STRING];
        else if(tree->type == SyntaxNode::EXPR_UN_OP) {
            UnaryOpSN* sn = (UnaryOpSN*)tree;
            TypeInfo* val = expression(ctx, sn->inside);
            if(val == nullptr)
                return nullptr;
            TypeInfo* ret = nullptr;
            if(UnaryOpSN::isInteractive(sn->opType))
                ret = typelogic::canUnaryOpInt(sn->opType, val);
            else
                ret = typelogic::canUnaryOp(sn->opType, val);
            if(ret != nullptr)
                return val;
            ctx.ers.push_back(SemanticContext::Error(string("Cannot apply unary operator to variable of type ") + val->name, tree));
            return nullptr;
        }
        else if(tree->type == SyntaxNode::EXPR_BIN_OP) {
            BinaryOpSN* sn = (BinaryOpSN*)tree;
            TypeInfo* first = expression(ctx, sn->first);
            TypeInfo* second = expression(ctx, sn->second);
            if(first == nullptr || second == nullptr)
                return nullptr;
            TypeInfo* ret = nullptr;
            if(BinaryOpSN::isInteractive(sn->opType))
                ret = typelogic::canBinaryOpInt(sn->opType, first, second);
            else
                ret = typelogic::canBinaryOp(sn->opType, first, second);
            if(ret != nullptr)
                return ret;
            ctx.ers.push_back(SemanticContext::Error(string("Cannot apply binary operator"), tree));
            return nullptr;
        }
    }

}