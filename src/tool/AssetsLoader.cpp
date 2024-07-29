#include "AssetsLoader.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::string AssetsLoader::loadCodeFromFile(const std::string& filePath) const
{
    std::string newPath = getAssetsPath() + filePath;

    std::ifstream in(newPath, std::ios::in);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if (!in)
    {
        std::cerr << "Reading file: " + newPath << "error is: " << errno << std::endl;
        std::abort();
    }

    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}