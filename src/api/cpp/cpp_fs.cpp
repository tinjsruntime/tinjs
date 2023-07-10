#include <iostream>
#include <fstream>
#include "cpp_fs.hpp"
#include <unistd.h>
#include <filesystem>

std::string _getcwd()
{
    return std::filesystem::current_path();
}

// /x/smthn/b.js -> /x/smthn
std::string parent(const std::string &filename)
{
    return std::filesystem::path(filename).parent_path().string();
}

// https://github.com/SenkoraJS/senkora/main/src/api/Senkora.cpp#L63C1-L79C1
std::string readFile(const std::string &filePath)
{
    std::ifstream file(filePath);

    if (!file.good())
        throw std::runtime_error("File not found");

    std::stringstream ss;

    ss << file.rdbuf();

    std::string out = ss.str();

    file.close();

    return out;
}

std::string absolute(const std::string &relativePath, const std::string &basePath)
{
    auto fullPath = std::filesystem::path(basePath + "/" + relativePath).lexically_normal();
    return fullPath.string();
}