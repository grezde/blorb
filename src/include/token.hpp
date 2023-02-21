#pragma once
#include "utils.hpp"

struct Token {

    static constexpr const char* PRINTS[] = { 
        " ERROR", "NEW LINE", "END OF FILE",
        " NUMBER", " CHAR", " STRING", " KEYWORD", " IDENT", 
        " PLUS", " MINUS", " STAR", " DIV", 
        " OPEN PAREN", " CLOSING PAREN", 
        " OPEN CURLY", " CLOSING CURLY",
        " OPEN SQUARE", " CLOSING SQUARE",
        " EQUALS", " SEMICOLON", " COMMA",
        " COMPARE", " LESS THAN", " LESS OR EQUAL", " GREATER THAN", " GREATER OR EQUAL", " NOT EQUAL",
        " NEGATE", " AND OP", " OR OP",
        " PLUS EQUAL", " MINUS EQUAL", " STAR EQUAL", " DIV EQUAL",
        " INCREMENT", " DECREMENT",
        " INCREMENT PREFIX", " DECREMENT PREFIX",
        " INCREMENT POSTFIX", " DECTREMENT POSTFIX"
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
        O_SQUARE, C_SQUARE,
        EQ, SC, COMMA,
        EQEQ, LT, LEQ, GT, GEQ, NEQ,
        NEGATE, ANDAND, OROR,
        PLUS_EQ, MINUS_EQ, STAR_EQ, DIV_EQ,
        PLUSPLUS, MINUSMINUS,
        PLUSPLUS_PREF, MINUSMINUS_PREF,
        PLUSPLUS_POST, MINUSMINUS_POST
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