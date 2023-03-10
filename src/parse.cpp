#include "parse.hpp"

namespace parse {

    SyntaxNode* typexpr(Tokens tokens, int& index) {
        if(tokens[index].type != Token::IDENTIFIER)
            return ExpectedSyntaxError("a type name", tokens[index], index);
        TextualSN* a = new TextualSN(SyntaxNode::TYPE_NAME, tokens[index].text, index);
        if(index + 2 < tokens.size() && tokens[index+1].type == Token::O_SQUARE && tokens[index+2].type == Token::C_SQUARE) {
            index += 3;
            return new UnaryTypeOpSN(UnaryTypeOpSN::M_ARRAY, a);
        }
        else {
            index++;
            return a;
        }
    }

    SyntaxNode* expression(Tokens tokens, int& index) {
        SyntaxNode* start = expressionStart(tokens, index);
        if(start->type == SyntaxNode::ERROR)
            return start;
        SyntaxNode* sn =  expressionRecursive(tokens, index, 0, start);
        return sn;
    }

    SyntaxNode* expressionStart(Tokens tokens, int& index) {
        if(UnaryOpSN::precedence(tokens[index].type) > 0) {
            int index0 = index;
            index++;
            SyntaxNode* sn = expressionStart(tokens, index);
            if(sn->type == SyntaxNode::ERROR)
                return sn;
            return new UnaryOpSN(tokens[index0].type, sn, true);
        }
        else if(tokens[index].type == Token::NUM_LIT)
            return new TextualSN(SyntaxNode::EXPR_NUM_LIT, tokens[index++].text, index);
        else if(tokens[index].type == Token::CHAR_LIT)
            return new TextualSN(SyntaxNode::EXPR_CHAR_LIT, tokens[index++].text, index);
        else if(tokens[index].type == Token::STR_LIT)
            return new TextualSN(SyntaxNode::EXPR_STRING_LIT, tokens[index++].text, index);
        else if(tokens[index].type == Token::IDENTIFIER)
            return new TextualSN(SyntaxNode::EXPR_VAR, tokens[index++].text, index);
        else if(tokens[index].type == Token::O_PAREN) {
            int index0 = index;
            index++;
            SyntaxNode* sn = expression(tokens, index);
            if(sn->type == SyntaxNode::ERROR)
                return sn;
            if(tokens[index].type != Token::C_PAREN) 
                return new TextualSN(SyntaxNode::ERROR, "Open parenthesis does not have matching closing parenthesis", index0, index);
            index++;
            return sn;
        }
        return ExpectedSyntaxError("expression", tokens[index], index);
    }

    SyntaxNode* expressionRecursive(Tokens tokens, int& index, int prec, SyntaxNode* startingNode) {
        if(BinaryOpSN::precedence(tokens[index].type) < prec)
            return startingNode;
        vector<SyntaxNode*> terms;
        vector<Token::Type> ops;
        terms.push_back(startingNode);
        while(BinaryOpSN::precedence(tokens[index].type) >= prec) {
            if(BinaryOpSN::precedence(tokens[index].type) > prec) {
                SyntaxNode* last = terms[terms.size()-1];
                terms.pop_back();
                SyntaxNode* nev = expressionRecursive(tokens, index, BinaryOpSN::precedence(tokens[index].type), last);
                if(nev->type == SyntaxNode::ERROR) {
                    for(SyntaxNode* s : terms)
                        delete s;
                    return nev;
                }
                terms.push_back(nev);
            } else {
                ops.push_back(tokens[index].type);
                index++;
                SyntaxNode* nxt = expressionStart(tokens, index);
                if(nxt->type == SyntaxNode::ERROR) {
                    for(SyntaxNode* s : terms)
                        delete s;
                    return nxt;
                }
                terms.push_back(nxt);   
            }
        }
        if(ops.size() == 0)
            return terms[0];
        
        //TODO associativity
        BinaryOpSN* red = new BinaryOpSN(ops[0], terms[0], terms[1]);
        for(int i=1; i<ops.size(); i++)
            red = new BinaryOpSN(ops[i], red, terms[i+1]);
        return red;
    }

