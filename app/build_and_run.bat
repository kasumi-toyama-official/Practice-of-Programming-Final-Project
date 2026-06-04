@echo off
chcp 65001 > nul
cd /d "%~dp0"
set PATH=D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.11.1\mingw_64\bin;%PATH%
if not exist build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug\Makefile (
    mkdir build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug\debug 2>nul
    qmake.exe -spec win32-g++ CONFIG+=debug QMAKE_CXX=D:\Qt\Tools\mingw1310_64\bin\g++.exe QMAKE_LINK=D:\Qt\Tools\mingw1310_64\bin\g++.exe -o build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug\Makefile big_project.pro
)
cd build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug
mingw32-make -j4
cd ..\..\
if not exist build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug\debug\big_project.exe (
    echo Build failed - exe not found
    pause
    exit /b 1
)
windeployqt.exe build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug\debug\big_project.exe >nul
start "" build\Desktop_Qt_6_11_0_MinGW_64_bit-Debug\debug\big_project.exe
