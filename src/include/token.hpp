#pragma once
#include "utils.hpp"

struct Token {

    static constexpr const char* PRINTS[] = { 
        " ERROR", "NEW LINE", "END OF FILE",
        " NUMBER", " CHAR", " STRING", " KEYWORD", " IDENT", 
        " PLUS", " MINUS", " STAR", " DIV", 
        " OPEN PAREN", " CLOSING PAREN", 
        " OPEN CURLY", " CLOSING CURLY",
        " EQUALS", " SEMICOLON", " COMMA",
        " COMPARE", " LESS THAN", " LESS OR EQUAL", " GREATER THAN", " GREATER OR EQUAL", " NOT EQUAL",
        " NEGATE", " AND OP", " OR OP"
    };

    static const int KEYWORDSN = 6;
    static constexpr const char* KEYWORDS[] = {
        "var", "print", "scan", "if", "else", "while"
    };
    
    enum Type {
        ERROR, NEWLINE, T_EOF,
        NUM_LIT, CHAR_LIT, STR_LIT, KEYWORD, IDENTIFIER,
        PLUS, MINUS, STAR, DIV,
        O_PAREN, C_PAREN,
        O_CURLY, C_CURLY,
        EQ, SC, COMMA,
        EQEQ, LT, LEQ, GT, GEQ, NEQ,
        NEGATE, ANDAND, OROR
    };

    Type type;
    string text;
    int position;

    string toString() const;

    static inline bool isVarStart(const char& c);
    static inline bool isWhiteSpace(const char& c);
    static inline bool isNum(const char& c);
    static char escape(const char& c);

    static Token nextStrLiteral(const string& input, int& index, Type type, string ername);
    static Token next(const string& input, int& index);
    static vector<Token> all(const string& input);
    static const string printAll(const vector<Token>& ts);
    
};