    SyntaxNode* varSet(Tokens tokens, int& index) {
        int index0 = index;
        if(tokens[index].type != Token::IDENTIFIER)
            return ExpectedSyntaxError("variable name", tokens[index], index);
        string vname = tokens[index++].text;
        if(tokens[index].type != Token::EQ)
            return ExpectedSyntaxError("EQUALS (=) token", tokens[index], index0, index);
        index++;
        SyntaxNode* exp = expression(tokens, index);
        if(exp->type == SyntaxNode::ERROR)
            return exp;
        return new SetSN(SyntaxNode::VAR_SET, vname, exp, index0, index);
    }

    SyntaxNode* varDeclStm(Tokens tokens, int& index) {
        int index0 = index;
        SyntaxNode* typexp = typexpr(tokens, index);
        if(typexp->type == SyntaxNode::ERROR)
            return typexp;
        VarDeclSN* decls = new VarDeclSN(typexp, index0);
        index--;
        do {
            if(tokens[++index].type != Token::IDENTIFIER)
                return ExpectedSyntaxError("variable name in variable declaration", tokens[index], index0, index);
            int index1 = index;
            SyntaxNode* sn = varSet(tokens, index);
            if(sn->type != SyntaxNode::ERROR)
                decls->appendChild(sn);
            else {
                delete sn;
                index = index1;
                decls->appendChild(new TextualSN(SyntaxNode::EXPR_VAR, tokens[index++].text, index));
            }
        } while(tokens[index].type == Token::COMMA);
        if(tokens[index].type != Token::SC)
            return ExpectedSyntaxError("SEMICOLON (;) token variable declaration statement", tokens[index], index);
        index++;
        return decls;
    }

    SyntaxNode* varSetStm(Tokens tokens, int& index) {
        SyntaxNode* sn = varSet(tokens, index);
        if(sn->type == SyntaxNode::ERROR)
            return sn;
        if(tokens[index].type != Token::SC) {
            SyntaxNode* res =  ExpectedSyntaxError("SEMICOLON (;) token in statement", tokens[index], sn->startPos, index);
            delete sn;
            return res;
        }
        index++;
        sn->endPos++;
        return sn;
    }

    SyntaxNode* exprStm(Tokens tokens, int& index) {
        int index0 = index;
        SyntaxNode* expr = expression(tokens, index);
        if(expr->type == SyntaxNode::ERROR)
            return expr;
        if(tokens[index].type != Token::SC) {
            delete expr;
            return ExpectedSyntaxError("SEMICOLON (;) token after expression in statement", tokens[index], index0, index);
        }
        index++;
        return new OneChildSN(SyntaxNode::STM_EXPR, expr, index0, index);
    }

    SyntaxNode* printStm(Tokens tokens, int& index) {
        if(!(tokens[index].type == Token::KEYWORD && tokens[index].text == string("print")))
            return ExpectedSyntaxError("'print' keyword in print statement", tokens[index], index);
        int index0 = index;
        ListSN* ls = new ListSN(SyntaxNode::STM_PRINT, index0);
        do {
            index++;
            SyntaxNode* sn = expression(tokens, index);
            if(sn->type == SyntaxNode::ERROR) {
                delete ls;
                return sn;
            }
            ls->appendChild(sn);
        } while(tokens[index].type == Token::COMMA);
        if(tokens[index].type != Token::SC)
            return ExpectedSyntaxError("SEMICOLON (;) in print statement", tokens[index], index);
        index++;
        return ls;
    }

    SyntaxNode* scanStm(Tokens tokens, int& index) {
        if(!(tokens[index].type == Token::KEYWORD && tokens[index].text == string("scan")))
            return ExpectedSyntaxError("'print' keyword in print statement", tokens[index], index);
        int index0 = index++;
        if(tokens[index].type != Token::IDENTIFIER)
            return ExpectedSyntaxError("variable name after 'scan' keyword", tokens[index], index0, index);
        string vname = tokens[index].text;
        index++;
        if(tokens[index].type != Token::SC)
            return ExpectedSyntaxError("SEMICOLON (;) in scan statement", tokens[index], index);
        return new TextualSN(SyntaxNode::STM_SCAN, vname, index0, index++);
    }

    SyntaxNode* compoundStm(Tokens tokens, int& index) {
        if(tokens[index].type != Token::O_CURLY)
            return ExpectedSyntaxError("COPEN CURLY ({) token in compound statement", tokens[index], index);
        index++;
        return statements(tokens, index, Token::C_CURLY);
    }

