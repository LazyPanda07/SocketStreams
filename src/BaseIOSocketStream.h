#pragma once

#include <istream>

#include "BaseIOSocketBuffer.h"

namespace streams
{
	template<typename ContainerT = std::vector<char>>
	class BaseIOSocketStream : public std::iostream
	{
	protected:
		buffers::BaseIOSocketBuffer<ContainerT>* buffer;

	protected:
		template<typename T>
		int sendFundamental(T value);

		template<typename T>
		int receiveFundamental(T& value);

	public:
		BaseIOSocketStream();

		BaseIOSocketStream(SOCKET clientSocket);

		BaseIOSocketStream(SOCKET clientSocket, size_t bufferSize);

		template<typename FirstStringT, typename SecondStringT>
		BaseIOSocketStream(const FirstStringT& ip, const SecondStringT& port);

		template<typename FirstStringT, typename SecondStringT>
		BaseIOSocketStream(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize);

		BaseIOSocketStream(buffers::BaseIOSocketBuffer<ContainerT>* IOSocketBufferSubclass);

		BaseIOSocketStream(BaseIOSocketStream<ContainerT>&& other) noexcept;

		BaseIOSocketStream<ContainerT>& operator = (BaseIOSocketStream<ContainerT>&& other) noexcept;

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

		virtual ~BaseIOSocketStream();
	};

	template<typename ContainerT>
	template<typename T>
	int BaseIOSocketStream<ContainerT>::sendFundamental(T value)
	{
		try
		{
			return buffer->getNetwork()->sendBytes(&value, sizeof(value));
		}
		catch (const web::exceptions::WebException& e)
		{
			buffer->getNetwork()->log(e.what());

			throw;
		}
	}

	template<typename ContainerT>
	template<typename T>
	int BaseIOSocketStream<ContainerT>::receiveFundamental(T& value)
	{
		try
		{
			return buffer->getNetwork()->receiveBytes(&value, sizeof(value));
		}
		catch (const web::exceptions::WebException& e)
		{
			buffer->getNetwork()->log(e.what());

			throw;
		}
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream() :
		std::iostream(nullptr),
		buffer(nullptr)
	{

	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(SOCKET clientSocket) :
		std::iostream(nullptr),
		buffer(new buffers::BaseIOSocketBuffer<ContainerT>(clientSocket))
	{
		std::iostream::rdbuf(buffer);
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(SOCKET clientSocket, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(new buffers::BaseIOSocketBuffer<ContainerT>(clientSocket, bufferSize))
	{
		std::iostream::rdbuf(buffer);
	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(const FirstStringT& ip, const SecondStringT& port) :
		std::iostream(nullptr),
		buffer(new buffers::BaseIOSocketBuffer<ContainerT>(ip, port))
	{
		std::iostream::rdbuf(buffer);
	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(new buffers::BaseIOSocketBuffer<ContainerT>(ip, port, bufferSize))
	{
		std::iostream::rdbuf(buffer);
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(buffers::BaseIOSocketBuffer<ContainerT>* IOSocketBufferSubclass) :
		std::iostream(nullptr),
		buffer(IOSocketBufferSubclass)
	{
		std::iostream::rdbuf(buffer);
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(BaseIOSocketStream<ContainerT>&& other) noexcept :
		std::iostream(nullptr)
	{
		*this = std::move(other);
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>& BaseIOSocketStream<ContainerT>::operator = (BaseIOSocketStream<ContainerT>&& other) noexcept
	{
		delete buffer;
		buffer = other.buffer;
		other.buffer = nullptr;

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (bool value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (short value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (int value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (long long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (unsigned short value)
	{
		try {
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (unsigned int value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (unsigned long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (unsigned long long value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (float value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (double value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}
	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (long double value)
	{
		try
		{
			this->sendFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		};

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (bool& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (short& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (int& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (long long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (unsigned short& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (unsigned int& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (unsigned long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (unsigned long long& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (float& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (double& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (long double& value)
	{
		try
		{
			this->receiveFundamental(value);
		}
		catch (const web::exceptions::WebException&)
		{
			throw;
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (const ContainerT& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::exceptions::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (ContainerT& data)
	{
		if constexpr (utility::checkResize<ContainerT>::value)
		{
			buffer->setInputType();
			if (buffer->pubsync() == -1)
			{
				throw web::exceptions::WebException();
			}
			data.resize(buffer->getLastPacketSize());
		}

		if (buffer->sgetn(data.data(), data.size()) == -1)
		{
			throw web::exceptions::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (const std::string& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::exceptions::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator >> (std::string& data)
	{
		buffer->setInputType();

		if (buffer->pubsync() == -1)
		{
			throw web::exceptions::WebException();
		}

		data.resize(buffer->getLastPacketSize());

		if (buffer->sgetn(data.data(), data.size()) == -1)
		{
			throw web::exceptions::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (const std::string_view& data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::exceptions::WebException();
		}

		return *this;
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::~BaseIOSocketStream()
	{
		delete buffer;
	}

	using IOSocketStream = BaseIOSocketStream<std::vector<char>>;
}