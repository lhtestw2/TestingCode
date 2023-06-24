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
#include "mupdf/fitz.h"
#include "mupdf/fitz/util.h"
#include <filesystem>

#ifdef _DEBUG
#pragma comment(lib,"D:\\mupdf\\mupdf\\platform\\win32\\Debug\\libmupdf.lib")
#pragma comment(lib,"D:\\mupdf\\mupdf\\platform\\win32\\Debug\\libthirdparty.lib")
#else
#pragma comment(lib,"D:\\mupdf\\mupdf\\platform\\win32\\Release\\libmupdf.lib")
#pragma comment(lib,"D:\\mupdf\\mupdf\\platform\\win32\\Release\\libthirdparty.lib")
#endif

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


class Parser {
public:
    Parser(std::string path) {
        std::ifstream infile;
        infile.open(path);
        std::string str;
        while (std::getline(infile, str)) {
            if (str.length()) {
                keys_.insert({str, str.length()});
            }
        }
        infile.close();
        path_ = std::move(path);
    }
    static size_t match(const std::string &src, const std::string key, size_t start) {
        auto len = src.length();
        if (len <= start) { return -1; }
        int j;
        int len_key = key.length();
        for (int i = start; i < len; ++i)
        {
            for (j = 0; j < len_key; ++j) {
                if (key[j] != src[i + j]) { break; }
            }
            if (j == len_key) { return i; }
        }
        return -1;
    }

    std::string parser(const std::string &src) {
        std::string res;
        size_t start = 0;
        size_t end = 0;
        while ((end = match(src, std::string("\n"), start)) != -1)
        {
            if (start >= end) {
                start += 1;
                continue;
            }
            auto sub_string = src.substr(start, end - start); 
            for (const auto &key : keys_) {
                size_t sub_start = 0;
                size_t sub_end = 0;

                while((sub_end = match(sub_string, key.first, sub_start)) != -1) {
                    sub_end += key.second;
                    sub_string.insert(sub_string.begin() + sub_end, '\n');
                    sub_start = sub_end + 1;
                }
            }
            res += sub_string;

            start = end + 1;
        }
        return res + src.substr(start, src.length() - start);
    }
    
    void pdf_extract(std::string path, std::ofstream &output_file) {
        float zoom, rotate;
        int page_number, page_count;
        fz_context *ctx;
        fz_document *doc;
        fz_pixmap *pix;
        fz_matrix ctm;
        int x, y;

        /* Create a context to hold the exception stack and various caches. */
        ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
        if (!ctx)
        {
            fprintf(stderr, "cannot create mupdf context\n");
            exit(-1);
        }

        /* Register the default file types to handle. */
        fz_try(ctx)
            fz_register_document_handlers(ctx);
        fz_catch(ctx)
        {
            fprintf(stderr, "cannot register document handlers: %s\n", fz_caught_message(ctx));
            fz_drop_context(ctx);
            exit(-1);
        }

        /* Open the document. */
        fz_try(ctx)
            doc = fz_open_document(ctx, path.c_str());
        fz_catch(ctx)
        {
            fprintf(stderr, "cannot open document: %s\n", fz_caught_message(ctx));
            fz_drop_context(ctx);
            exit(-1);
        }

        /* Count the number of pages. */
        fz_try(ctx)
            page_count = fz_count_pages(ctx, doc);
        fz_catch(ctx)
        {
            fprintf(stderr, "cannot count number of pages: %s\n", fz_caught_message(ctx));
            fz_drop_document(ctx, doc);
            fz_drop_context(ctx);
            exit(-1);
        }

        fz_stext_options fz_options = {0, 0};
        for (int i = 0; i < page_count; ++i) {
            auto path_buffer = fz_new_buffer_from_page_number(ctx, doc, i, &fz_options)->data;
            char *char_buffer = (char *)(path_buffer);
            std::string path_string(char_buffer);
            // auto GB_string = UTF8TOGB(path_string);
            // std::cout << GB_string << std::endl;
            // auto res = UTF8TOGB(parser(path_string));
            auto res = parser(path_string);
            // std::cout << res << std::endl;
            output_file << res;
        }
        fz_drop_pixmap(ctx, pix);
	    fz_drop_document(ctx, doc);
	    fz_drop_context(ctx);
    }

private:
    std::map<std::string, int> keys_;
    std::string path_;
};

