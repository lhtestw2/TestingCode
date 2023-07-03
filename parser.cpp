#include <string>
#include <map>
#include <fstream>
#include "parser.hpp"
#include "mupdf/fitz.h"
#include "mupdf/fitz/util.h"
#include <algorithm>

namespace FileExtract {
Parser::Parser(std::string split_path, std::string key_path) {
    std::ifstream infile;
    infile.open(split_path);
    std::string str;
    while (std::getline(infile, str)) {
        if (str.length()) {
            splits_.insert({str, str.length()});
        }
    }
    infile.close();
    infile.open(key_path);
    while(std::getline(infile, str)) {
        if (str.length()) {
            keys_.push_back(str);
        }
    }
    infile.close();
}
Parser::Parser(const Parser &other) : keys_(other.keys_), splits_(other.splits_) {}

size_t Parser::match(const std::string &src, const std::string key, size_t start) {
    auto len = src.length();
    if (len <= start) { return -1; }
    int j;
    int len_key = key.length();
    for (int i = start; i < len; ++i)
    {
        for (j = 0; j < len_key; ++j) {
            if (key[j] != src[i + j]) { 
                break; 
            }
        }
        if (j == len_key) { return i; }
    }
    return -1;
}
std::string Parser::parser(const std::string &src) const{
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
        for (const auto &key : splits_) {
            size_t sub_start = 0;
            size_t sub_end = 0;

            while((sub_end = match(sub_string, key.first, sub_start)) != -1) {
                sub_end += key.second;
                sub_string.insert(sub_string.begin() + sub_end, '\n');
                sub_start = sub_end + 1;
            }
        }
        res += sub_string;

        start = end + 1; // drop '\n'
    }
    return res + src.substr(start, src.length() - start);
}

void Parser::pdf_extract(std::string path, std::ofstream &output_file) const {
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
        auto *char_buffer = reinterpret_cast<char *>(path_buffer);
        std::string path_string(char_buffer);
        auto res = parser(path_string);
        output_file << res;
    }
    fz_drop_pixmap(ctx, pix);
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);
}

bool Parser::is_question(const std::string &str) const {
    return std::any_of(keys_.cbegin(), keys_.cend(), [&str](const std::string &key) {
        return Parser::match(str, key, 0) != -1;
    });
}

} // end namespace FileExtract




    
