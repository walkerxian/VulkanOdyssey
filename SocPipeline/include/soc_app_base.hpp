#pragma once

#include "soc_window.hpp"
#include "soc_pipeline.hpp"

namespace soc{

    class SocAppBase{
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

        private:
        SocWindow socWindow{WIDTH,HEIGHT,"Hello Vulkan!"};
        SocPipeline pipeline{"shaders\\simple_shader.vert.spv", "shaders\\simple_shader.frag.spv"};
    };
}