#pragma once

#include <utility>
#include <optional> // may make my own at some point

namespace hecate::util {
	template <
		typename tHandle,
		typename tDeleter
	>
	class UniqueResource {
	public:
		constexpr UniqueResource() = default; // initializes the handle to std::none_t
		constexpr UniqueResource(
			tHandle&&  handle,
			tDeleter&& deleter = {}
		);
		~UniqueResource();

		constexpr UniqueResource             (const UniqueResource&) = delete;
		constexpr UniqueResource& operator = (const UniqueResource&) = delete;

		constexpr UniqueResource(UniqueResource&& ur) noexcept(
			std::is_nothrow_constructible_v<tHandle>  &&
			std::is_nothrow_constructible_v<tDeleter>
		);

		constexpr UniqueResource& operator = (UniqueResource&& ur) noexcept(
			std::is_nothrow_move_assignable_v<tHandle>  &&
			std::is_nothrow_move_assignable_v<tDeleter>
		);

		explicit constexpr operator bool() const noexcept;

		constexpr       tHandle  get()          const noexcept;
		constexpr       tHandle* operator -> () const noexcept;
		constexpr const tHandle& operator * ()  const noexcept;
		
		constexpr void release();
		constexpr void reset();

		void swap(UniqueResource& ur);

	private:
		std::optional<tHandle> m_Handle;
		tDeleter               m_Deleter;
	};

	template <typename T, typename D>
	void swap(
		UniqueResource<T, D>& a,
		UniqueResource<T, D>& b
	);
}

#include "unique_resource.inl"