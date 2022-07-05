#pragma once

#include <atomic>

namespace hecate::util {
	// unfair mutex; should only be used in low-contention situations
	// (no arbitration is done between multiple blocking threads)
	// 
	// https://en.cppreference.com/w/cpp/named_req/BasicLockable
	// https://en.cppreference.com/w/cpp/named_req/Lockable
	class SpinMutex {
	public:
		SpinMutex() noexcept = default;

		SpinMutex             (const SpinMutex&) = delete;
		SpinMutex& operator = (const SpinMutex&) = delete;

		void lock()     noexcept;
		bool try_lock() noexcept;
		void unlock()   noexcept;

	private:
		void lock_contention(uint32_t state) noexcept; // le spin

		// because we want to use atomic 'fetch_sub' we need some kind of basic integer (can't use enum)
		static inline constexpr uint32_t unlocked       = 0; // unlocked without other threads waiting
		static inline constexpr uint32_t locked_nowait  = 1; // locked without other threads waiting
		static inline constexpr uint32_t locked_waiting = 2; // locked *and* another thread is actually waiting to acquire

		std::atomic<uint32_t> m_Semaphore = unlocked;
	};
}