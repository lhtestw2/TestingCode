#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <atlstr.h>
#include <map>
#include <fstream>
#include <windows.h>
#include <io.h>
#include <regex>
#include <set>



template <class T>
int getEditDistance(const T &first, const T &second) {
    int m = first.length();
    int n = second.length();
    std::vector<std::vector<size_t>> record(m + 1, std::vector<size_t>(n + 1, 0));
    for (int i = 0; i <= m; ++i) {
        record[i][0] = i;
    }
    for (int j = 0; j <= n; ++j) {
        record[0][j] = j;
    }
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            size_t weight = first[i - 1] == second[j - 1] ? 0 : 1;
            record[i][j] = min(min(record[i - 1][j] + 1, record[i][j - 1] + 1), 
            record[i - 1][j - 1] + weight);
        }
    }
    return record[m][n];
}

template <class T>
double StringSimilarity(const T &first, const T &second) {
    double max_len = max(first.length(), second.length());
    if (max_len > 0) {
        return (max_len - getEditDistance(first, second)) / max_len;
    }
    return 1;
}

std::string UTF8TOGB(std::string &str) {
    int i = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    WCHAR *strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, strSrc, i);

    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    LPSTR szRes = new CHAR[i + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);
    std::string result = szRes;
    delete []strSrc;
    delete []szRes;
    return result;
}


std::vector<std::string> walk_through(const std::string &path) {
    std::vector<std::string> res;
    std::string findFile = path + std::string("\\*");
    _finddata_t FileInfo;
    long handle_ = _findfirst(findFile.c_str(), &FileInfo);
    do {
        auto file_name = std::string(FileInfo.name);
        if (file_name != ".." && file_name != ".") {
            // res.push_back(path + std::string("\\") + file_name);
            res.push_back(file_name);
        }
    } while (_findnext(handle_, &FileInfo) == 0);
    return res;
}



class Question {
public:
    Question(std::string path) {
        std::ifstream infile;
        infile.open(path);
        std::string str;
        while (std::getline(infile, str)) {
            if (str.length()) {
                keys_.push_back(UTF8TOGB(str));
            }
        }
        // for (auto &p : keys_) {
        //     std::cout << p << std::endl;
        // }
        path_ = std::move(path);
    }
    bool is_question(const std::string &str) {

        return std::any_of(keys_.cbegin(), keys_.cend(), [&str](const std::string key) {
            return str.find(key) != std::string::npos;
        });
    }
private:
    std::vector<std::string> keys_;
    std::string path_;
};

class Solver {
public:
    Solver(std::string path, double thresh = 0.5) : thresh_(thresh) {
        path_list_ = std::move(walk_through(path));
        path_ = std::move(path);
    }

    void first_read(Question &question) {
        std::ifstream infile;
        for (const auto &p : path_list_) {
            std::string abs_path = path_ + std::string("\\") + p;
            infile.open(abs_path);
            std::string str;
            while (std::getline(infile, str)) {
                str = UTF8TOGB(str);
                if (question.is_question(str)) {
                    bool flag = false;
                    for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {  // std::pair<size_t, std::map<std::string, std::set<std::string>>>
                        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {  // std::pair<std::string, std::set<std::string>>
                            for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                                if (StringSimilarity(*it3, str) >= thresh_) {
                                    it2->second.insert(str);
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
    void print_res() {
        for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {
            std::cout << it1->first << ":" << std::endl;
            for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
                std::cout << "\t" << it2->first << ":" << std::endl;
                for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                    std::cout << "\t\t" << *it3 << std::endl;
                } 
            }
        }
    }
private:
    double thresh_;
    std::string path_;
    std::vector<std::string> path_list_;
    std::map<size_t, std::map<std::string, std::set<std::string>>> res_;
};


int main()
{

    std::string path("files");
    std::string key_path("./keys.txt");
    Question question(key_path);
    Solver readfile(path);
    readfile.first_read(question);
    readfile.print_res();

    return 0;
}