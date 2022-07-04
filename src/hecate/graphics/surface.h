#pragma once

namespace hecate::graphics {
	class Surface {
	public:
		virtual ~Surface() = default;

		Surface             (const Surface&)     = delete;
		Surface& operator=  (const Surface&)     = delete;
		Surface             (Surface&&) noexcept = delete;
		Surface& operator = (Surface&&) noexcept = delete;

		virtual       void    init() {}
		virtual       void    set_device(Device* d) noexcept;
		[[nodiscard]] Device* get_device() const noexcept;

	protected:

	};
}