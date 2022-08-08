#include "syntax.hpp"

SyntaxNode* ExpectedSyntaxError(string what, const Token& token, int startPos, int endPos) {
    osstream ss;
    ss << "Expected " << what << ", instead got token " << token.toString();
    return new TextualSN(SyntaxNode::ERROR, ss.str(), startPos, endPos);
}

SyntaxNode* ExpectedSyntaxError(string what, const Token& token, int pos) {
    return ExpectedSyntaxError(what, token, pos, pos);
}

string SyntaxNode::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    ss << PRINTS[type] << endl;
    return ss.str();
}

string TextualSN::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    ss << PRINTS[type] << " (" << text << ")" << endl;
    return ss.str();
}

string ListSN::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    ss << PRINTS[type] << ":" << endl;
    for(SyntaxNode* sn : children)
        ss << sn->toString(indent+1);
    return ss.str();
}

string SetSN::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    ss << PRINTS[type] << " (" << name << "):" << endl << value->toString(indent+1);
    return ss.str();
}

string OneChildSN::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    ss << PRINTS[type] << ":" << endl << child->toString(indent+1);
    return ss.str();
}

string BinaryOpSN::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    char* a = Token::PRINTS[opType][0] == ' ' ? (char*)Token::PRINTS[opType] + 1 : (char*)Token::PRINTS[opType];
    ss << PRINTS[type] << " (" << a << "):" << endl;
    ss << first->toString(indent+1) << second->toString(indent+1);
    return ss.str();
}

string UnaryOpSN::toString(int indent) {
    osstream ss;
    for(int i=0; i<indent; i++)
        ss << INDENT;
    ss << PRINTS[type] << " (" << Token::PRINTS[opType] << "):" << endl;
    ss << inside->toString(indent+1);
    return ss.str();
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
        case Token::PLUS: case Token::MINUS:
            return 1;
        case Token::STAR: case Token::DIV:
            return 2;
        default:
            return -1;
    }
}
bool BinaryOpSN::associativity(Token::Type op) {
    switch(op) {
        case Token::PLUS: case Token::MINUS:
        case Token::STAR: case Token::DIV:
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
        case Token::PLUS: case Token::MINUS:
            return 1;
        default:
            return -1;
    }
}
