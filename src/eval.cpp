#include "eval.hpp"
#include "typelogic.hpp"

namespace eval {

    void varDeclStm(EvalContext& ctx, SyntaxNode* tree) {
        VarDeclSN* sn = (VarDeclSN*)tree;
        TextualSN* typexp = (TextualSN*)sn->typexp;
        TypeInfo* ti = ctx.getTypeInfo(typexp->text);
        for(SyntaxNode* tchild : sn->assignements) {
            if(tchild->type == SyntaxNode::EXPR_VAR) {
                TextualSN* schild = (TextualSN*)tchild;
                ctx.createVar(schild->text, ti);
            } else if(tchild->type == SyntaxNode::VAR_SET) {
                SetSN* schild = (SetSN*)tchild;
                Variable expr = expression(ctx, schild->value);
                if(ctx.error != nullptr)
                    return;
                Variable v = typelogic::coerce(expr, ti);
                expr.free();
                ctx.createVar(schild->name, ti, v.value);
            }
        }
    }

    void varSetStm(EvalContext& ctx, SyntaxNode* tree) {
        SetSN* sn = (SetSN*)tree;
        Variable& vtarget = ctx.getVar(sn->name);
        Variable expr = expression(ctx, sn->value);
        if(ctx.error != nullptr)
            return;
        vtarget.free();
        vtarget = typelogic::coerce(expr, vtarget.typeinfo);
        expr.free();
    }

    void ifElseStm(EvalContext& ctx, SyntaxNode* tree) {
        IfElseSN* sn = (IfElseSN*)tree;
        Variable res = expression(ctx, sn->expr);
        if(ctx.error != nullptr)
            return;
        bool rest = *(bool*)res.value;
        res.free();
        if(rest)
            statement(ctx, sn->ifStm);
        else if(sn->elseStm != nullptr)
            statement(ctx, sn->elseStm);
    }

    void whileStm(EvalContext& ctx, SyntaxNode* tree) {
        WhileSN* sn = (WhileSN*)tree;
        Variable res = expression(ctx, sn->expr);
        if(ctx.error != nullptr)
            return;
        bool rest = *(bool*)res.value;
        res.free();
        while(rest) {
            statement(ctx, sn->stm);
            if(ctx.error != nullptr)
                return;
            res = expression(ctx, sn->expr);
            if(ctx.error != nullptr)
                return;
            rest = *(bool*)res.value;
            res.free();
        }
    }

    void statement(EvalContext& ctx, SyntaxNode* tree) {
        //cout << "STATEMENT" << endl << tree->toString() << endl;
        if(tree->type == SyntaxNode::STM_PRINT) {
            ListSN* sn = (ListSN*)tree;
            for(SyntaxNode* child : sn->children) {
                Variable val = expression(ctx, child);
                if(ctx.error != nullptr)
                    return;
                cout << typelogic::print(val);
            }
        }
        else if(tree->type == SyntaxNode::STM_SCAN) {
            TextualSN* sn = (TextualSN*)tree;
            Variable& vn = ctx.getVar(sn->text);
            vn.value = typelogic::scan(std::cin, vn.typeinfo);
        } 
        else if(tree->type == SyntaxNode::VAR_DECL)
            varDeclStm(ctx, tree);
        else if(tree->type == SyntaxNode::VAR_SET)
            varSetStm(ctx, tree);
        else if(tree->type == SyntaxNode::IF_ELSE)
            ifElseStm(ctx, tree);
        else if(tree->type == SyntaxNode::WHILE)
            whileStm(ctx, tree);
        else if(tree->type == SyntaxNode::STM_LIST) {
            ListSN* sn = (ListSN*)tree;
            ctx.pushScope();
            for(SyntaxNode* c : sn->children) {
                statement(ctx, c);
                if(ctx.error != nullptr) {
                    ctx.popScope();
                    return;
                }
            }
            ctx.popScope();
        }
    };

    Variable expression(EvalContext& ctx, SyntaxNode* tree) {
        if(ctx.error != nullptr)
            return Variable();
        if(tree->type == SyntaxNode::EXPR_VAR) {
            TextualSN* sn = (TextualSN*)tree;
            return ctx.getVar(sn->text).ref();
        }
        else if(tree->type == SyntaxNode::EXPR_NUM_LIT) {
            TextualSN* sn = (TextualSN*)tree;
            int* val = new int;
            isstream(sn->text) >> *val;
            return Variable(PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::INT32], val);
        } 
        else if(tree->type == SyntaxNode::EXPR_CHAR_LIT) {
            TextualSN* sn = (TextualSN*)tree;
            return Variable(PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::CHAR], new char(sn->text[0]));
        } else if(tree->type == SyntaxNode::EXPR_STRING_LIT) {
            TextualSN* sn = (TextualSN*)tree;
            return Variable(PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::STRING], new string(sn->text));
        }
        else if(tree->type == SyntaxNode::EXPR_UN_OP) {
            UnaryOpSN* sn = (UnaryOpSN*)tree;
            Variable val = expression(ctx, sn->inside);
            if(ctx.error != nullptr)
                return Variable();
            Variable res = typelogic::unaryOp(sn->opType, val);
            val.free();
            return res;
        } 
        else if(tree->type == SyntaxNode::EXPR_BIN_OP) {
            BinaryOpSN* sn = (BinaryOpSN*)tree;
            Variable first = expression(ctx, sn->first);
            if(ctx.error != nullptr)
                return Variable();
            Variable second = expression(ctx, sn->second);
            Variable res = typelogic::binaryOp(sn->opType, first, second);
            first.free();
            second.free();
            return res;
        }
        return Variable();
    }

}