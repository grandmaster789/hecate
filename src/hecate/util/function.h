#pragma once

#include <memory>

namespace hecate::util {
	// Based on CppWeekly ep 333
	// NOTE - this resembles Sean Parents' Concept/Implementation style
	// NOTE - noexcept specifiers are missing
	// NOTE - this is owning, so it can have full value semantics; non-owning may be more efficient

	template <typename>
	class Function;

	template <typename t_Result, typename... t_Args>
	class Function<t_Result(t_Args...)> {
	public:
		template <typename t_Callable> // is there a concept for this?
		constexpr Function(t_Callable x);

		constexpr Function             (const Function& fn);
		constexpr Function& operator = (const Function& fn);
		constexpr Function             (Function&&) noexcept = default;
		constexpr Function& operator = (Function&&) noexcept = default;

		constexpr t_Result operator()(t_Args&&...);

	private:
		struct Interface {
			constexpr Interface() noexcept = default;

			virtual ~Interface() = default;

			constexpr Interface             (const Interface&)     = default;
			constexpr Interface& operator = (const Interface&)     = default;
			constexpr Interface             (Interface&&) noexcept = default;
			constexpr Interface& operator = (Interface&&) noexcept = default;

			virtual constexpr Interface* clone()           const = 0;
			virtual constexpr t_Result   call(t_Args&&...) const = 0;
		};

		template <typename Callable>
		struct Implementation :
			Interface
		{
			constexpr Implementation()                              noexcept = default;
			constexpr Implementation             (Implementation&&) noexcept = default;
			constexpr Implementation& operator = (Implementation&&) noexcept = default;

			constexpr Implementation(Callable c);

			constexpr Implementation(const Implementation& impl) :
				m_StoredFunction(impl.m_StoredFunction)
			{
			}

			constexpr Implementation& operator = (const Implementation& impl) {
				m_StoredFunction = impl.m_StoredFunction;
				return *this;
			}

			constexpr Interface* clone()           const override;
			constexpr t_Result   call(t_Args&&...) const override;

			Callable m_StoredFunction;
		};

		std::unique_ptr<Interface> m_Storage;
	};
}

#include "function.inl"