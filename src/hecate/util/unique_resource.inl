#pragma once

#include "unique_resource.h"

namespace hecate::util {
	template <typename T, typename D>
	constexpr UniqueResource<T, D>::UniqueResource(
		T&& handle,
		D&& deleter
	):
		m_Handle (std::forward<T>(handle)),
		m_Deleter(std::forward<D>(deleter))
	{
	}

	template <typename T, typename D>
	UniqueResource<T, D>::~UniqueResource() {
		reset();
	}

	template <typename T, typename D>
	constexpr UniqueResource<T, D>::UniqueResource(UniqueResource&& ur) noexcept(
		std::is_nothrow_constructible_v<T>&&
		std::is_nothrow_constructible_v<D>
	):
		m_Handle (std::move(ur.m_Handle)),
		m_Deleter(std::move(ur.m_Deleter))
	{
	}

	template <typename T, typename D>
	constexpr UniqueResource<T, D>& UniqueResource<T, D>::operator = (UniqueResource&& ur) noexcept(
		std::is_nothrow_move_assignable_v<T>&&
		std::is_nothrow_move_assignable_v<D>
	) {
		reset();

		m_Handle  = std::move(ur.m_Handle);
		m_Deleter = std::move(ur.m_Deleter);

		return *this;
	}

	template <typename T, typename D>
	constexpr UniqueResource<T, D>::operator bool() const noexcept {
		return (m_Handle != std::nullopt_t);
	}

	template <typename T, typename D>
	constexpr T UniqueResource<T, D>::get() const noexcept {
		return *m_Handle;
	}

	template <typename T, typename D>
	constexpr T* UniqueResource<T, D>::operator -> () const noexcept {
		return &(*m_Handle);
	}

	template <typename T, typename D>
	constexpr const T& UniqueResource<T, D>::operator * () const noexcept {
		return *m_Handle;
	}

	template <typename T, typename D>
	constexpr void UniqueResource<T, D>::release() {
		m_Handle.release();
	}

	template <typename T, typename D>
	constexpr void UniqueResource<T, D>::reset() {
		if (m_Handle)
			m_Deleter(*m_Handle);
	}
}
