#pragma once

#include <concepts>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace hecate::util {
	// Based on CppWeekly ep 333
	// NOTE - this resembles Sean Parents' Concept/Implementation style
	// NOTE - this is owning, so it can have full value semantics; non-owning may be more efficient
	// IMPROVED - Added perfect forwarding for arguments
	// IMPROVED - Added Small Object Optimization (32 bytes inline storage)
	// IMPROVED - Added null state checking and empty() / operator bool()
	// IMPROVED - Added C++20 concepts for type safety
	// IMPROVED - Fixed exception safety in constructor

	// Exception thrown when calling an empty Function
	class bad_function_call : public std::exception {
	public:
		const char* what() const noexcept override {
			return "bad function call: Function is empty";
		}
	};

	template <typename>
	class Function;

	template <typename t_Result, typename... t_Args>
	class Function<t_Result(t_Args...)> {
	public:
		// Concept to constrain callable types
		template <typename t_Callable>
		static constexpr bool is_valid_callable =
			std::invocable<t_Callable, t_Args...> &&
			(std::is_void_v<t_Result> || std::convertible_to<std::invoke_result_t<t_Callable, t_Args...>, t_Result>);

		// Default constructor creates empty function
		Function() noexcept = default;

		// Construct from callable
		template <typename t_Callable>
			requires is_valid_callable<t_Callable> &&
			         (!std::is_same_v<std::decay_t<t_Callable>, Function>)
		Function(t_Callable x) {
			using ImplType = Implementation<std::decay_t<t_Callable>>;

			// Use Small Object Optimization if callable fits
			if constexpr (sizeof(ImplType) <= InlineSize && alignof(ImplType) <= InlineAlign) {
				new (m_InlineStorage) ImplType(std::move(x));
				m_UsesInlineStorage = true;
			} else {
				m_Storage = std::make_unique<ImplType>(std::move(x));
				m_UsesInlineStorage = false;
			}
		}

		Function             (const Function& fn);
		Function& operator = (const Function& fn);
		Function             (Function&&) noexcept;
		Function& operator = (Function&&) noexcept;

		~Function();

		// Call operator with perfect forwarding
		template <typename... t_CallArgs>
		t_Result operator()(t_CallArgs&&... args) {
			if (empty()) {
				throw bad_function_call();
			}
			// Perfect forwarding happens here, then gets converted to t_Args... at the interface boundary
			return get_interface()->call(std::forward<t_CallArgs>(args)...);
		}

		// Check if function is non-empty
		explicit operator bool() const noexcept { return !empty(); }
		bool empty() const noexcept { return !m_UsesInlineStorage && !m_Storage; }

	private:
		static constexpr size_t InlineSize = 32;
		static constexpr size_t InlineAlign = alignof(std::max_align_t);

		struct Interface {
			Interface() noexcept = default;

			virtual ~Interface() = default;

			Interface             (const Interface&)     = default;
			Interface& operator = (const Interface&)     = default;
			Interface             (Interface&&) noexcept = default;
			Interface& operator = (Interface&&) noexcept = default;

			virtual Interface* clone() const = 0;
			virtual void clone_into(void* dest) const = 0;
			virtual t_Result call(t_Args... args) = 0;
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

			Interface* clone() const override;
			void clone_into(void* dest) const override;
			t_Result call(t_Args... args) override;

			Callable m_StoredFunction;
		};

		void destroy() noexcept;
		Interface* get_interface() noexcept;

		alignas(InlineAlign) std::byte m_InlineStorage[InlineSize];
		std::unique_ptr<Interface> m_Storage;
		bool m_UsesInlineStorage = false;
	};
}

#include "function.inl"