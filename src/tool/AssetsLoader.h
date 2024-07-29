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

    std::string loadCodeFromFile(const std::string& filePath) const;

private:
    static std::string getAssetsPath()
    {
        return "./../assets/";
    }
};