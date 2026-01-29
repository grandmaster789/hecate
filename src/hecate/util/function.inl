#include "function.h"

#include <functional> // std::invoke
#include <utility>    // std::forward

namespace hecate::util {
	template <typename T, typename...Args>
	template <typename Fn>
	Function<T(Args...)>::Function(Fn x) noexcept:
		m_Storage(std::make_unique<Implementation<decltype(x)>>(x))
	{
	}

	template <typename T, typename...Args>
	Function<T(Args...)>::Function(const Function& fn):
		m_Storage(fn.m_Storage->clone())
	{
	}

	template <typename T, typename...Args>
	Function<T(Args...)>& Function<T(Args...)>::operator = (const Function<T(Args...)>& fn) {
		m_Storage.reset(fn.m_Storage->clone());
		return *this;
	}

	template <typename T, typename...Args>
	T Function<T(Args...)>::operator()(Args... args) {
		return m_Storage->call(args...);
	}

	template <typename T, typename...Args>
	template <typename Fn>
	Function<T(Args...)>::Implementation<Fn>::Implementation(Fn x):
		m_StoredFunction(std::move(x))
	{
	}

	// internal compiler error, we meet again -_-
	// 
	//template <typename T, typename...Args>
	//template <typename Fn>
	//Function<T(Args...)>::Implementation<Fn>::Implementation(const Implementation& impl) :
	//	m_StoredFunction(impl.m_StoredFunction)
	//{
	//}

	// internal compiler error, we meet again -_-
	// 
	//template <typename T, typename...Args>
	//template <typename Fn>
	//typename Function<T(Args...)>::Implementation<Fn>& Function<T(Args)>::Implementation<Fn>::operator = (const Implementation& impl) {
	//	return *this;
	//}

	template <typename T, typename...Args>
	template <typename Fn>
	Function<T(Args...)>::Interface* Function<T(Args...)>::Implementation<Fn>::clone() const {
		return new Implementation(*this);
	}

	template <typename T, typename...Args>
	template <typename Fn>
	T Function<T(Args...)>::Implementation<Fn>::call(Args... args) {
		return std::invoke(m_StoredFunction, args...);
	}
}