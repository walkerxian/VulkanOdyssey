#include "soc_pipeline.hpp"

//std
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>


namespace soc{

    SocPipeline::SocPipeline(const std::string& vertFilepath,const std::string& fragFilepath){

        createGraphicsPipeline(vertFilepath,fragFilepath);
    }

    SocPipeline::~SocPipeline()
    {

    }

void SocPipeline::createGraphicsPipeline(
  
  const std::string& vertFilepath, const std::string& fragFilepath) {

  //std::cout << "Current path is " << std::filesystem::current_path() << std::endl;
    auto vertCode = readFile(vertFilepath);
    auto fragCode = readFile(fragFilepath);

   std::cout << "Vertex Shader Code Size: " << vertCode.size() << '\n';
   std::cout << "Fragment Shader Code Size: " << fragCode.size() << '\n';
}


std::vector<char> SocPipeline::readFile(const std::string& filename) {
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

// std::vector<char> SocPipeline::readFile(const std::string& filename) {
//   std::ifstream file{filename, std::ios::ate | std::ios::binary};

//   if (!file.is_open()) {
//     throw std::runtime_error("failed to open file: " + filename);
//   }

//   size_t fileSize = (size_t)file.tellg();
//   std::vector<char> buffer(fileSize);

//   file.seekg(0);
//   file.read(buffer.data(), fileSize);

//   file.close();

//   return buffer;
// }



// std::vector<char> SocPipeline::readFile(const std::string& filepath) {
//   std::ifstream file{filepath, std::ios::ate | std::ios::binary};

//   if (!file.is_open()) {
//     std::cerr << "failed to open file: " << filepath << std::endl;
//     throw std::runtime_error("failed to open file: " + filepath);
//   }

//   size_t fileSize = static_cast<size_t>(file.tellg());
//   if (fileSize == -1) {
//     std::cerr << "failed to read the file size: " << filepath << std::endl;
//     throw std::runtime_error("failed to read the file size");
//   }

//   std::vector<char> buffer(fileSize);

//   file.seekg(0);
//   if (!file) {
//     std::cerr << "failed to seek to the beginning of the file: " << filepath << std::endl;
//     throw std::runtime_error("failed to seek to the beginning of the file");
//   }

//   file.read(buffer.data(), fileSize);
//   if (!file) {
//     std::cerr << "failed to read the file: " << filepath << std::endl;
//     throw std::runtime_error("failed to read the file");
//   }

//   file.close();
//   return buffer;
// }



}