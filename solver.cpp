#include "solver.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>

namespace FileExtract {

Solver::Solver(std::string path, double thresh) : thresh_(thresh) {
    path_list_ = FileExtract::util::walk_through(path);
    path_ = std::move(path);
}

void Solver::first_read(const Parser &parser) {
    std::ifstream infile;
    for (const auto &p : path_list_) {
        std::string abs_path = path_ + "/" + p;
        infile.open(abs_path);
        std::string str;
        while (std::getline(infile, str)) {
            if (str.length() == 0) { continue; }
            if (parser.is_question(str)) {
                bool flag = false;
                for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {  // std::pair<size_t, std::map<std::string, std::set<std::string>>>
                    for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {  // std::pair<std::string, std::set<std::string>>
                        for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                            if (FileExtract::util::StringSimilarity(*it3, str) >= thresh_) {
                                it1->second[p].insert(str);
                                flag = true;
                                break;
                            }
                        }
                        if (flag) { break; }
                    }
                    if (flag) { break; }
                }
                if (!flag) {
                    res_[res_.size()][p].insert(str);
                }
            }
        }
        infile.close();
    }
}
    
void Solver::second_read(const Parser &parser) {
    std::ifstream infile;
    for (const auto &p : path_list_) {
        std::string abs_path = path_ + "/" + p;
        infile.open(abs_path);
        std::string str;
        while (std::getline(infile, str)) {
            if (str.length() == 0) { continue; }
            bool flag = false;
            for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {  // std::pair<size_t, std::map<std::string, std::set<std::string>>>
                for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {  // std::pair<std::string, std::set<std::string>>
                    for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                        if (FileExtract::util::StringSimilarity(*it3, str) >= thresh_) {
                            it1->second[p].insert(str);
                            flag = true;
                            break;
                        }
                    }
                    if (flag) { break; }
                }
                if (flag) { break; }
            }             
        }
        infile.close();
    }
}

void Solver::store_res(std::string res_path, int thresh) {
    std::ofstream res_file;
    res_file.open(res_path);
    for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {
        if (it1->second.size() < thresh) { continue; }
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                auto item = *it3;
                res_file << it1->first << "," << it2->first << "," << FileExtract::util::UTF8TOGB(item) << "\n";
            }
        }
    }
    res_file.close();
}
    
void Solver::print_res() {
    for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {
        std::cout << it1->first << ":" << std::endl;
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            std::cout << "\t" << it2->first << ":" << std::endl;
            for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                auto item = *it3;
                std::cout << "\t\t" << FileExtract::util::UTF8TOGB(item) << std::endl;
            } 
        }
    }
}

} // end namespace 