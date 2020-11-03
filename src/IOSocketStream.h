#pragma once

#include <istream>

#include "IOSocketBuffer.h"

namespace streams
{
	template<typename ContainerT = std::vector<char>>
	class IOSocketStream : public std::iostream
	{
	public:

	protected:
		buffers::IOSocketBuffer<ContainerT>* buffer;

	protected:
		template<typename T>
		int sendFundamental(T value);

		template<typename T>
		int receiveFundamental(T& value);

	public:
		IOSocketStream(SOCKET clientSocket);

		IOSocketStream(SOCKET clientSocket, size_t bufferSize);

		template<typename FirstStringT, typename SecondStringT>
		IOSocketStream(const FirstStringT& ip, const SecondStringT& port);

		template<typename FirstStringT, typename SecondStringT>
		IOSocketStream(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize);

		IOSocketStream(buffers::IOSocketBuffer<ContainerT>* IOSocketBufferSubclass);

		IOSocketStream(IOSocketStream<ContainerT>&& other) noexcept;

		IOSocketStream<ContainerT>& operator = (IOSocketStream<ContainerT>&& other) noexcept;

		std::iostream& operator << (bool value);
		std::iostream& operator << (short value);
		std::iostream& operator << (int value);
		std::iostream& operator << (long value);
		std::iostream& operator << (long long value);

		std::iostream& operator << (unsigned short value);
		std::iostream& operator << (unsigned int value);
		std::iostream& operator << (unsigned long value);
		std::iostream& operator << (unsigned long long value);

		std::iostream& operator << (float value);
		std::iostream& operator << (double value);
		std::iostream& operator << (long double value);

		std::iostream& operator >> (bool& value);
		std::iostream& operator >> (short& value);
		std::iostream& operator >> (int& value);
		std::iostream& operator >> (long& value);
		std::iostream& operator >> (long long& value);

		std::iostream& operator >> (unsigned short& value);
		std::iostream& operator >> (unsigned int& value);
		std::iostream& operator >> (unsigned long& value);
		std::iostream& operator >> (unsigned long long& value);

		std::iostream& operator >> (float& value);
		std::iostream& operator >> (double& value);
		std::iostream& operator >> (long double& value);

		virtual std::iostream& operator << (const ContainerT& data);

		virtual std::iostream& operator >> (ContainerT& data);

		virtual std::iostream& operator << (const std::string& data);

		virtual std::iostream& operator >> (std::string& data);

		virtual std::iostream& operator << (const std::string_view& data);

		virtual ~IOSocketStream();
	};

	template<typename ContainerT>
	template<typename T>
	int IOSocketStream<ContainerT>::sendFundamental(T value)
	{
		try
		{
			return buffer->getNetwork()->sendBytes(&value, sizeof(value));
		}
		catch (web::WebException& e)
		{
			buffer->getNetwork()->log(e.what());

			throw std::move(e);
		}
	}

	template<typename ContainerT>
	template<typename T>
	int IOSocketStream<ContainerT>::receiveFundamental(T& value)
	{
		try
		{
			return buffer->getNetwork()->receiveBytes(&value, sizeof(value));
		}
		catch (web::WebException& e)
		{
			buffer->getNetwork()->log(e.what());

			throw std::move(e);
		}
	}

	template<typename ContainerT>
	IOSocketStream<ContainerT>::IOSocketStream(SOCKET clientSocket) : buffer(new buffers::IOSocketBuffer<ContainerT>(clientSocket)), std::iostream(buffer)
	{

	}

	template<typename ContainerT>
	IOSocketStream<ContainerT>::IOSocketStream(SOCKET clientSocket, size_t bufferSize) : buffer(new buffers::IOSocketBuffer<ContainerT>(clientSocket, bufferSize)), std::iostream(buffer)
	{

	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketStream<ContainerT>::IOSocketStream(const FirstStringT& ip, const SecondStringT& port) : buffer(new buffers::IOSocketBuffer<ContainerT>(ip, port)), std::iostream(buffer)
	{

	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketStream<ContainerT>::IOSocketStream(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize) : buffer(new buffers::IOSocketBuffer<ContainerT>(ip, port, bufferSize)), std::iostream(buffer)
	{

	}

	template<typename ContainerT>
	IOSocketStream<ContainerT>::IOSocketStream(buffers::IOSocketBuffer<ContainerT>* IOSocketBufferSubclass) : buffer(IOSocketBufferSubclass), std::iostream(buffer)
	{

	}

	template<typename ContainerT>
	IOSocketStream<ContainerT>::IOSocketStream(IOSocketStream<ContainerT>&& other) noexcept :
		std::iostream(nullptr)
	{
		*this = std::move(other);
	}

	template<typename ContainerT>
	IOSocketStream<ContainerT>& IOSocketStream<ContainerT>::operator = (IOSocketStream<ContainerT>&& other) noexcept
	{
		delete buffer;
		buffer = other.buffer;
		other.buffer = nullptr;

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (bool value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (short value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (int value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (long long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (unsigned short value)
	{
		try {
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (unsigned int value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (unsigned long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (unsigned long long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (float value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (double value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}
	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (long double value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		};

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (bool& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (short& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (int& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (long long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (unsigned short& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (unsigned int& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (unsigned long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (unsigned long long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (float& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (double& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (long double& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (web::WebException& e)
		{
			throw std::move(e);
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (const ContainerT& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (ContainerT& data)
	{
		if constexpr (utility::checkResize<ContainerT>::value)
		{
			buffer->setInputType();
			if (buffer->pubsync() == -1)
			{
				throw web::WebException();
			}
			data.resize(buffer->getLastPacketSize());
		}

		if (buffer->sgetn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (const std::string& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator >> (std::string& data)
	{
		buffer->setInputType();

		if (buffer->pubsync() == -1)
		{
			throw web::WebException();
		}

		data.resize(buffer->getLastPacketSize());

		if (buffer->sgetn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& IOSocketStream<ContainerT>::operator << (const std::string_view& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	IOSocketStream<ContainerT>::~IOSocketStream()
	{
		delete buffer;
	}
}