#include <string>
#include "parser.hpp"
#include <vector>
#include <map>
#include <set>
#pragma once 

namespace FileExtract {
class Solver {
public:
    Solver(std::string path, double thresh = 0.5);

    void first_read(const Parser &parser);
    
    void second_read(const Parser &parser);

    void store_res(std::string res_path, int thresh = 2);

    void print_res();

private:
    double thresh_;
    std::string path_;
    std::vector<std::string> path_list_;
    std::map<size_t, std::map<std::string, std::set<std::string>>> res_;
};
}