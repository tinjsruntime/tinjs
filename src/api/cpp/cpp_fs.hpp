#ifndef CPP_FS_HPP
#define CPP_FS_HPP
#include <string>

std::string _getcwd();
std::string readFile(const std::string& filePath);
std::string absolute(const std::string& relativePath, const std::string& basePath);
std::string parent(const std::string& path);

#endif