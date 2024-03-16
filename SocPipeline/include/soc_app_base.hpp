#pragma once

#include "soc_window.hpp"

namespace soc{

    class SocAppBase{
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

        private:
        SocWindow socWindow{WIDTH,HEIGHT,"Hello Vulkan!"};

    };
}