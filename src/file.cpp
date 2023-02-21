#include "file.hpp"
#include <fstream>
#include "typecheck.hpp"
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

    std::ofstream fout("debug.txt");

    if(SHOW_CONTENTS) {
        fout << "-- FILE CONTENTS --" << endl;
        fout << contents << endl << endl;
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
    fout << "-- TOKENS --" << endl;
    fout << Token::printAll(tokens) << std::endl;

    tree = parse::file(tokens);
    fout << "-- SYNTAX TREE --" << endl;
    fout << tree->toString() << std::endl;
    fout.close();

    SemanticContext sctx;
    typecheck::statement(sctx, tree);
    if(sctx.ers.size() > 0) {
        for(SemanticContext::Error er : sctx.ers)
            cout << er.msg << endl;
        return;
    }
    
    EvalContext ectx;
    ectx.error = nullptr;
    eval::statement(ectx, tree);
    if(ectx.error != nullptr)
        cout << "EVALUATION ERROR: " << ectx.error->msg << endl;
    
}

File::~File() {
    if(tree != nullptr)
        delete tree;
}