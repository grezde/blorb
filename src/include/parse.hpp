#include "syntax.hpp"

namespace parse {

    typedef const vector<Token>& Tokens;

    SyntaxNode* expressionRecursive(Tokens tokens, int& index, int prec, SyntaxNode* startingNode);
    SyntaxNode* expressionStart(Tokens tokens, int& index);
    SyntaxNode* expression(Tokens tokens, int& index);
    SyntaxNode* varSet(Tokens tokens, int& index);
    SyntaxNode* varDeclStm(Tokens tokens, int& index);
    SyntaxNode* varSetStm(Tokens tokens, int& index);
    SyntaxNode* printStm(Tokens tokens, int& index);
    SyntaxNode* scanStm(Tokens tokens, int& index);
    SyntaxNode* compoundStm(Tokens tokens, int& index);
    SyntaxNode* statement(Tokens tokens, int& index);
    SyntaxNode* statements(Tokens tokens, int& index, Token::Type endingToken);
    SyntaxNode* file(Tokens tokens);

}