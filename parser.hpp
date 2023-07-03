#include <string>
#include <map>
#include <vector>
#pragma once 

namespace FileExtract {
class Parser {
public:
    /**
     * Get the split chatacters.
    */
    Parser(std::string split_path, std::string key_path);
    Parser(const Parser &);

    static size_t match(const std::string &src, const std::string key, size_t start);

    std::string parser(const std::string &src) const;
    
    void pdf_extract(std::string path, std::ofstream &output_file) const;

    bool is_question(const std::string &str) const;

private:
    std::vector<std::string> keys_;
    std::map<std::string, int> splits_;
};

}