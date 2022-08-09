#include "token.hpp"
#include <iomanip>
#include <sstream>

string Token::toString() const {
    if(PRINTS[type][0] == ' ') {
        string s = PRINTS[type]+1;
        s.append(" ");
        s.append(text);
        return s;
    }
    else
        return string(PRINTS[type]);
}

bool Token::isVarStart(const char& c) {
    return 
        ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z') ||
        c == '_';
}

bool Token::isWhiteSpace(const char& c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool Token::isNum(const char& c) {
    return '0' <= c && c <= '9';
}

Token Token::next(const string& input, int& index) {
    Token a;
    a.type = ERROR; 
    a.text = "";
    while(index < input.length() && isWhiteSpace(input[index]))
        index++;
    a.position = index; 
    if(index >= input.length() || input[index] == '\0' || input[index] == EOF)
        a.type = T_EOF;
    else if(input[index] == '#') {
        do index++;
        while(index < input.length() && input[index] != '\n');
        index++;
        return next(input, index);
    }
    else if(isNum(input[index])) {
        while(isNum(input[index]))
            a.text += input[index++];
        a.type = NUM_LIT;
    }
    else if(isVarStart(input[index])) {
        while(isVarStart(input[index]) || isNum(input[index]))
            a.text += input[index++];
        a.type = IDENTIFIER;
        for(int j=0; j<KEYWORDSN; j++)
            if(a.text == string(KEYWORDS[j])) {
                a.type = KEYWORD;
                break;
            }
    }
    else if(input[index] == '+') {
        a.type = PLUS;
        a.text = "+";
        index++;
    }
    else if(input[index] == '-') {
        a.type = MINUS;
        a.text = "-";
        index++;
    }
    else if(input[index] == '*') {
        a.type = STAR;
        a.text = "*";
        index++;
    }
    else if(input[index] == '/') {
        a.type = DIV;
        a.text = "/";
        index++;
    }
    else if(input[index] == '(') {
        a.type = O_PAREN;
        a.text = "(";
        index++;
    }
    else if(input[index] == ')') {
        a.type = C_PAREN;
        a.text = ")";
        index++;
    }
    else if(input[index] == '=') {
        a.type = EQ;
        a.text = "=";
        index++;
    } else if(input[index] == ';') {
        a.type = SC;
        a.text = ";";
        index++;
    } else if(input[index] == ',') {
        a.type = COMMA;
        a.text = ",";
        index++;
    } else if(input[index] == '{') {
        a.type = O_CURLY;
        a.text = "{";
        index++;
    } else if(input[index] == '}') {
        a.type = C_CURLY;
        a.text = "}";
        index++;
    }
    else {
        a.text = "Unexpected character '";
        a.text += input[index++];
        a.text += "'.";
    } //*/
    return a;
}

vector<Token> Token::all(const string& input) {
    vector<Token> ts;
    int index = 0;
    Token t = next(input, index);
    while(t.type != T_EOF) {
        if(t.type == ERROR) {
            ts.clear();
            ts.push_back(t);
            return ts;
        }
        ts.push_back(t);
        t = next(input, index);
    }
    ts.push_back(t);
    return ts;
}

const string Token::printAll(const vector<Token>& ts) {
    std::ostringstream ss;
    for(int i=0; i<ts.size(); i++) {
        ss << "T" << std::setw(2) << std::setfill('0') << i << " at ";
        ss << std::setw(3) << std::setfill('0') << ts[i].position << ": ";
        ss << ts[i].toString() << endl;
    }
    return ss.str();
}