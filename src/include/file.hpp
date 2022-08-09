#pragma once
#include "parse.hpp"

struct File {

    static const bool SHOW_CONTENTS = true;

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