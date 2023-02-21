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

char Token::escape(const char& c) {
    switch(c) {
        case '0':   return '\0';
        case 'n':   return '\n';
        case '\'':  return '\'';
        case '"':   return '"';
        case 'r':   return '\r';
        case 't':   return '\t';
        case '\\':  return '\\';
        case '\n':  return '\n';
        default:    return c;  
    }
}

Token Token::nextStrLiteral(const string& input, int& index, Token::Type type, string ername) {
    Token a;
    a.type = ERROR;
    a.text = "";
    a.position = index;
    const char chr = input[index];
    bool esc = false;
    while(input[++index] != chr) {
        if(input[index] == EOF) {
            osstream ss;
            ss << "Unexpected end of input in " << ername << " literal " << chr << a.text << chr;
            a.text = ss.str();
            return a;
        }
        if(input[index] == '\n' && !esc) {
            osstream ss;
            ss << "Unexpected new line in " << ername << " literal " << chr << a.text << chr << ". Use \\ to mark a new line character";
            a.text = ss.str();
            return a;
        }
        if(input[index] == '\\')
            esc = true;
        else if(esc) {
            a.text += escape(input[index]);
            esc = false;
        } else {
            a.text += input[index];
        }
    };
    index++;
    if(type == CHAR_LIT && a.text.length() != 1) {
        osstream ss;
        ss << "Character literal '" << a.text << "' must have exactly 1 charcter";
        a.text = ss.str();
        return a;
    }
    a.type = type;
    return a;
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
        while(input[index] != EOF && input[index] != '\n');
        index++;
        return next(input, index);
    }
    else if(input[index] == '"')
        return nextStrLiteral(input, index, STR_LIT, "string");
    else if(input[index] == '\'') 
        return nextStrLiteral(input, index, CHAR_LIT, "char");
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
        if(input[index+1] == '+') {
            a.type = PLUSPLUS;
            a.text = "++";
            index+=2;
        }
        else if(input[index+1] == '=') {
            a.type = PLUS_EQ;
            a.text = "+=";
            index+=2;
        }
        else {
            a.type = PLUS;
            a.text = "+";
            index++;
        }
    }
    else if(input[index] == '-') {
        if(input[index+1] == '-') {
            a.type = MINUSMINUS;
            a.text = "--";
            index+=2;
        }
        else if(input[index+1] == '=') {
            a.type = MINUS_EQ;
            a.text = "-=";
            index+=2;
        }
        else {
            a.type = MINUS;
            a.text = "-";
            index++;
        }
    }
    else if(input[index] == '*') {
        if(input[index+1] == '=') {
            a.type = STAR_EQ;
            a.text = "*=";
            index+=2;
        }
        else {
            a.type = STAR;
            a.text = "*";
            index++;
        }
    }
    else if(input[index] == '/') {
        if(input[index+1] == '=') {
            a.type = DIV_EQ;
            a.text = "/=";
            index += 2;
        }
        else {
            a.type = DIV;
            a.text = "/";
            index++;
        }
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
        if(input[index+1] == '=') {
            a.type = EQEQ;
            a.text = "==";
            index += 2;
        } else {
            a.type = EQ;
            a.text = "=";
            index++;
        }
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
    } else if(input[index] == '&' && input[index+1] == '&') {
        a.type = ANDAND;
        a.text = "&&";
        index += 2;
    } else if(input[index] == '|' && input[index+1] == '|') {
        a.type = OROR;
        a.text = "||";
        index += 2;
    } else if(input[index] == '>') {
        if(input[index+1] == '=') {
            a.type = GEQ;
            a.text = ">=";
            index += 2;
        } else {
            a.type = GT;
            a.text = ">";
            index++;
        }
    } else if(input[index] == '<') {
        if(input[index+1] == '=') {
            a.type = LEQ;
            a.text = "<=";
            index += 2;
        } else {
            a.type = LT;
            a.text = "<";
            index++;
        }
    } else if(input[index] == '!') {
        if(input[index+1] == '=') {
            a.type = NEQ;
            a.text = "!=";
            index += 2;
        } else {
            a.type = NEGATE;
            a.text = "!";
            index++;
        }
    }
    else if(input[index] == '[') {
        a.type = O_SQUARE;
        a.text = "[";
        index++;
    }
    else if(input[index] == ']') {
        a.type = C_SQUARE;
        a.text = "]";
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