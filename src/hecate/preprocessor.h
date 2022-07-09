#pragma once

// some more information about architecture and platform macros can be found here: https://abseil.io/docs/cpp/platforms/macros

// lay down some constants
#define HECATE_PLATFORM_WINDOWS 1
#define HECATE_PLATFORM_LINUX 2

// try to autodetect the platform
#ifdef _WIN32
	#define HECATE_PLATFORM HECATE_PLATFORM_WINDOWS
#else // this is very crude
	#define HECATE_PLATFORM HECATE_PLATFORM_LINUX
#endif

// perform platform-specific fixes
#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS

	// WIN32_LEAN_AND_MEAN may already be defined via the CMake script
	// (it disables a lot of headers from being pulled in when including windows.h)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#define NOGDICAPMASKS       // CC_ * , LC_*, PC_*, CP_*, TC_*, RC_
	//#define NOVIRTUALKEYCODES // VK_ *
	//#define NOWINMESSAGES     // WM_ * , EM_*, LB_*, CB_*
	//#define NOWINSTYLES       // WS_ * , CS_*, ES_*, LBS_*, SBS_*, CBS_*
	//#define NOSYSMETRICS      // SM_ *
	#define NOMENUS             // - MF_ *
	//#define NOICONS           // - IDI_ *
	#define NOKEYSTATES         // - MK_ *
	#define NOSYSCOMMANDS       // - SC_ *
	#define NORASTEROPS         // - Binary and Tertiary raster ops
	//#define NOSHOWWINDOW      // - SW_ *
	#define OEMRESOURCE         // - OEM Resource values
	#define NOATOM              // - Atom Manager routines
	#define NOCLIPBOARD         // - Clipboard routines
	#define NOCOLOR             // - Screen colors
	//#define NOCTLMGR          // - Control and Dialog routines
	#define NODRAWTEXT          // - DrawText() and DT_*
	//#define NOGDI             // - All GDI defines and routines
	#define NOKERNEL            // - All KERNEL defines and routines
	//#define NOUSER            // - All USER defines and routines
	//#define NONLS               // - All NLS defines and routines (code pages are in here)
	//#define NOMB              // - MB_ * and MessageBox()
	#define NOMEMMGR            // - GMEM_ * , LMEM_*, GHND, LHND, associated routines
	#define NOMETAFILE          // - typedef METAFILEPICT
	#define NOMINMAX            // - Macros min(a, b) and max(a, b)
	//#define NOMSG             // - typedef MSG and associated routines
	#define NOOPENFILE          // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
	#define NOSCROLL            // - SB_ * and scrolling routines
	#define NOSERVICE           // - All Service Controller routines, SERVICE_ equates, etc.
	#define NOSOUND             // - Sound driver routines
	#define NOTEXTMETRIC        // - typedef TEXTMETRIC and associated routines
	#define NOWH                // - SetWindowsHook and WH_*
	//#define NOWINOFFSETS      // - GWL_ * , GCL_*, associated routines
	#define NOCOMM              // - COMM driver routines
	#define NOKANJI             // - Kanji support stuff.
	#define NOHELP              // - Help engine interface.
	#define NOPROFILER          // - Profiler interface.
	#define NODEFERWINDOWPOS    // - DeferWindowPos routines
	#define NOMCX               // - Modem Configuration Extensions

	#include <SDKDDKVer.h>
	#include <Windows.h>
	#include <intrin.h>
	#include <debugapi.h>
	#include <dwmapi.h>

	#define VK_USE_PLATFORM_WIN32_KHR
#endif

// Build types
#define HECATE_BUILD_DEBUG   1
#define HECATE_BUILD_RELEASE 2

// with MSVC we can figure out whether this is a debug build
#ifdef _DEBUG
	#define HECATE_BUILD HECATE_BUILD_DEBUG
#else
	// assume release build in other cases
	#define HECATE_BUILD HECATE_BUILD_RELEASE
#endif

// NO_MINMAX should mean that these are not defined, but just in case
#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif

#define NOT_IMPLEMENTED throw std::runtime_error("Not implemented");

// compiler related
#define HECATE_COMPILER_MSVC  1
#define HECATE_COMPILER_CLANG 2
#define HECATE_COMPILER_GCC   3

#if defined(_MSC_BUILD)
	#define HECATE_COMPILER HECATE_COMPILER_MSVC

	#define force_inline __forceinline
	#define no_inline __declspec(noinline)
#elif defined(__clang__)
	#define HECATE_COMPILER HECATE_COMPILER_CLANG

	#define unreachable()        __builtin_unreachable()
	#define force_inline  inline __attribute__((always_inline))
	#define no_inline            __attribute__((noinline))
#elif defined(__GNUC__)
	#define HECATE_COMPILER HECATE_COMPILER_GCC

	// https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
	#define unreachable()        __builtin_unreachable()
	#define force_inline  inline __attribute__((always_inline))
	#define no_inline            __attribute__((noinline))
#else
	#error "unsupported compiler"
#endif

// make preprocessor stuff available 
namespace hecate {
	enum class e_Platform {
		windows = HECATE_PLATFORM_WINDOWS,
		linux   = HECATE_PLATFORM_LINUX,

		current = HECATE_PLATFORM
	};

	enum class e_Compiler {
		msvc  = HECATE_COMPILER_MSVC,
		clang = HECATE_COMPILER_CLANG,
		gcc   = HECATE_COMPILER_GCC,

		current = HECATE_COMPILER
	};

	enum class e_Build {
		debug   = HECATE_BUILD_DEBUG,
		release = HECATE_BUILD_RELEASE,

		current = HECATE_BUILD
	};

	// make preprocessor constants available as constant expressions
	static constexpr e_Platform  k_Platform  = e_Platform ::current;
	static constexpr e_Compiler  k_Compiler  = e_Compiler ::current;
	static constexpr e_Build     k_Build     = e_Build    ::current;
}