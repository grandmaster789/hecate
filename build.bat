@echo off

rem default behavior (no arguments) is to build in debug mode
if [%1]==[] goto debug

if /I %1==clean goto clean
if /I %1==release goto release
if /I %1==debug goto debug

echo Usage: build (debug / release / clean)
goto exit

:debug
set BUILD_TYPE_ARG="Debug"
goto build

:release
set BUILD_TYPE_ARG="Release"
goto build

:build
set PROJECT_ROOT_WIN=%CD%
set PROJECT_ROOT=%PROJECT_ROOT_WIN:\=/%
set VCPKG_TARGET_TRIPLET=x64-windows

echo Project root = %PROJECT_ROOT%
echo Build type   = %BUILD_TYPE_ARG%

if not exist build mkdir build
cd build
if not exist "x64-%BUILD_TYPE_ARG%" mkdir "x64-%BUILD_TYPE_ARG%"
cd "x64-%BUILD_TYPE_ARG%"

echo Working directory: %CD%
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE_ARG% "%PROJECT_ROOT%"
cmake --build .

cd ~dp0
goto exit

:clean
if exists build rd build /s /q
goto exit

:exit
echo Done