@echo off
@rem compile shaders using glslc
@rem glslc path is "C:/VulkanSDK/1.4.328.1/Bin/glslc.exe"
set GLSLC_PATH=C:/VulkanSDK/1.4.328.1/Bin/glslc.exe

%GLSLC_PATH% shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
if %errorlevel% neq 0 ( echo ERROR: Failed to compile simple_shader.vert. ) else ( echo Compiled simple_shader.vert successfully.)

%GLSLC_PATH% shaders/simple_shader.frag -o shaders/simple_shader.frag.spv
if %errorlevel% neq 0 ( echo ERROR: Failed to compile simple_shader.frag. ) else ( echo Compiled simple_shader.frag successfully. )
