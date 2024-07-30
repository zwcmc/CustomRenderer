#pragma once

#include <iostream>
#include <string>

class AssetsLoader
{
public:
    static AssetsLoader& getInstance()
    {
        static AssetsLoader instance;
        return instance;
    }

    static std::string getAssetsPath()
    {
        return "./../assets/";
    }

    std::string loadCodeFromFile(const std::string &filePath) const;
};