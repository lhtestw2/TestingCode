/**
 * Test about factory mode.
 */
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



#define TEST 1

class ATest {
public:
    float a;
    int b;
    ATest() : a(1.0), b(2) {}
};

class BTest : virtual public ATest {
public:
    float c;
    BTest(float c) : ATest(), c(c) {}
};

class MoreTest {
    int a;
    double b;
    MoreTest() : a(1), b(2.2) {}
    void print() { std::cout << "this is a modify print" << std::endl; }
    void print2() {}
    void print3() {}
    void print1() {}
    void add_func() {}
    void add_func1() { std::cout << "This is a test about snap." << std::endl; }
};


template <class T>
int getEditDistance(T &first, T &second) {
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
double StringSimilarity(T &first, T &second) {
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


std::vector<std::string> walk_through(std::string path) {
    std::vector<std::string> res;
    std::string findFile = path + std::string("\\*");
    _finddata_t FileInfo;
    long handle_ = _findfirst(findFile.c_str(), &FileInfo);
    do {
        auto file_name = std::string(FileInfo.name);
        if (file_name != ".." && file_name != ".") {
            res.push_back(path + std::string("\\") + file_name);
        }
    } while (_findnext(handle_, &FileInfo) == 0);
    return res;
}


class Question {
public:
    Question(std::string &path) {
        std::ifstream infile;
        infile.open(path);
        std::string str;
        while (std::getline(infile, str)) {
            if (str.length()) {
                keys.push_back(UTF8TOGB(str));
            }
        }
        for (auto &p : keys) {
            std::cout << p << std::endl;
        }
    }
    bool is_question(std::string str) {

        return std::any_of(keys.cbegin(), keys.cend(), [&str](const std::string key) {
            return str.find(key) != std::string::npos;
        });
    }
private:
    std::vector<std::string> keys;
};


int main()
{
    // ATest atest;
    // std::cout << atest.a << ' ' << atest.b << std::endl;
    // std::cout << 2 << std::endl;
    // BTest btest(1.2);
    // std::cout << btest.a << ' ' << btest.b << ' ' << btest.c << std::endl;
    std::string path("files");
    std::cout << path << std::endl;
    auto files = walk_through(path);
    for (auto &p : files) {
        std::cout << p << std::endl;
    }
    std::map<int, std::map<std::string, std::string>> res;
    std::ifstream infile;
    infile.open("./files/file1.txt");
    std::string str;
    std::string key_path("./keys.txt");
    Question q(key_path);
    while (std::getline(infile, str))
    {
    //    int s = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    //    std::cout << s << std::endl;
       std::cout << str << std::endl;
       std::cout << UTF8TOGB(str) << std::endl;
       std::cout << (q.is_question(UTF8TOGB(str)) ? "True" : "False") << std::endl;
    }
    

    std::wstring s(L"中国");
    // std::wstring
    // std::cout << s << std::endl;
    std::cout << s.length() << std::endl;
    // std::cout << s << std::endl;
    // // std::cout << ws << std::endl;
    // char *p = R"中国";
    // std::cout << p << std::endl;
    std::wstring s1(L"中国人 ");
    std::wstring s2(L"中国 ");
    std::cout << StringSimilarity(s1, s2) << std::endl;
    return 0;
}