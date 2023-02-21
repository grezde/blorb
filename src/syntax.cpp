#include "syntax.hpp"

SyntaxNode* ExpectedSyntaxError(string what, const Token& token, int startPos, int endPos) {
    osstream ss;
    ss << "Expected " << what << ", instead got token " << token.toString();
    return new TextualSN(SyntaxNode::ERROR, ss.str(), startPos, endPos);
}

SyntaxNode* ExpectedSyntaxError(string what, const Token& token, int pos) {
    return ExpectedSyntaxError(what, token, pos, pos);
}

IfElseSN::~IfElseSN() {
    delete expr;
    delete ifStm;
    if(elseStm != nullptr)
        delete elseStm;
}

void SyntaxNode::printIndent(std::ostream& ss, int indent) {
    for(int i=0; i<indent; i++)
        ss << "  ";
}

string SyntaxNode::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << PRINTS[type] << endl;
    return ss.str();
}

string TextualSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << PRINTS[type] << " (" << text << ")" << endl;
    return ss.str();
}

string ListSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << PRINTS[type] << ":" << endl;
    for(SyntaxNode* sn : children)
        ss << sn->toString(indent+1);
    return ss.str();
}

string SetSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << PRINTS[type] << " (" << name << "):" << endl << value->toString(indent+1);
    return ss.str();
}

string OneChildSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << PRINTS[type] << ":" << endl << child->toString(indent+1);
    return ss.str();
}

string BinaryOpSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    char* a = Token::PRINTS[opType][0] == ' ' ? (char*)Token::PRINTS[opType] + 1 : (char*)Token::PRINTS[opType];
    ss << PRINTS[type] << " (" << a << "):" << endl;
    ss << first->toString(indent+1) << second->toString(indent+1);
    return ss.str();
}

string UnaryOpSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    char* a = Token::PRINTS[opType][0] == ' ' ? (char*)Token::PRINTS[opType] + 1 : (char*)Token::PRINTS[opType];
    ss << PRINTS[type] << " (" << a << "):" << endl;
    ss << inside->toString(indent+1);
    return ss.str();
}

string UnaryTypeOpSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << "UNARY TYPE OPERATOR\n";
    ss << inside->toString(indent+1);
    return ss.str();
}

string VarDeclSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << PRINTS[type] << ":" << endl;
    ss << typexp->toString(indent+1);
    for(SyntaxNode* sn : assignements)
        ss << sn->toString(indent+1);
    printIndent(ss, indent);
    ss << "END VAR DECL" << std::endl;
    return ss.str();
}

string IfElseSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << "IF:" << endl << expr->toString(indent+1);
    printIndent(ss, indent);
    ss << "THEN:" << endl << ifStm->toString(indent+1);
    if(elseStm != nullptr) {
        printIndent(ss, indent);
        ss << "ELSE:" << endl << elseStm->toString(indent+1);
    }
    printIndent(ss, indent);
    ss << "END IF" << endl;
    return ss.str();
}

string WhileSN::toString(int indent) {
    osstream ss;
    printIndent(ss, indent);
    ss << "WHILE:" << endl << expr->toString(indent+1);
    printIndent(ss, indent);
    ss << "THEN:" << endl << stm->toString(indent+1);
    printIndent(ss, indent);
    ss << "END WHILE" << endl;
    return ss.str();
}

WhileSN::~WhileSN() {
    delete expr;
    delete stm;
}

void VarDeclSN::appendChild(SyntaxNode* child) {
    assignements.push_back(child);
    if(child->endPos > endPos)
        endPos = child->endPos;
    if(child->startPos < startPos)
        startPos = child->startPos;
};


VarDeclSN::~VarDeclSN() {
    delete typexp;
    for(SyntaxNode* sn : assignements)
        delete sn;
}

void ListSN::appendChild(SyntaxNode* child) {
    children.push_back(child);
    if(endPos < child->endPos)
        endPos = child->endPos;
    if(startPos > child->startPos)
        startPos = child->startPos;
}

BinaryOpSN::BinaryOpSN(Token::Type opType, SyntaxNode* first, SyntaxNode* second)
    : SyntaxNode(EXPR_BIN_OP, first->startPos, second->endPos),
      opType(opType), first(first), second(second)
    {}
BinaryOpSN::~BinaryOpSN() {
    delete first;
    delete second;
}

int BinaryOpSN::precedence(Token::Type op) {
    switch(op) {
        case Token::PLUS_EQ: case Token::MINUS_EQ:
        case Token::STAR_EQ: case Token::DIV_EQ:
        case Token::EQ:
            return 1;
        case Token::OROR:
            return 2;
        case Token::ANDAND: 
            return 3;
        case Token::EQEQ:  case Token::NEQ:
            return 4;
        case Token::LEQ:   case Token::GEQ:
        case Token::LT:    case Token::GT:
            return 5;
        case Token::PLUS:  case Token::MINUS:
            return 6;
        case Token::STAR:  case Token::DIV:
            return 7;
        default:
            return -1;
    }
}
bool BinaryOpSN::associativity(Token::Type op) {
    int p = precedence(op);
    switch(p) {
        case 1:
            return false;
        case 2: case 3: case 4:
        case 5: case 6: case 7:
            return true;
        default:
            return false;
    }
}
bool BinaryOpSN::isBooleanOp(Token::Type op) {
    switch(op) {
        case Token::EQEQ: case Token::NEQ:
        case Token::GT:   case Token::LT:
        case Token::GEQ:  case Token::LEQ:
            return true;
        default:
            return false;
    }
}
bool BinaryOpSN::isInteractive(Token::Type op) {
    switch(op) {
        case Token::EQ:  
        case Token::PLUS_EQ: case Token::MINUS_EQ:
        case Token::STAR_EQ: case Token::DIV_EQ:
        case Token::PLUSPLUS_PREF: case Token::MINUSMINUS_PREF:
        case Token::PLUSPLUS_POST: case Token::MINUSMINUS_POST:
            return true;
        default:
            return false;
    }
}

UnaryOpSN::UnaryOpSN(Token::Type opType, SyntaxNode* inside, bool prefix)
    : SyntaxNode(EXPR_UN_OP, inside->startPos - prefix ? 1 : 0, inside->endPos + prefix ? 0 : 1),
      opType(opType)  
{}
UnaryOpSN::~UnaryOpSN() {
    delete inside;
}

int UnaryOpSN::precedence(Token::Type op) {
    switch(op) {
        case Token::PLUSPLUS_PREF: case Token::MINUSMINUS_PREF:
        case Token::PLUS: case Token::MINUS:
        case Token::NEGATE:
            return 1;
        case Token::PLUSPLUS_POST: case Token::MINUSMINUS_POST:
            return 2;
        default:
            return -1;
    }
}
bool UnaryOpSN::isInteractive(Token::Type op) {
    switch(op) {
        case Token::PLUSPLUS_POST: case Token::PLUSPLUS_PREF:
        case Token::MINUSMINUS_POST: case Token::MINUSMINUS_PREF:
            return true;
        default:
            return false;
    }
}
