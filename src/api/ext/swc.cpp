#include "swc.hpp"
#include <iostream>

namespace swc
{
    std::string runcmdandreturnout(std::string cmd)
    {
        std::string data;
        FILE *stream;
        const int max_buffer = 256;
        char buffer[max_buffer];
        cmd.append(" 2>&1");

        stream = popen(cmd.c_str(), "r");
        if (stream)
        {
            while (!feof(stream))
                if (fgets(buffer, max_buffer, stream) != NULL)
                    data.append(buffer);
            pclose(stream);
        }
        return data;
    }

    std::string transpile(std::string binPath, std::string filename)
    {
        std::string cmd = binPath + " compile " + filename;
        std::string output = runcmdandreturnout(cmd);
        output.erase(0, output.find("\n") + 1);
        return output;
    }
}