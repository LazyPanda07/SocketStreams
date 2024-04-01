#pragma once

#include <concepts>
#include <functional>

namespace web
{
	namespace utility
	{
		/**
		* @brief Requirements for in/out socket operations
		*/
		template<typename T>
		concept Container = requires(T value)
		{
			{ value.data() } -> std::same_as<char*>;
			{ static_cast<const T&>(value).data() } -> std::same_as<const char*>;
			{ static_cast<const T&>(value).size() } -> std::same_as<size_t>;
			{ value.resize(size_t()) };
			{ value[size_t()] } -> std::same_as<char&>;
		};

		/**
		* @brief Wrap Container concept instance
		*/
		class ContainerWrapper
		{
		private:
			std::function<char* ()> dataImplementation;
			std::function<const char* ()> constDataImplementation;
			std::function<size_t()> sizeImplementation;
			std::function<void(size_t)> resizeImplementation;
			std::function<char& (size_t)> operatorImplementation;

		public:
			template<Container T>
			ContainerWrapper(T& value);

			ContainerWrapper(const ContainerWrapper&) = delete;

			ContainerWrapper(ContainerWrapper&&) noexcept = default;

			ContainerWrapper& operator = (const ContainerWrapper&) = delete;

			ContainerWrapper& operator = (ContainerWrapper&&) noexcept = default;

			char* data();

			const char* data() const;

			size_t size() const;

			void resize(size_t newSize);

			char& operator [](size_t index);

			~ContainerWrapper() = default;
		};
	}
}

namespace web
{
	namespace utility
	{
		template<Container T>
		ContainerWrapper::ContainerWrapper(T& value)
		{
			dataImplementation = [&value]() mutable -> char*
				{
					return value.data();
				};

			constDataImplementation = [&value]() mutable -> const char*
				{
					return const_cast<const T&>(value).data();
				};

			sizeImplementation = [&value]() mutable -> size_t
				{
					return value.size();
				};

			resizeImplementation = [&value](size_t newSize) mutable -> void
				{
					value.resize(newSize);
				};

			operatorImplementation = [&value](size_t index) mutable -> char&
				{
					return value[index];
				};
		}
	}
}
