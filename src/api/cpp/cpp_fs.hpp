#include <string>

namespace cppFs {
    std::string getcwd();

    std::string readFile(const std::string &filePath);

    std::string absolute(const std::string &relativePath, const std::string &basePath);

    std::string parent(const std::string &path);
}