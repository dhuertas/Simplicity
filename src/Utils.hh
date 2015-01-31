#ifndef __UTILS_HH__
#define __UTILS_HH__

#include <sstream>
#include <string>
#include <vector>

// Vector of strings
typedef std::vector<std::string> vectstr_t;

// Splits a string given a delimiter and returns a vector of substrings
vectstr_t split(std::string str, char delim);

// Trim a string left and right
std::string &trim(std::string &s);

#endif