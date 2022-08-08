#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using std::string;
using std::vector;
const char endl = '\n';
using std::cout;
typedef std::ostringstream osstream;
typedef std::istringstream isstream;

vector<string> readArgs(int argc, const char** argv);
