#pragma once
#include "parse.hpp"

struct File {

    static const bool SHOW_HEADERS = false;
    static const bool SHOW_CONTENTS = true;
    static const bool SHOW_TOKENS = true;
    static const bool SHOW_SYNTAX_TREE = true; 

    File(const string filename);
    ~File();

    const string filename;
    string error;
    string contents;
    vector<int> newlines;
    vector<Token> tokens;
    SyntaxNode* tree;

    static string* readContents(const string& filename);

};