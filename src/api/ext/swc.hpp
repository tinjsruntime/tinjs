#pragma once
#include <string>

namespace swc {
    std::string transpile(std::string filename);
    void initTs(std::string path);

    std::string runcmdandreturnout(std::string cmd);
    bool tsIsInstalled();
}