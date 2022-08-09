#include "eval.hpp"

EvalContext::EvalContext() {
    pushScope();
}

EvalContext::~EvalContext() {
    if(error != nullptr)
        delete error;
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

int& EvalContext::getVar(string varname) {
    for(int i=scopes.size()-1; i>=0; i--)
        if(scopes[i].variables.count(varname))
            return scopes[i].variables[varname];
    scopes[scopes.size()-1].variables[varname] = 0;
    throw string("Tried getting variable name '") + varname + string("' altough it does not exist");
}

void EvalContext::createVar(string varname, int value) {
    scopes[scopes.size()-1].variables[varname] = value;
}

void EvalContext::pushScope() {
    scopes.push_back(Scope());
}

void EvalContext::popScope() {
    scopes.pop_back();
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
        if(!ctx.hasVar(sn->text)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->text + string("' was not declared"), tree);
            return;
        }
        std::cin >> ctx.getVar(sn->text);
    } else if(tree->type == SyntaxNode::VAR_DECL) {
        ListSN* sn = (ListSN*)tree;
        for(SyntaxNode* tchild : sn->children) {
            if(tchild->type == SyntaxNode::EXPR_VAR) {
                TextualSN* schild = (TextualSN*)tchild;
                if(ctx.hasVarLastScope(schild->text)) {
                    ctx.error = new EvalContext::Error(string("Variable '") + schild->text + string("' was already declared in this scope"), tree);
                    return;
                }
                ctx.createVar(schild->text, 0);
            } else if(tchild->type == SyntaxNode::VAR_SET) {
                SetSN* schild = (SetSN*)tchild;
                if(ctx.hasVarLastScope(schild->name)) {
                    ctx.error = new EvalContext::Error(string("Variable '") + schild->name + string("' was already declared in this scope"), tchild);
                    return;
                }
                int expr = evalExpression(ctx, schild->value);
                if(ctx.error != nullptr)
                    return;
                ctx.createVar(schild->name, expr);
            }
        }
    } else if(tree->type == SyntaxNode::VAR_SET) {
        SetSN* sn = (SetSN*)tree;
        if(!ctx.hasVar(sn->name)) {
            ctx.error = new EvalContext::Error(string("Variable '") + sn->name + string("' was not declared or is out of scope"), sn);
            return;
        }
        int expr = evalExpression(ctx, sn->value);
        if(ctx.error != nullptr)
            return;
        ctx.getVar(sn->name) = expr;
    } else if(tree->type == SyntaxNode::STM_LIST) {
        ListSN* sn = (ListSN*)tree;
        ctx.pushScope();
        for(SyntaxNode* c : sn->children) {
            evalStatement(ctx, c);
            if(ctx.error != nullptr)
                return;
        }
        ctx.popScope();
    }
};

int evalExpression(EvalContext& ctx, SyntaxNode* tree) {
    if(ctx.error != nullptr)
        return 0;
    if(tree->type == SyntaxNode::EXPR_VAR) {
        TextualSN* sn = (TextualSN*)tree;
        if(ctx.hasVar(sn->text)) 
            return ctx.getVar(sn->text);
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
            default: throw string("UNEXPECTED OP TYPE ") + string(Token::PRINTS[sn->opType]);
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
            default: throw string("UNEXPECTED OP TYPE ") + string(Token::PRINTS[sn->opType]);
        }
    }
    return 0;
}