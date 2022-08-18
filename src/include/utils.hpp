#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <array>

using std::string;
using std::vector;
const char endl = '\n';
using std::cout;
using std::map;
using std::pair;
using std::array;
typedef std::ostringstream osstream;
typedef std::istringstream isstream;

vector<string> readArgs(int argc, const char** argv);
