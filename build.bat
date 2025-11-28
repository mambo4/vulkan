set ROOT=%CD%
set VULKAN_SDK_INCLUDE_PATH="C:\VulkanSDK\1.4.328.1\Include"
set GLFW_INCLUDE_PATH="C:\vulkan\src\GLFW"
set VULKAN_SDK_LIB_PATH="C:\VulkanSDK\1.4.328.1\Lib"
set GLFW_LIB_PATH="C:\vulkan\src\GLFW"

mkDir bin
mkDir build

:: CLEANUP
::for %%f in (dir *.obj) DO del %%f
del /Q %CD%\src\*.obj
del /Q %CD%\*.obj
del /Q %CD%\build\*.*
del /Q %CD%\bin\*.* 

cl.exe  /Zi /EHsc /nologo /I%root%\src /I%VULKAN_SDK_INCLUDE_PATH% /I%GLFW_INCLUDE_PATH% /Fe%root%\bin\main.exe /Fd%root%\build\ /Fo%root%\build\ src\\*.cpp /link /LIBPATH:%GLFW_LIB_PATH% /LIBPATH:%VULKAN_SDK_LIB_PATH% glfw3_mt.lib vulkan-1.lib user32.lib gdi32.lib  shell32.lib                                     