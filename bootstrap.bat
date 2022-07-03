@echo off

rem Run this as administrator to go ahead and install all required tools/SDKs to compile the project
rem This is (very) similar to the steps taken in CI pipelines (see .github/workflows/ci.yml),
rem but as this is a batch script it's windows (x64) specific
rem regrettably, setup_cpp_windows will deploy vcpkg in the user directory, not the project itself...
rem Should be mostly fine, but especially on the developer machine multiple projects may be in flight
rem at the same time, possibly creating conflicts with the dependencies

rem setup_cpp_windows options:
rem     compiler and analyzer   llvm, gcc, msvc, vcvarsall, cppcheck, clangtidy, clangformat
rem     build system            cmake, ninja, meson, make, task
rem     package manager         vcpkg, conan, choco, brew
rem     cache                   cppcache
rem     documentation           doxygen, graphviz
rem     coverage                gcovr, opencppcoverage, kcov
rem     other                   python, sevenzip

curl -LJO "https://github.com/aminya/setup-cpp/releases/download/v0.15.0/setup_cpp_windows.exe"
.\setup_cpp_windows ^
    --compiler msvc ^
    --cmake true ^
    --ninja true ^
    --ccache true

rem vcpkg is installed via cmake itself

rem TODO - figure out how to pin versions with chocolatey
choco install vulkan-sdk

rem reload the environment
RefreshEnv.cmd

