:: **************************** VCVARS.BAT FILE ****************************************
:: set %msdev_cmd% to your chosen vcvars bat file
set msdev_cmd="%programfiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

:: ************************** PROJECT FOLDER **********************************************
:: edit  %root% to your actual project root (i.e, parent root of .vscode)
set root=C:/vulkan

:: *************************** leave this part alone ********************************************
set path = %path%;%root%
Call %msdev_cmd%
echo root: %root%
cd /d %root%
code .
pause