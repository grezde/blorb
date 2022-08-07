#include "utils.hpp"
#include <fstream>
#include "token.hpp"

string* readFile(const string& filename) {
    std::ifstream fin(filename);
    if(!fin.is_open())
        return nullptr;
    string* s = new string();
    while(!fin.eof())
        s->push_back(fin.get());
    return s;
}

vector<string> readArgs(int argc, const char** argv) {
    vector<string> args;
    for(int i=1; i<argc; i++)
        args.push_back(string(argv[i]));
    return args;
}

void mainLoop(int argc, const char** argv) {
    auto args = readArgs(argc, argv);
    if(args.size() != 1) {
        cout << "COMPILER ERROR: Wrong usage." << endl;
        exit(1);   
    }
    string* fc = readFile(args[0]);
    if(fc == nullptr) {
        cout << "COMPILER ERROR: File " << args[0] << "could not be opened." << endl;
        exit(1); 
    }
    vector<Token> ts = Token::all(*fc);
    cout << Token::printAll(ts);
    delete fc;
}