class Question {
public:
    Question(std::string path) {
        std::ifstream infile;
        infile.open(path);
        std::string str;
        while (std::getline(infile, str)) {
            if (str.length()) {
                // keys_.push_back(UTF8TOGB(str));
                keys_.push_back(str);
            }
        }
        // for (auto &p : keys_) {
        //     std::cout << p << std::endl;
        // }
        infile.close();
        path_ = std::move(path);
    }
    bool is_question(const std::string &str) {

        return std::any_of(keys_.cbegin(), keys_.cend(), [&str](const std::string &key) {
            return Parser::match(str, key, 0) != -1;
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
            std::string abs_path = path_ + std::string("/") + p;
            infile.open(abs_path);
            std::string str;
            while (std::getline(infile, str)) {
                if (str.length() == 0) { continue; }
                // str = UTF8TOGB(str);
                if (question.is_question(str)) {
                    bool flag = false;
                    for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {  // std::pair<size_t, std::map<std::string, std::set<std::string>>>
                        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {  // std::pair<std::string, std::set<std::string>>
                            for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                                if (StringSimilarity(*it3, str) >= thresh_) {
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
    void second_read(Question &question) {
        std::ifstream infile;
        for (const auto &p : path_list_) {
            std::string abs_path = path_ + std::string("/") + p;
            infile.open(abs_path);
            std::string str;
            while (std::getline(infile, str)) {
                if (str.length() == 0) { continue; }
                // str = UTF8TOGB(str);
                bool flag = false;
                for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {  // std::pair<size_t, std::map<std::string, std::set<std::string>>>
                    for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {  // std::pair<std::string, std::set<std::string>>
                        for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                            if (StringSimilarity(*it3, str) >= thresh_) {
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
    void store_res(std::string res_path, int thresh = 2) {
        std::ofstream res_file;
        res_file.open(res_path);
        for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {
            if (it1->second.size() < thresh) { continue; }
            for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
                for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                    auto item = *it3;
                    res_file << it1->first << "," << it2->first << "," << UTF8TOGB(item) << "\n";
                }
            }
        }
        res_file.close();
    }
    void print_res() {
        for (auto it1 = res_.begin(); it1 != res_.end(); ++it1) {
            std::cout << it1->first << ":" << std::endl;
            for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
                std::cout << "\t" << it2->first << ":" << std::endl;
                for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                    auto item = *it3;
                    std::cout << "\t\t" << UTF8TOGB(item) << std::endl;
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

// class PDFReader {
//     PDFReader()
// }

int main(int argc, char **argv)
{
    // Parser parser("./punctuation.txt");
    // std::string pdf_path("D:\\download\\used_files");
    // std::filesystem::path pdf_path_u8 = std::filesystem::u8path(pdf_path);
    // for (const auto &file : std::filesystem::recursive_directory_iterator(pdf_path_u8)) {
    //     auto abs_path = file.path().generic_string();
    //     auto file_name = file.path().filename().generic_string();
    //     std::string pdf2txt_path = "./files_nospace/" + file_name.substr(0, file_name.length() - 4) + ".txt";
    //     std::ofstream output_file;
    //     output_file.open(pdf2txt_path);
    //     parser.pdf_extract(abs_path, output_file);
    //     output_file.close();  
    // }

    for (int i = 1; i < 7; ++i) {
        Question question("./keys.txt");
        Solver readfile(std::string("./files_nospace"), 0.1 * i);
        readfile.first_read(question);
        // readfile.print_res();
        // readfile.second_read(question);
        // readfile.print_res();
        readfile.store_res(std::string("./res_nospace_") + std::to_string(i) + std::string(".csv"));
    }
	
    // std::cout << std::to_string(5) << std::endl;
    std::cout << "done" << std::endl;
	return 0;


}