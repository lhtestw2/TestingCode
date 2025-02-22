#include <filesystem>
#include <iostream>
#include <fstream>
#include "utils.hpp"
#include "parser.hpp"
#include "solver.hpp"

#ifdef _DEBUG
#pragma comment(lib,"C:\\Users\\haol\\mupdf\\platform\\win32\\Debug\\libmupdf.lib")
#pragma comment(lib,"C:\\Users\\haol\\mupdf\\win32\\Debug\\libthirdparty.lib")
#else
#pragma comment(lib,"C:\\Users\\haol\\mupdf\\platform\\win32\\Release\\libmupdf.lib")
#pragma comment(lib,"C:\\Users\\haol\\mupdf\\platform\\win32\\Release\\libthirdparty.lib")
#endif


int main(int argc, char **argv)
{
    FileExtract::Parser parser("./punctuation.txt", "./keys.txt");
    std::filesystem::path pdf_path_u8 = std::filesystem::u8path("C:\\Users\\haol\\download\\需要用到的文件");
    std::string prefix("./files_nospace");
    // std::filesystem::create_directory(std::filesystem::u8path(prefix));
    // for (const auto &file : std::filesystem::directory_iterator(pdf_path_u8)) {
    //     auto abs_path = file.path().generic_u8string();
    //     auto file_name = file.path().filename().generic_u8string();
    //     std::string pdf2txt_path = prefix + "/" + file_name.substr(0, file_name.length() - 4) + ".txt";
    //     std::ofstream output_file;
    //     output_file.open(pdf2txt_path);
    //     parser.pdf_extract(abs_path, output_file);
    //     output_file.close();  
    // }

    for (int i = 3; i < 7; ++i) {
        FileExtract::Solver readfile(prefix, 0.1 * i);
        readfile.first_read(parser);
        // readfile.print_res();
        readfile.second_read(parser);
        // readfile.print_res();
        readfile.store_res(std::string(prefix) + std::to_string(i) + std::string(".csv"));
    }
	
    // std::cout << std::to_string(5) << std::endl;
    std::cout << "done" << std::endl;
	return 0;

}