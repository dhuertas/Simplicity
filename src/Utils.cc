#include "Utils.hh"

//------------------------------------------------------------------------------
// Splits a string given a delimiter and returns a vector of substrings
vectstr_t split(std::string str, char delim) {

  vectstr_t elems;
  std::stringstream ss(str);
  std::string item;

  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }

  return elems;
}

//------------------------------------------------------------------------------
// Trim a string left and right
std::string &trim(std::string &s) {
  
  while (s.at(0) == ' ') {
    s.erase(0, 1);
  }

  while (s.at(s.length() - 1) == ' ') {
    s.erase(s.length() - 1, 1);
  }

  return s;
}