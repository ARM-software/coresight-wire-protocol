@echo on

set REGIME=%1

if "%REGIME%" == "" (
  echo "usage: build.bat <Release|Debug>"
  exit /b 1
)

set SRC_DIR=..\..\..
set BUILD_DIR=build\win_32-x86_64\%REGIME%
set GEN="Visual Studio 17 2022"

mkdir build
mkdir build\win_32-x86_64
mkdir build\win_32-x86_64\%REGIME%
cd %BUILD_DIR%

cmake -G %GEN% -DCMAKE_BUILD_TYPE=%REGIME% -DPLATFORM_ARCH=x86_64 -DCMAKE_INSTALL_PREFIX=dist %SRC_DIR%
cd %SRC_DIR%