#pragma once

#include <istream>

#include "BaseIOSocketBuffer.h"

namespace streams
{
	/// @brief Base input/output socket stream
	/// @tparam ContainerT 
	template<typename ContainerT = std::vector<char>>
	class BaseIOSocketStream : public std::iostream
	{
	protected:
		std::unique_ptr<buffers::BaseIOSocketBuffer<ContainerT>> buffer;

	protected:
		template<typename T>
		int sendFundamental(T value);

		template<typename T>
		int receiveFundamental(T& value);

	public:
		BaseIOSocketStream();

		/// @brief Deleted copy constructor
		BaseIOSocketStream(const BaseIOSocketStream&) = delete;

		/// @brief Move constructor
		/// @param other 
		BaseIOSocketStream(BaseIOSocketStream<ContainerT>&& other) noexcept;

		/// @brief Server side contructor
		/// @param clientSocket 
		BaseIOSocketStream(SOCKET clientSocket);

		/// @brief Server side constructor
		/// @param clientSocket 
		/// @param bufferSize 
		BaseIOSocketStream(SOCKET clientSocket, size_t bufferSize);

		/// @brief Client side contructor
		/// @tparam HostStringT 
		/// @tparam PortStringT 
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		template<typename HostStringT, typename PortStringT>
		BaseIOSocketStream(const HostStringT& ip, const PortStringT& port);

		/// @brief Client side contructor
		/// @tparam HostStringT 
		/// @tparam PortStringT 
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param bufferSize 
		template<typename HostStringT, typename PortStringT>
		BaseIOSocketStream(const HostStringT& ip, const PortStringT& port, size_t bufferSize);

		BaseIOSocketStream(std::unique_ptr<buffers::BaseIOSocketBuffer<ContainerT>>&& buffer);

		/// @brief Constructor with custom network and default buffer
		/// @param network 
		BaseIOSocketStream(std::unique_ptr<web::BaseNetwork<ContainerT>>&& network);

		/// @brief Constructor with custom network and default buffer with buffer size
		/// @param network 
		/// @param bufferSize 
		BaseIOSocketStream(std::unique_ptr<web::BaseNetwork<ContainerT>>&& network, size_t bufferSize);

		/// @brief Deleted copy assignment operator
		/// @param  
		/// @return 
		BaseIOSocketStream<ContainerT>& operator = (const BaseIOSocketStream<ContainerT>&) = delete;

		/// @brief Move assignment operator
		/// @param other 
		/// @return 
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

		virtual std::iostream& operator << (std::string_view data);

		virtual ~BaseIOSocketStream() = default;
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
		std::iostream(nullptr)
	{

	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(SOCKET clientSocket) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::BaseIOSocketBuffer<ContainerT>>(clientSocket))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(SOCKET clientSocket, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::BaseIOSocketBuffer<ContainerT>>(clientSocket, bufferSize))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	template<typename HostStringT, typename PortStringT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(const HostStringT& ip, const PortStringT& port) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::BaseIOSocketBuffer<ContainerT>>(ip, port))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	template<typename HostStringT, typename PortStringT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(const HostStringT& ip, const PortStringT& port, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::BaseIOSocketBuffer<ContainerT>>(ip, port, bufferSize))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(std::unique_ptr<buffers::BaseIOSocketBuffer<ContainerT>>&& buffer) :
		std::iostream(nullptr),
		buffer(std::move(buffer))
	{
		std::iostream::rdbuf(this->buffer.get());
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(std::unique_ptr<web::BaseNetwork<ContainerT>>&& network) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(std::move(network)))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(std::unique_ptr<web::BaseNetwork<ContainerT>>&& network, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(std::move(network), bufferSize))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>::BaseIOSocketStream(BaseIOSocketStream<ContainerT>&& other) noexcept :
		std::iostream(nullptr),
		buffer(std::move(other.buffer))
	{
		std::iostream::rdbuf(buffer.get());
	}

	template<typename ContainerT>
	BaseIOSocketStream<ContainerT>& BaseIOSocketStream<ContainerT>::operator = (BaseIOSocketStream<ContainerT>&& other) noexcept
	{
		buffer = std::move(other.buffer);

		std::iostream::rdbuf(buffer.get());

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
	std::iostream& BaseIOSocketStream<ContainerT>::operator << (std::string_view data)
	{
		if (buffer->sputn(data.data(), data.size()) == -1)
		{
			throw web::exceptions::WebException();
		}

		return *this;
	}

	using IOSocketStream = BaseIOSocketStream<std::vector<char>>;
}