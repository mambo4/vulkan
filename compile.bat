@echo off
@rem compile shaders using glslc
@rem glslc path is "C:/VulkanSDK/1.4.328.1/Bin/glslc.exe"
set GLSLC_PATH=C:/VulkanSDK/1.4.328.1/Bin/glslc.exe

for %%f in (shaders\*.vert shaders\*.frag shaders\*.comp) do (
    %GLSLC_PATH% %%f -o %%f.spv
    if %errorlevel% neq 0 (
        echo ERROR: Failed to compile %%f.
    ) else (
        echo Compiled %%f successfully.
    )
)