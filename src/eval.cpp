#include "eval.hpp"

EvalContext::~EvalContext() {
    if(error != nullptr)
        delete error;
}

void evalStatement(EvalContext& ctx, SyntaxNode* tree) {
    if(tree->type == SyntaxNode::STM_PRINT) {
        OneChildSN* sn = (OneChildSN*)tree;
        int val = evalExpression(ctx, sn->child);
        if(ctx.error != nullptr)
            return;
        cout << val << endl;
    }
    else if(tree->type == SyntaxNode::STM_SCAN) {
        TextualSN* sn = (TextualSN*)tree;
        if(!ctx.variables.count(sn->text)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->text + string("' was not declared"), tree);
            return;
        }
        std::cin >> ctx.variables[sn->text];
    } else if(tree->type == SyntaxNode::VAR_DECL) {
        ListSN* sn = (ListSN*)tree;
        for(SyntaxNode* tchild : sn->children) {
            if(tchild->type == SyntaxNode::EXPR_VAR) {
                TextualSN* schild = (TextualSN*)tchild;
                if(ctx.variables.count(schild->text)) {
                    ctx.error = new EvalContext::Error(string("Variable '") + schild->text + string("' was already declared"), tree);
                    return;
                }
                ctx.variables[schild->text] = 0;
            } else if(tchild->type == SyntaxNode::VAR_SET) {
                SetSN* schild = (SetSN*)tchild;
                if(ctx.variables.count(schild->name)) {
                    ctx.error = new EvalContext::Error(string("Variable '") + schild->name + string("' was already declared"), tchild);
                    return;
                }
                int expr = evalExpression(ctx, schild->value);
                if(ctx.error != nullptr)
                    return;
                ctx.variables[schild->name] = expr;
            }
        }
    } else if(tree->type == SyntaxNode::VAR_SET) {
        SetSN* sn = (SetSN*)tree;
        if(ctx.variables.count(sn->name)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->name + string("' was already declared"), sn);
            return;
        }
        int expr = evalExpression(ctx, sn->value);
        if(ctx.error != nullptr)
            return;
        ctx.variables[sn->name] = expr;
    } else if(tree->type == SyntaxNode::STM_LIST) {
        ListSN* sn = (ListSN*)tree;
        for(SyntaxNode* c : sn->children) {
            evalStatement(ctx, c);
            if(ctx.error != nullptr)
                return;
        }
    }
};

int evalExpression(EvalContext& ctx, SyntaxNode* tree) {
    if(ctx.error != nullptr)
        return 0;
    if(tree->type == SyntaxNode::EXPR_VAR) {
        TextualSN* sn = (TextualSN*)tree;
        if(ctx.variables.count(sn->text)) 
            return ctx.variables[sn->text];
        ctx.error = new EvalContext::Error(string("Variable '") + sn->text + "' was not declared", tree);
        return 0;
    }
    else if(tree->type == SyntaxNode::EXPR_NUM_LIT) {
        TextualSN* sn = (TextualSN*)tree;
        int val;
        isstream(sn->text) >> val;
        return val;
    } else if(tree->type == SyntaxNode::EXPR_UN_OP) {
        UnaryOpSN* sn = (UnaryOpSN*)tree;
        int val = evalExpression(ctx, sn->inside);
        if(ctx.error != nullptr)
            return 0;
        switch(sn->opType) {
            case Token::PLUS:  return val;
            case Token::MINUS: return -val;
            default: throw "UNEXPECTED OP TYPE";
        };
    } else if(tree->type == SyntaxNode::EXPR_BIN_OP) {
        BinaryOpSN* sn = (BinaryOpSN*)tree;
        int first = evalExpression(ctx, sn->first);
        if(ctx.error != nullptr)
            return 0;
        int second = evalExpression(ctx, sn->second);
        if(ctx.error != nullptr)
            return 0;
        switch(sn->opType) {
            case Token::PLUS:   return first + second;
            case Token::MINUS:  return first - second;
            case Token::STAR:   return first * second;
            case Token::DIV:    return first / second;
            default:            throw "UNEXPECTED OP TYPE";
        }
    }
    return 0;
}