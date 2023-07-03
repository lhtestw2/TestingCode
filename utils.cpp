#include <vector>
#include <string>
#include <windows.h>
#include <filesystem>
#include "utils.hpp"


namespace FileExtract{
namespace util{
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
    auto u8_path = std::filesystem::u8path(path);
    for (const auto &sub_file : std::filesystem::directory_iterator(u8_path)) {
        auto relative_path = std::filesystem::relative(sub_file, path).generic_u8string();
        if (sub_file.is_regular_file()) {
            res.push_back(relative_path);
        }
    }
    return res;
}

}
}
