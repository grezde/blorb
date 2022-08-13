#pragma once
#include "token.hpp"

struct SyntaxNode {

    static constexpr const char* PRINTS[] = { 
        "ERROR", "LIST",
        "PRINT", "SCAN", "VAR SET", "VAR DECL", 
        "BIN OP", "UN OP", 
        "NUMBER", "CHAR", "STRING",
        "VAR", "TYPENAME", "IF-ELSE", "WHILE"
    };
    static void printIndent(std::ostream& ss, int indent);

    enum Type {
        ERROR,           // Textual
        STM_LIST,        // List
        STM_PRINT,       // List
        STM_SCAN,        // Textual
        VAR_SET,         // Set
        VAR_DECL,        // VarDecl
        EXPR_BIN_OP,     // BinaryOp
        EXPR_UN_OP,      // UnaryOp
        EXPR_NUM_LIT,    // Textual
        EXPR_CHAR_LIT,   // Textual
        EXPR_STRING_LIT, // Textual
        EXPR_VAR,        // Textual
        TYPE_NAME,       // Textual
        IF_ELSE,         // IfElse
        WHILE            // While
    };
    Type type;
    int startPos, endPos;

    SyntaxNode(Type type, int pos) : type(type), startPos(pos), endPos(pos) {};
    SyntaxNode(Type type, int endPos, int startPos) : type(type), startPos(startPos), endPos(endPos) {};
    virtual string toString(int indent = 0);
};

SyntaxNode* ExpectedSyntaxError(string what, const Token& token, int startPos, int endPos);
SyntaxNode* ExpectedSyntaxError(string what, const Token& token, int pos);

struct IfElseSN : SyntaxNode {
    SyntaxNode* expr;
    SyntaxNode* ifStm;
    SyntaxNode* elseStm;
    IfElseSN(SyntaxNode* expr, SyntaxNode* ifStm, int index) 
        : SyntaxNode(SyntaxNode::IF_ELSE, index, ifStm->endPos), expr(expr), ifStm(ifStm), elseStm(nullptr) {};
    IfElseSN(SyntaxNode* expr, SyntaxNode* ifStm, SyntaxNode* elseStm, int index) 
        : SyntaxNode(SyntaxNode::IF_ELSE, index, elseStm->endPos), expr(expr), ifStm(ifStm), elseStm(elseStm) {};
    ~IfElseSN();
    string toString(int indent = 0);
};

struct VarDeclSN : SyntaxNode {
    SyntaxNode* typexp;
    vector<SyntaxNode*> assignements;
    VarDeclSN(SyntaxNode* typexp, int index) : SyntaxNode(SyntaxNode::VAR_DECL, index), typexp(typexp) {};
    void appendChild(SyntaxNode* child);
    string toString(int indent = 0);
    ~VarDeclSN();
};

struct SetSN : SyntaxNode {
    SyntaxNode* value;
    string name;
    SetSN(Type type, string name, SyntaxNode* value, int startPos, int endPos) 
        : SyntaxNode(type, startPos, endPos), value(value), name(name) {}
    ~SetSN() { delete value; }
    string toString(int indent = 0);
};

struct OneChildSN : SyntaxNode {
    SyntaxNode* child;
    OneChildSN(Type type, SyntaxNode* child, int startPos, int endPos) 
        : SyntaxNode(type, startPos, endPos), child(child) {};
    ~OneChildSN() { delete child; };
    string toString(int indent = 0);
};

struct ListSN : SyntaxNode {
    vector<SyntaxNode*> children;
    ListSN(Type type, int& index) : SyntaxNode(type, index, index) {};
    void appendChild(SyntaxNode* child);
    string toString(int indent = 0);
};

struct TextualSN : SyntaxNode {
    string text;
    TextualSN(Type type, string text, int pos) : SyntaxNode(type, pos), text(text) {}
    TextualSN(Type type, string text, int startPos, int endPos) : SyntaxNode(type, startPos, endPos), text(text) {};
    string toString(int indent = 0);
};

struct UnaryOpSN : SyntaxNode {
    Token::Type opType;
    SyntaxNode* inside;
    UnaryOpSN(Token::Type opType, SyntaxNode* inside, bool prefix);
    ~UnaryOpSN();
    string toString(int indent = 0);
    static int precedence(Token::Type op);
};

struct BinaryOpSN : SyntaxNode {
    Token::Type opType;
    SyntaxNode* first;
    SyntaxNode* second;
    BinaryOpSN(Token::Type opType, SyntaxNode* first, SyntaxNode* second);
    ~BinaryOpSN();
    string toString(int indent = 0);
    static const int MAX_PRECEDENCE = 3;
    static int precedence(Token::Type op);
    static bool associativity(Token::Type op);
    static bool isBinaryOp(Token::Type op);
};