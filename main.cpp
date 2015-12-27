#include "engine/Engine.h"

#include <iostream>
#include <fstream>
#include <string>

bool readFile(const std::string& file, std::string& ret)
{
    std::ifstream ifs(file);
    if (!ifs)
    {
        std::cerr << "[FATAL] unable to open file '" << file << "'.\n";
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ret = content;
    ifs.close();

    return true;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        InteractiveEngine ie;
        ie.run();
        return 0;
    }

    std::string input;
    if (readFile(std::string(argv[1]), input)) {
        StringEngine se(input);
        se.run();
    }

    return 0;
}
