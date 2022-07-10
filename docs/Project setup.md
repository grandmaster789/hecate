Project setup is sort of platform-specific, so here I'll document the environment

### Windows
- https://www.python.org/
- https://www.7-zip.org/download.html
- https://chocolatey.org/
	- choco install ninja
- https://www.lunarg.com/vulkan-sdk/
- https://cmake.org/
	- cmake . -Bbuild -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 17 2022" -A x64
	- cmake --build build --config Debug

This should mirror the steps in the Github Workflows yml, at .github/workflows/windows.yml
