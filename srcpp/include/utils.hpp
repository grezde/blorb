#pragma once
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
const char endl = '\n';
using std::cout;

string* readFile(const string& filename);
vector<string> readArgs(int argc, const char** argv);
void mainLoop(int argc, const char** argv);
