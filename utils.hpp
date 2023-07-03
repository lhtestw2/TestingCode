#include <vector>
#include <string>
#pragma once 


namespace FileExtract{
namespace util{
template <class T>
int getEditDistance(T &first, T &second);

template <class T>
double StringSimilarity(const T &first, const T &second);

std::string UTF8TOGB(std::string &str);
std::vector<std::string> walk_through(const std::string &path);
}
}