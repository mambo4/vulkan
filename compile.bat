@rem compile shaders using glslc
@rem glslc path is "C:/VulkanSDK/1.4.328.1/Bin/glslc.exe"
set GLSLC_PATH=C:/VulkanSDK/1.4.328.1/Bin/glslc.exe

%GLSLC_PATH% shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
%GLSLC_PATH% shaders/simple_shader.frag -o shaders/simple_shader.frag.spv
echo Shaders compiled successfully.