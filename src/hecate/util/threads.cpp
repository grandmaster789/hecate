#include "threads.h"
#include "literals.h"
#include "cast.h"
#include "string_util.h"
#include "debugger.h"
#include "codec/utf.h"

#include <cassert>
#include <array>

#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS
	#include <processthreadsapi.h>
#else
#endif

namespace hecate::util {
#if HECATE_PLATFORM == HECATE_PLATFORM_WINDOWS
	void set_current_thread_name(std::string_view name) {

		std::wstring wide = to_wstring(name);
		// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription
		SetThreadDescription(GetCurrentThread(), wide.data());
	}

	std::string get_current_thread_name() {
		PWSTR desc;

		// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreaddescription
		GetThreadDescription(GetCurrentThread(), &desc);

		auto result = to_string(desc);

		LocalFree(desc);

		return result;
	}

	bool is_main_thread() {
		NOT_IMPLEMENTED
	}

	void run_from_main_thread(std::function<void()> fn) {
		NOT_IMPLEMENTED
	}

	ThreadID get_current_threadID() noexcept {
		// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid
		return GetCurrentThreadId();
	}

	size_t get_current_cpuID() noexcept {
		// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocessornumber
		auto result = GetCurrentProcessorNumber();
		assert(result < k_MaxNumCPUs);
		return result;
	}

	std::vector<bool> get_process_affinity_mask() noexcept {
		DWORD_PTR process_mask;
		DWORD_PTR system_mask;

		// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getprocessaffinitymask
		HANDLE handle = GetCurrentProcess();

		if (!GetProcessAffinityMask(handle, &process_mask, &system_mask))
			debugger_abort("Failed to get affinity mask");

		// convert from DWORD_PTR to vector<bool>
		std::vector<bool> result(64);

		for (size_t i = 0; i < 64; ++i)
			result[i] = static_cast<bool>(process_mask & (0x1ull << i));

		return result;
	}

	std::vector<bool> set_thread_affinity_mask(const std::vector<bool>& mask) {
		// convert from vector<bool> to DWORD_PTR
		DWORD_PTR affinity_mask = 0;
		
		for (size_t i = 0; i < mask.size(); ++i)
			if (mask[i])
				affinity_mask |= (0x1ull << i);

		// try and set the affinity mask
		// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask
		HANDLE    handle   = GetCurrentProcess();
		DWORD_PTR old_mask = SetThreadAffinityMask(handle, affinity_mask);

		if (old_mask == 0)
			throw std::runtime_error("Failed to set thread affinity");

		// convert from DWORD_PTR to vector<bool>
		std::vector<bool> result(64);

		for (size_t i = 0; i < 64; ++i)
			result[i] = static_cast<bool>(old_mask & (0x1ull << i));

		return result;
	}

#else
	void set_thread_name(std::string_view name) {
		// https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
		pthread_setname_np(name.data());
	}

	ThreadID get_current_threadID() noexcept {
		return std::bit_cast<uint64_t>(&c_CurrentThreadID_helper); // adress of a threadlocal variable should be unique and nonzero
	}

	size_t get_current_cpuID() noexcept {
		NOT_IMPLEMENTED	
	}

	std::vector<bool> get_process_affinity_mask() noexcept {
		NOT_IMPLEMENTED
	}

	std::vector<bool> set_thread_affinity_mask(const std::vector<bool>& mask) {
		NOT_IMPLEMENTED
	}

#endif

	std::vector<bool> set_thread_affinity(size_t cpuID) {
		std::vector<bool> mask(cpuID + 1);
		mask[cpuID] = true;
		return set_thread_affinity_mask(mask);
	}

	size_t advance_thread_affinity(size_t& cpuID) noexcept {
		auto available = get_process_affinity_mask();

		assert(cpuID < available.size());

		ssize_t selected = -1;

		do {
			if (available[cpuID]) {
				try {
					set_thread_affinity(cpuID);
					selected = narrow_cast<ssize_t>(cpuID);
				}
				catch (...) {}

				do {
					++cpuID;

					if (cpuID == available.size())
						cpuID = 0;
				} 
				while (!available[cpuID]);
			}
		} while (selected < 0);

		return narrow_cast<size_t>(selected);
	}
}