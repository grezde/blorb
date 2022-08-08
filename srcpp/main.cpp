#include <iostream>
#include "file.hpp"

int main(int argc, const char** argv) {
    auto args = readArgs(argc, argv);
    if(args.size() != 1) {
        cout << "COMPILER ERROR: Wrong usage." << endl;
        return 1;
    }
    File f(args[0]);
    if(f.error.length() != 0)
        cout << f.error << endl;
    return 0;
}