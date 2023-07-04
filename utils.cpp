#include <vector>
#include <string>
#include <windows.h>
#include <filesystem>
#include "utils.hpp"


namespace FileExtract{
namespace util{
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
