#pragma once

#include <memory>

namespace hecate::util {
	// Based on CppWeekly ep 333
	// NOTE - this resembles Sean Parents' Concept/Implementation style
	// NOTE - noexcept specifiers are missing (call operator could probably be conditionally noexcept)
	// NOTE - this is owning, so it can have full value semantics; non-owning may be more efficient
	// NOTE - constexpr unique_ptr is c++23, which is not fully supported yet on my platform
	//        this ended up being the reason that this can't be constexpr at the moment
	// NOTE - small object optimization is missing and this still does heap allocation, 
	//        so this is probably less performant than std::function

	template <typename>
	class Function;

	template <typename t_Result, typename... t_Args>
	class Function<t_Result(t_Args...)> {
	public:
		template <typename t_Callable> // is there a concept for this?
		Function(t_Callable x) noexcept;

		Function             (const Function& fn);
		Function& operator = (const Function& fn);
		Function             (Function&&) noexcept = default;
		Function& operator = (Function&&) noexcept = default;

		t_Result operator()(t_Args&&...);

	private:
		struct Interface {
			Interface() noexcept = default;

			virtual ~Interface() = default;

			Interface             (const Interface&)     = default;
			Interface& operator = (const Interface&)     = default;
			Interface             (Interface&&) noexcept = default;
			Interface& operator = (Interface&&) noexcept = default;

			virtual Interface* clone()           const = 0;
			virtual t_Result   call(t_Args&&...) const = 0;
		};

		template <typename Callable>
		struct Implementation :
			Interface
		{
			Implementation()                              noexcept = default;
			Implementation             (Implementation&&) noexcept = default;
			Implementation& operator = (Implementation&&) noexcept = default;

			Implementation(Callable c);

			Implementation(const Implementation& impl) :
				m_StoredFunction(impl.m_StoredFunction)
			{
			}

			Implementation& operator = (const Implementation& impl) {
				m_StoredFunction = impl.m_StoredFunction;
				return *this;
			}

			Interface* clone()           const override;
			t_Result   call(t_Args&&...) const override;

			Callable m_StoredFunction;
		};

		std::unique_ptr<Interface> m_Storage;
	};
}

#include "function.inl"