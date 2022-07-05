#include "spin_mutex.h"
#include <thread>
#include <cassert>

namespace hecate::util {
	void SpinMutex::lock() noexcept {
		uint32_t state = unlocked;

		// if we were unlocked, we can set it to the basic 'locked' state
		// https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
		if (!m_Semaphore.compare_exchange_strong(
			state, 
			locked_nowait, 
			std::memory_order_acquire
		)) {
			[[unlikely]]
			lock_contention(state); // otherwise we need to spin
		}
	}

	bool SpinMutex::try_lock() noexcept {
		uint32_t state = unlocked;

		// https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
		if (!m_Semaphore.compare_exchange_strong(
			state, 
			locked_nowait, 
			std::memory_order_acquire
		)) {
			[[unlikely]]
			return false; // we'd need to block...
		}

		return true;
	}

	void SpinMutex::unlock() noexcept {
		// going in here, the state should either be 
		//    'locked_nowait'  (1) 
		// or 'locked_waiting' (2)
		//
		// so we'll assume (1) and fetch_sub to go from (1) to (0) unlocked
		// and test the result of fetch_sub to do a 'notify_one' for the actual
		// waiting case
		// 
		// https://en.cppreference.com/w/cpp/atomic/atomic/fetch_sub
		if (m_Semaphore.fetch_sub(1, std::memory_order_relaxed) != locked_nowait) {
			// handle the case where we started at 'locked_waiting' (2)
			[[unlikely]]
			m_Semaphore.store(unlocked, std::memory_order_release);
			m_Semaphore.notify_one();
		}
		else
			// https://en.cppreference.com/w/cpp/atomic/atomic_thread_fence
			atomic_thread_fence(std::memory_order_release); 
	}

	void SpinMutex::lock_contention(uint32_t state) noexcept {
		do {
			bool should_wait = (state == locked_waiting);

			if (should_wait || m_Semaphore.compare_exchange_strong(state, locked_waiting))
				m_Semaphore.wait(locked_waiting);

			state = unlocked;
		} while (!m_Semaphore.compare_exchange_strong(state, locked_waiting));
	}
}