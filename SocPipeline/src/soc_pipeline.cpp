

#include "soc_pipeline.hpp"

//std
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace soc{

    SocPipeline::SocPipeline(const std::string& vertFilepath,const std::string& fragFilepath){

        createGraphicPipeline(vertFilepath,fragFilepath);
    }


    void SocPipeline::createGraphicPipeline(const std::string& vertFilepath,const std::string& fragFilepath)
    {
        auto vertCode = readFile(vertFilepath);
        auto fragCode = readFile(fragFilepath);

        std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
        std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';
    }


    std::vector<char> SocPipeline::readFile(const std::string& filename){

        std::ifstream file{filename, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;

    }


}