    SyntaxNode* ifElseStm(Tokens tokens, int& index) {
        if(!(tokens[index].type == Token::KEYWORD && tokens[index].text == string("if")))
            return ExpectedSyntaxError("'if' keyword in if statement", tokens[index], index);
        index++;
        if(tokens[index].type != Token::O_PAREN)
            return ExpectedSyntaxError("open parenthesis in if statement", tokens[index], index);
        index++;
        SyntaxNode* expr = expression(tokens, index);
        if(expr->type == SyntaxNode::ERROR)
            return expr;
        if(tokens[index].type != Token::C_PAREN) {
            delete expr;
            return ExpectedSyntaxError("closing parenthesis in if statement", tokens[index], index);
        }
        index++;
        SyntaxNode* stm = statement(tokens, index);
        if(stm->type == SyntaxNode::ERROR) {
            delete expr;
            return stm;
        }
        if(!(tokens[index].type == Token::KEYWORD && tokens[index].text == string("else")))
            return new IfElseSN(expr, stm);
        index++;
        SyntaxNode* stm2 = statement(tokens, index);
        if(stm2->type == SyntaxNode::ERROR) {
            delete expr; delete stm;
            return stm2;
        }
        return new IfElseSN(expr, stm, stm2);
    }

    SyntaxNode* whileStm(Tokens tokens, int& index) {
        int index0 = index;
        if(!(tokens[index].type == Token::KEYWORD && tokens[index].text == string("while")))
            return ExpectedSyntaxError("'while' keyword in while statement", tokens[index], index);
        index++;
        if(tokens[index].type != Token::O_PAREN)
            return ExpectedSyntaxError("open parenthesis in while statement", tokens[index], index);
        index++;
        SyntaxNode* expr = expression(tokens, index);
        if(expr->type == SyntaxNode::ERROR)
            return expr;
        if(tokens[index].type != Token::C_PAREN) {
            delete expr;
            return ExpectedSyntaxError("closing parenthesis in while statement", tokens[index], index);
        }
        index++;
        SyntaxNode* stm = statement(tokens, index);
        if(stm->type == SyntaxNode::ERROR) {
            delete expr;
            return stm;
        }
        WhileSN* sn = new WhileSN(expr, stm);
        return sn;
    }

    SyntaxNode* statement(Tokens tokens, int& index) {
        // we predetermine what type of statement is based on the beggining
        // instead of trying them all
        if(tokens[index].type == Token::KEYWORD) {
            if(tokens[index].text == string("scan"))
                return scanStm(tokens, index);
            if(tokens[index].text == string("print"))
                return printStm(tokens, index);
            if(tokens[index].text == string("if")) 
                return ifElseStm(tokens, index);
            if(tokens[index].text == string("while")) 
                return whileStm(tokens, index);
            osstream ss;
            ss << "Unexpected keyword '" << tokens[index].text << "' at the beggining of statement";
            return new TextualSN(SyntaxNode::ERROR, ss.str(), index);
        }
        if(tokens[index].type == Token::O_CURLY)
            return compoundStm(tokens, index);
        int index0 = index;
        SyntaxNode* sn = exprStm(tokens, index);
        if(sn->type != SyntaxNode::ERROR)
            return sn;
        delete sn;
        index = index0;
        sn = varDeclStm(tokens, index);
        if(sn->type != SyntaxNode::ERROR)
            return sn;
        delete sn;
        index = index0;
        return ExpectedSyntaxError("statement", tokens[index], index);
    }

    SyntaxNode* statements(Tokens tokens, int& index, Token::Type endingToken) {
        static int indent = 0;
        ListSN* ns = new ListSN(SyntaxNode::STM_LIST, index);
        indent++;
        while(tokens[index].type != endingToken) {
            SyntaxNode* sn = statement(tokens, index);
            if(sn->type == SyntaxNode::ERROR) {
                delete ns;
                return sn;
            }
            ns->appendChild(sn);
        }
        index++;
        indent--;
        return ns;
    }

    SyntaxNode* file(Tokens tokens) {
        int index = 0;
        return statements(tokens, index, Token::T_EOF);
    }

}

