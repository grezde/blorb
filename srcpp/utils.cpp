#include "utils.hpp"
#include <fstream>

vector<string> readArgs(int argc, const char** argv) {
    vector<string> args;
    for(int i=1; i<argc; i++)
        args.push_back(string(argv[i]));
    return args;
}
