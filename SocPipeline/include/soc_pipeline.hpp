#pragma once

#include <string>
#include <vector>

namespace soc{
    class SocPipeline
    {
 
    public:
        SocPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
        ~SocPipeline();

    private:
        static std::vector<char> readFile(const std::string& filename);
        void createGraphicPipeline(const std::string& vertFilepath,const std::string& fragFilepath);
        
    };
    
}