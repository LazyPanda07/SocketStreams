#pragma once

#include <istream>

#include "IOSocketBuffer.h"

namespace streams
{
	/// @brief Base input/output socket stream
	class IOSocketStream : public std::iostream
	{
	protected:
		std::unique_ptr<buffers::IOSocketBuffer> buffer;

	protected:
		/// @brief This method should throw web::exceptions::WebException
		virtual void logAndThrowWebException();

	protected:
		template<typename T>
		int sendFundamental(T value);

		template<typename T>
		int receiveFundamental(T& value);

	public:
		IOSocketStream();

		/// @brief Deleted copy constructor
		IOSocketStream(const IOSocketStream&) = delete;

		/// @brief Move constructor
		/// @param other 
		IOSocketStream(IOSocketStream&& other) noexcept;

		/// @brief Server side contructor
		/// @param clientSocket 
		IOSocketStream(SOCKET clientSocket);

		/// @brief Server side constructor
		/// @param clientSocket 
		/// @param bufferSize Fixed buffer size
		IOSocketStream(SOCKET clientSocket, size_t bufferSize);

		/// @brief Client side contructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		IOSocketStream(std::string_view ip, std::string_view port, DWORD timeout = 30'000);

		/// @brief Client side contructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param bufferSize Fixed buffer size
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		IOSocketStream(std::string_view ip, std::string_view port, size_t bufferSize, DWORD timeout = 30'000);

		IOSocketStream(std::unique_ptr<buffers::IOSocketBuffer>&& buffer);

		/// @brief Constructor with custom network and default buffer
		/// @param network 
		IOSocketStream(std::unique_ptr<web::Network>&& network);

		/// @brief Constructor with custom network and default buffer with buffer size
		/// @param network 
		/// @param bufferSize Fixed buffer size
		IOSocketStream(std::unique_ptr<web::Network>&& network, size_t bufferSize);

		/// @brief Deleted copy assignment operator
		/// @param  
		/// @return 
		IOSocketStream& operator = (const IOSocketStream&) = delete;

		/// @brief Move assignment operator
		/// @param other 
		/// @return 
		IOSocketStream& operator = (IOSocketStream&& other) noexcept;

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

		virtual std::ostream& operator << (const std::vector<char>& data);

		virtual std::istream& operator >> (std::vector<char>& data);

		virtual std::istream& operator >> (std::string& data);

		virtual std::ostream& operator << (std::string_view data);

		virtual ~IOSocketStream() = default;
	};

	template<typename T>
	int IOSocketStream::sendFundamental(T value)
	{
		try
		{
			return buffer->getNetwork()->sendBytes(&value, sizeof(value));
		}
		catch (const web::exceptions::WebException& e)
		{
			buffer->getNetwork()->log(e.what());

			if (!fail())
			{
				setstate(failbit);
			}

			throw;
		}
	}

	template<typename T>
	int IOSocketStream::receiveFundamental(T& value)
	{
		try
		{
			return buffer->getNetwork()->receiveBytes(&value, sizeof(value));
		}
		catch (const web::exceptions::WebException& e)
		{
			buffer->getNetwork()->log(e.what());

			if (!fail())
			{
				setstate(failbit);
			}

			throw;
		}
	}
}
