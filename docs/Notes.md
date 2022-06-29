# Hecate
High level goals
- Multiplatform 'large' project (windows + linux, look into android, ios, mac etc)
- CMake + vcpkg
	- Initial project setup via [[https://github.com/friendlyanon/cmake-init]] [[Project setup]]
	- 
- Automated tests
- Vulkan compute with CPU reference variants
- Geometric algebra
	- CPU side can be implemented using templates and/or simd
	- GPU side should probably be specialized for common cases
- Minimal dependencies
	- Try and build everything from specifications or first principles
	- This is a guideline, not a hard rule - using small libraries is acceptable
	- At some point, internal subprojects should be factored out and become their own library
- Plugin architecture
	- I/O via plugins
		- plugins are allowed more leeway in what libraries are used
- Fully custom user interface
	- Based mostly on the Elements library, but fully hardware accelerated
- Computer vision pipelines
	- Triangulation
	- Multi-view stereo [[https://github.com/openMVG/openMVG]]
	- Voxel carving [[https://github.com/unclearness/vacancy]]
- Manual design docs via obsidian (yer looking at em)
	- Math using integrated LaTeX [[https://github.com/joeyuping/quick_latex_obsidian]]
- Live shader editing
	- ShaderToy-like

Non-goals (maybe in the far future, or use external libraries for these)
- Database systems
- Networking
- 

So beginnings are kind of easy but also kind of difficult. It's easy to extend an existing framework, but it's hard to pre-suppose the long-term consequences of early decisions. It's also simple to just start working -- probably I'll end up going my favored route: just build something, then afterwards start over and build it again, but better. This approach should be limited to maybe 3x before accepting the current state and moving on though.

