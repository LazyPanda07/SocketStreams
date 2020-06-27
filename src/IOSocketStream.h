#pragma once

#include <istream>

#include "IOSocketBuffer.h"

namespace streams
{
	template<typename CharT, typename ContainerT = std::vector<CharT>>
	class IOSocketStream : public std::basic_iostream<CharT>
	{
	public:
		using dataContainer = typename ContainerT;
		using ioBuffer = typename buffers::IOSocketBuffer<CharT, ContainerT>;

	protected:
		ioBuffer* buffer;

	protected:
		template<typename T>
		int_fast32_t sendFundamental(T value);

		template<typename T>
		int_fast32_t receiveFundamental(T& value);

	public:
		IOSocketStream(SOCKET clientSocket);

		IOSocketStream(SOCKET clientSocket, size_t bufferSize);

		template<typename FirstStringT, typename SecondStringT>
		IOSocketStream(const FirstStringT& ip, const SecondStringT& port);

		template<typename FirstStringT, typename SecondStringT>
		IOSocketStream(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize);

		IOSocketStream(ioBuffer* IOSocketBufferSubclass);

		std::basic_iostream<CharT>& operator << (short value) override;
		std::basic_iostream<CharT>& operator << (int value) override;
		std::basic_iostream<CharT>& operator << (long value) override;
		std::basic_iostream<CharT>& operator << (long long value) override;

		std::basic_iostream<CharT>& operator << (unsigned short value) override;
		std::basic_iostream<CharT>& operator << (unsigned int value) override;
		std::basic_iostream<CharT>& operator << (unsigned long value) override;
		std::basic_iostream<CharT>& operator << (unsigned long long value) override;

		std::basic_iostream<CharT>& operator << (float value) override;
		std::basic_iostream<CharT>& operator << (double value) override;
		std::basic_iostream<CharT>& operator << (long double value) override;

		std::basic_iostream<CharT>& operator >> (short& value) override;
		std::basic_iostream<CharT>& operator >> (int& value) override;
		std::basic_iostream<CharT>& operator >> (long& value) override;
		std::basic_iostream<CharT>& operator >> (long long& value) override;

		std::basic_iostream<CharT>& operator >> (unsigned short& value) override;
		std::basic_iostream<CharT>& operator >> (unsigned int& value) override;
		std::basic_iostream<CharT>& operator >> (unsigned long& value) override;
		std::basic_iostream<CharT>& operator >> (unsigned long long& value) override;

		std::basic_iostream<CharT>& operator >> (float& value) override;
		std::basic_iostream<CharT>& operator >> (double& value) override;
		std::basic_iostream<CharT>& operator >> (long double& value) override;

		virtual std::basic_iostream<CharT>& operator << (const dataContainer& data);

		virtual std::basic_iostream<CharT>& operator >> (dataContainer& data);

		virtual std::basic_iostream<CharT>& operator << (const std::basic_string<CharT>& data);

		virtual std::basic_iostream<CharT>& operator >> (std::basic_string<CharT>& data);

		virtual std::basic_iostream<CharT>& operator << (const std::basic_string_view<CharT>& data);

		virtual ~IOSocketStream();
	};

	template<typename CharT, typename ContainerT>
	template<typename T>
	int_fast32_t IOSocketStream<CharT, ContainerT>::sendFundamental(T value)
	{
		try
		{
			return buffer->network->sendBytes(&value, sizeof(value));
		}
		catch (web::WebException& e)
		{
			buffer->network->log(e.what());

			throw std::move(e);
		}
	}

	template<typename CharT, typename ContainerT>
	template<typename T>
	int_fast32_t IOSocketStream<CharT, ContainerT>::receiveFundamental(T& value)
	{
		try
		{
			return buffer->network->receiveBytes(&value, sizeof(value));
		}
		catch (web::WebException& e)
		{
			buffer->network->log(e.what());

			throw std::move(e);
		}
	}

	template<typename CharT, typename ContainerT>
	IOSocketStream<CharT, ContainerT>::IOSocketStream(SOCKET clientSocket) : buffer(new ioBuffer(clientSocket)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename ContainerT>
	IOSocketStream<CharT, ContainerT>::IOSocketStream(SOCKET clientSocket, size_t bufferSize) : buffer(new ioBuffer(clientSocket, bufferSize)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketStream<CharT, ContainerT>::IOSocketStream(const FirstStringT& ip, const SecondStringT& port) : buffer(new ioBuffer(ip, port)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketStream<CharT, ContainerT>::IOSocketStream(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize) : buffer(new ioBuffer(ip, port, bufferSize)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename ContainerT>
	IOSocketStream<CharT, ContainerT>::IOSocketStream(IOSocketStream<CharT, ContainerT>::ioBuffer* IOSocketBufferSubclass) : buffer(IOSocketBufferSubclass), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (short value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (int value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (long value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (long long value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (unsigned short value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (unsigned int value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (unsigned long value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (unsigned long long value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (float value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (double value)
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
	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (long double value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (short& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (int& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (long& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (long long& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (unsigned short& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (unsigned int& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (unsigned long& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (unsigned long long& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (float& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (double& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (long double& value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (const IOSocketStream<CharT, ContainerT>::dataContainer& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (IOSocketStream<CharT, ContainerT>::dataContainer& data)
	{
		if constexpr (utility::checkResize<dataContainer>::value)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (const std::basic_string<CharT>& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator >> (std::basic_string<CharT>& data)
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

	template<typename CharT, typename ContainerT>
	std::basic_iostream<CharT>& IOSocketStream<CharT, ContainerT>::operator << (const std::basic_string_view<CharT>& data)
	{
		if (buffer->sputn(data.data(), data.size()))
		{
			throw web::WebException();
		}

		return *this;
	}

	template<typename CharT, typename ContainerT>
	IOSocketStream<CharT, ContainerT>::~IOSocketStream()
	{
		delete buffer;
	}
}