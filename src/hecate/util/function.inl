#include "function.h"

#include <functional> // std::invoke
#include <utility>    // std::forward

namespace hecate::util {
	// Copy constructor
	template <typename t_Result, typename...t_Args>
	Function<t_Result(t_Args...)>::Function(const Function& fn) :
		m_UsesInlineStorage(fn.m_UsesInlineStorage)
	{
		if (fn.m_UsesInlineStorage) {
			reinterpret_cast<const Interface*>(fn.m_InlineStorage)->clone_into(m_InlineStorage);
		} else if (fn.m_Storage) {
			m_Storage.reset(fn.m_Storage->clone());
		}
	}

	// Copy assignment
	template <typename t_Result, typename...t_Args>
	Function<t_Result(t_Args...)>& Function<t_Result(t_Args...)>::operator = (const Function<t_Result(t_Args...)>& fn) {
		if (this != &fn) {
			destroy();

			m_UsesInlineStorage = fn.m_UsesInlineStorage;

			if (fn.m_UsesInlineStorage) {
				reinterpret_cast<const Interface*>(fn.m_InlineStorage)->clone_into(m_InlineStorage);
			} else if (fn.m_Storage) {
				m_Storage.reset(fn.m_Storage->clone());
			}
		}
		return *this;
	}

	// Move constructor
	template <typename t_Result, typename...t_Args>
	Function<t_Result(t_Args...)>::Function(Function&& fn) noexcept :
		m_Storage(std::move(fn.m_Storage)),
		m_UsesInlineStorage(fn.m_UsesInlineStorage)
	{
		if (m_UsesInlineStorage) {
			std::memcpy(m_InlineStorage, fn.m_InlineStorage, InlineSize);
			fn.m_UsesInlineStorage = false;
		}
	}

	// Move assignment
	template <typename t_Result, typename...t_Args>
	Function<t_Result(t_Args...)>& Function<t_Result(t_Args...)>::operator = (Function&& fn) noexcept {
		if (this != &fn) {
			destroy();

			m_Storage = std::move(fn.m_Storage);
			m_UsesInlineStorage = fn.m_UsesInlineStorage;

			if (m_UsesInlineStorage) {
				std::memcpy(m_InlineStorage, fn.m_InlineStorage, InlineSize);
				fn.m_UsesInlineStorage = false;
			}
		}
		return *this;
	}

	// Destructor
	template <typename t_Result, typename...t_Args>
	Function<t_Result(t_Args...)>::~Function() {
		destroy();
	}

	// Helper to destroy stored callable
	template <typename t_Result, typename...t_Args>
	void Function<t_Result(t_Args...)>::destroy() noexcept {
		if (m_UsesInlineStorage) {
			get_interface()->~Interface();
			m_UsesInlineStorage = false;
		} else {
			m_Storage.reset();
		}
	}

	// Helper to get interface pointer
	template <typename t_Result, typename...t_Args>
	typename Function<t_Result(t_Args...)>::Interface* Function<t_Result(t_Args...)>::get_interface() noexcept {
		if (m_UsesInlineStorage) {
			return reinterpret_cast<Interface*>(m_InlineStorage);
		} else {
			return m_Storage.get();
		}
	}

	// Implementation constructor
	template <typename t_Result, typename...t_Args>
	template <typename Fn>
	Function<t_Result(t_Args...)>::Implementation<Fn>::Implementation(Fn x):
		m_StoredFunction(std::move(x))
	{
	}

	// Implementation clone (heap allocation)
	template <typename t_Result, typename...t_Args>
	template <typename Fn>
	typename Function<t_Result(t_Args...)>::Interface* Function<t_Result(t_Args...)>::Implementation<Fn>::clone() const {
		return new Implementation(*this);
	}

	// Implementation clone_into (placement new for SBO)
	template <typename t_Result, typename...t_Args>
	template <typename Fn>
	void Function<t_Result(t_Args...)>::Implementation<Fn>::clone_into(void* dest) const {
		new (dest) Implementation(*this);
	}

	// Implementation call
	template <typename t_Result, typename...t_Args>
	template <typename Fn>
	t_Result Function<t_Result(t_Args...)>::Implementation<Fn>::call(t_Args... args) {
		return std::invoke(m_StoredFunction, std::forward<t_Args>(args)...);
	}
}