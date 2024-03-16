-- 设置编译器工具链
set_toolset("cc", "D:\\_iWorks\\_RenderPipeline\\Tools\\mingw64\\bin\\gcc.exe")
set_toolset("cxx", "D:\\_iWorks\\_RenderPipeline\\Tools\\mingw64\\bin\\g++.exe")
set_toolset("ld", "D:\\_iWorks\\_RenderPipeline\\Tools\\mingw64\\bin\\g++.exe")
set_toolset("ar", "D:\\_iWorks\\_RenderPipeline\\Tools\\mingw64\\bin\\ar.exe")
set_toolset("sh", "D:\\_iWorks\\_RenderPipeline\\Tools\\mingw64\\bin\\g++.exe")


-- 设置 Vulkan SDK、GLFW 和 GLM 的路径
local vulkan_sdk_path = "D:\\Program Files\\VulkanSDK\\1.3.275.0"
local glfw_sdk_path = "D:\\_iWorks\\_RenderPipeline\\Vulkan\\Plugins\\glfw-3.3.9.bin.WIN64"
local glm_path = "D:\\Program Files\\VulkanSDK\\1.3.275.0\\Include\\glm" 

-- 添加包含目录
add_includedirs(vulkan_sdk_path .. "\\Include")
add_includedirs(glfw_sdk_path .. "\\include")
add_includedirs(glm_path)

-- 添加链接目录
add_linkdirs(vulkan_sdk_path .. "\\Lib")
add_linkdirs(glfw_sdk_path .. "\\lib-mingw-w64")

-- 添加链接依赖项
add_links("glfw3", "vulkan-1")

-- 添加系统链接库
add_syslinks("user32", "gdi32", "shell32")




target("SocPipeline")
    set_kind("binary")
    add_files("src/*.cpp") -- 添加源文件
    add_includedirs("include")
    -- 设置编译选项
    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        set_symbols("hidden")
        set_optimize("fastest")
        set_strip("all")
    end

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

