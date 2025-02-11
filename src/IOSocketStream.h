#pragma once

#include <istream>
#include <limits>

#include "IOSocketBuffer.h"
#include "SocketStreamsUtility.h"

namespace streams
{
	/// @brief Base input/output socket stream
	class IOSocketStream : public std::iostream
	{
	protected:
		std::unique_ptr<buffers::IOSocketBuffer> buffer;

	protected:
		template<web::utility::Fundamental T>
		int sendFundamental(T value);

		template<web::utility::Fundamental T>
		int receiveFundamental(T& value);

		virtual int sendFundamentalImplementation(const char* value, int valueSize, bool& endOfStream);

		virtual int receiveFundamentalImplementation(char* value, int valueSize, bool& endOfStream);

	private:
		IOSocketStream(std::unique_ptr<buffers::IOSocketBuffer>&& buffer);

		IOSocketStream(std::unique_ptr<web::Network>&& network);

	public:
		template<std::derived_from<web::Network> T, typename... Args>
		static IOSocketStream createStream(Args&&... args);

		template<std::derived_from<buffers::IOSocketBuffer> T, typename... Args>
		static IOSocketStream createStream(Args&&... args);

	public:
		/// @brief Deleted copy constructor
		IOSocketStream(const IOSocketStream&) = delete;

		/// @brief Move constructor
		/// @param other 
		IOSocketStream(IOSocketStream&& other) noexcept;

		/// @brief Deleted copy assignment operator
		/// @param  
		/// @return 
		IOSocketStream& operator = (const IOSocketStream&) = delete;

		/// @brief Move assignment operator
		/// @param other 
		/// @return 
		IOSocketStream& operator = (IOSocketStream&& other) noexcept;

		template<std::derived_from<web::Network> T = web::Network>
		T& getNetwork();

		template<std::derived_from<web::Network> T = web::Network>
		const T& getNetwork() const;

		std::ostream& operator << (bool value);
		std::ostream& operator << (short value);
		std::ostream& operator << (int value);
		std::ostream& operator << (long value);
		std::ostream& operator << (long long value);

		std::ostream& operator << (unsigned short value);
		std::ostream& operator << (unsigned int value);
		std::ostream& operator << (unsigned long value);
		std::ostream& operator << (unsigned long long value);

		std::ostream& operator << (float value);
		std::ostream& operator << (double value);
		std::ostream& operator << (long double value);

		std::istream& operator >> (bool& value);
		std::istream& operator >> (short& value);
		std::istream& operator >> (int& value);
		std::istream& operator >> (long& value);
		std::istream& operator >> (long long& value);

		std::istream& operator >> (unsigned short& value);
		std::istream& operator >> (unsigned int& value);
		std::istream& operator >> (unsigned long& value);
		std::istream& operator >> (unsigned long long& value);

		std::istream& operator >> (float& value);
		std::istream& operator >> (double& value);
		std::istream& operator >> (long double& value);

		/**
		* @brief Send data through network
		* @param data Container concept instance
		*/
		template<web::utility::Container T>
		std::ostream& operator << (const T& data);

		/**
		* @brief Receive data through network
		* @param data Container concept instance
		*/
		template<web::utility::Container T>
		std::istream& operator >> (T& data);

		virtual ~IOSocketStream() = default;
	};

	template<web::utility::Fundamental T>
	int IOSocketStream::sendFundamental(T value)
	{
		try
		{
			bool endOfStream = false;
			int lastPacketSize = this->sendFundamentalImplementation(reinterpret_cast<const char*>(&value), sizeof(value), endOfStream);

			if (endOfStream)
			{
				setstate(std::ios_base::eofbit);
			}

			return lastPacketSize;
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}
	}

	template<web::utility::Fundamental T>
	int IOSocketStream::receiveFundamental(T& value)
	{
		try
		{
			bool endOfStream = false;
			int lastPacketSize = this->receiveFundamentalImplementation(reinterpret_cast<char*>(&value), sizeof(value), endOfStream);

			if (endOfStream)
			{
				setstate(std::ios_base::eofbit);
			}

			return lastPacketSize;
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}
	}

	template<std::derived_from<web::Network> T, typename... Args>
	IOSocketStream IOSocketStream::createStream(Args&&... args)
	{
		return IOSocketStream(std::make_unique<T>(std::forward<Args>(args)...));
	}

	template<std::derived_from<buffers::IOSocketBuffer> T, typename... Args>
	IOSocketStream IOSocketStream::createStream(Args&&... args)
	{
		return IOSocketStream(std::make_unique<T>(std::forward<Args>(args)...));
	}

	template<std::derived_from<web::Network> T>
	T& IOSocketStream::getNetwork()
	{
		if (buffer->getNetwork())
		{
			return dynamic_cast<T&>(*buffer->getNetwork());
		}

		throw std::runtime_error("Network is nullptr");

		return *buffer->getNetwork();
	}

	template<std::derived_from<web::Network> T>
	const T& IOSocketStream::getNetwork() const
	{
		if (buffer->getNetwork())
		{
			return dynamic_cast<T&>(*buffer->getNetwork());
		}

		throw std::runtime_error("Network is nullptr");

		return *buffer->getNetwork();
	}

	template<web::utility::Container T>
	std::ostream& IOSocketStream::operator << (const T& data)
	{
		web::utility::ContainerWrapper container(const_cast<T&>(data));
		constexpr std::streamsize size = (std::numeric_limits<std::streamsize>::max)();

		try
		{
			if (buffer->sputn(reinterpret_cast<const char*>(&container), size) == buffers::IOSocketBuffer::traits_type::eof())
			{
				setstate(std::ios_base::eofbit);
			}
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}

		return *this;
	}

	template<web::utility::Container T>
	std::istream& IOSocketStream::operator >> (T& data)
	{
		web::utility::ContainerWrapper container(data);
		constexpr std::streamsize size = (std::numeric_limits<std::streamsize>::max)();

		try
		{
			if (buffer->sgetn(reinterpret_cast<char*>(&container), size) == buffers::IOSocketBuffer::traits_type::eof())
			{
				setstate(std::ios_base::eofbit);
			}
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}

		return *this;
	}
}
