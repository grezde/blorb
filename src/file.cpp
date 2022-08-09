#include "file.hpp"
#include <fstream>
#include "eval.hpp"

string* File::readContents(const string& filename) {
    std::ifstream fin(filename);
    if(!fin.is_open())
        return nullptr;
    string* s = new string();
    while(!fin.eof())
        s->push_back(fin.get());
    return s;
}

File::File(const string filename) 
    : filename(filename), error("") {

    string* s = readContents(filename);
    if(s == NULL) {
        error = "COMPILER ERROR: cannot read file \'";
        error += filename;
        error += "\'";
        return;
    }
    contents = string(*s);
    delete s;
    newlines.push_back(0);
    for(int i=0; i<contents.length(); i++)
        if(contents[i] == '\n')
            newlines.push_back(i+1);
    newlines.push_back(contents.length());

    if(SHOW_HEADERS)
        cout << endl;
    if(SHOW_CONTENTS) {
        if(SHOW_HEADERS)
            cout << "-- FILE CONTENTS --" << endl;
        cout << contents << endl;
        if(SHOW_HEADERS)
            cout << endl;
    }

    tokens = Token::all(contents);
    if(tokens.size() == 0)
        return;
    if(tokens[0].type == Token::ERROR) {
        int i=0;
        while(newlines[i] < tokens[0].position)
            i++;
        i--;
        int line = i+1;
        int row = tokens[0].position - newlines[i] + 1;
        osstream ss;
        ss << "LEXICAL ERROR: file " << filename << ", line " << line << ", row " << row << ": " << tokens[0].text;
        error = ss.str();
        return;
    }
    if(SHOW_TOKENS) {
        if(SHOW_HEADERS)
            cout << "-- TOKENS --" << endl;
        cout << Token::printAll(tokens);
        if(SHOW_HEADERS)
            cout << endl;
    }

    for(int i=0; i<7; i++)
        cout << SyntaxNode::PRINTS[i] << endl;

    tree = parse::file(tokens);
    if(SHOW_SYNTAX_TREE) {
        if(SHOW_HEADERS)
            cout << "-- SYNTAX TREE --" << endl;
        cout << tree->toString();
        if(SHOW_HEADERS)
            cout << endl;
    }

    if(SHOW_EVAL) {
        if(SHOW_HEADERS)
            cout << "-- EVALUATION --" << endl;
        EvalContext ctx;
        ctx.error = nullptr;
        evalStatement(ctx, tree);
        if(ctx.error != nullptr)
            cout << "EVALUATION ERROR: " << ctx.error->msg << endl;
        if(SHOW_HEADERS)
            cout << endl;
    }
}

File::~File() {
    if(tree != nullptr)
        delete tree;
}