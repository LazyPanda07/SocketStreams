#pragma once

#include <streambuf>
#include <memory>

#include "BaseNetwork.h"

namespace buffers
{
	/// @brief Base input/output socket buffer
	/// @tparam ContainerT 
	template<typename ContainerT = std::vector<char>>
	class BaseIOSocketBuffer : public std::streambuf
	{
	public:
		using typename std::streambuf::int_type;
		using typename std::streambuf::char_type;
		using typename std::streambuf::traits_type;

		using std::streambuf::pbase;
		using std::streambuf::pptr;
		using std::streambuf::epptr;
		using std::streambuf::setp;
		using std::streambuf::pbump;

		using std::streambuf::eback;
		using std::streambuf::gptr;
		using std::streambuf::egptr;
		using std::streambuf::setg;
		using std::streambuf::gbump;

	protected:
		enum class IOType
		{
			input,
			output
		};

	protected:
		ContainerT outBuffer;
		ContainerT inBuffer;
		std::unique_ptr<web::BaseNetwork<ContainerT>> network;
		int lastPacketSize;
		IOType type;

	protected:
		void setPointers();

		int_type overflow(int_type ch) override;

		int_type underflow() override;

		std::streamsize xsputn(const char_type* s, std::streamsize count) override;

		std::streamsize xsgetn(char_type* s, std::streamsize count) override;

		int sync() override;

		ContainerT dataPart() noexcept;

	public:
		BaseIOSocketBuffer() = default;

		/// @brief Deleted copy contructor
		BaseIOSocketBuffer(const BaseIOSocketBuffer&) = delete;

		/// @brief Move constructor
		/// @param other 
		BaseIOSocketBuffer(BaseIOSocketBuffer&& other) noexcept;

		/// @brief Server side constructor
		/// @param clientSocket 
		BaseIOSocketBuffer(SOCKET clientSocket);

		/// @brief Server side constructor
		/// @param clientSocket 
		/// @param bufferSize Fixed buffer size
		BaseIOSocketBuffer(SOCKET clientSocket, size_t bufferSize);

		/// @brief Client side constructor
		/// @tparam HostStringT 
		/// @tparam PortStringT 
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		template<typename HostStringT, typename PortStringT>
		BaseIOSocketBuffer(const HostStringT& ip, const PortStringT& port, DWORD timeout = 30'000, web::Network::receiveMode mode = web::Network::receiveMode::allowResize);

		/// @brief Client side constructor
		/// @tparam HostStringT 
		/// @tparam PortStringT 
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param bufferSize 
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		template<typename HostStringT, typename PortStringT>
		BaseIOSocketBuffer(const HostStringT& ip, const PortStringT& port, size_t bufferSize, DWORD timeout = 30'000, web::Network::receiveMode mode = web::Network::receiveMode::allowResize);

		BaseIOSocketBuffer(std::unique_ptr<web::BaseNetwork<ContainerT>>&& networkSubclass);

		BaseIOSocketBuffer(std::unique_ptr<web::BaseNetwork<ContainerT>>&& networkSubclass, size_t bufferSize);

		/// @brief Deleted copy assignment operator
		/// @param  
		/// @return 
		BaseIOSocketBuffer& operator = (const BaseIOSocketBuffer&) = delete;

		/// @brief Move assignment operator 
		/// @param other 
		/// @return Self
		BaseIOSocketBuffer& operator = (BaseIOSocketBuffer&& other) noexcept;

		virtual void setInputType() final;

		virtual void setOutputType() final;

		virtual const std::unique_ptr<web::BaseNetwork<ContainerT>>& getNetwork() const final;

		virtual std::unique_ptr<web::BaseNetwork<ContainerT>>& getNetwork() final;

		virtual int getLastPacketSize();

		virtual ~BaseIOSocketBuffer() = default;
	};

	template<typename ContainerT>
	void BaseIOSocketBuffer<ContainerT>::setPointers()
	{
		setp(outBuffer.data(), outBuffer.data() + outBuffer.size() - 1);
	}

	template<typename ContainerT>
	typename BaseIOSocketBuffer<ContainerT>::int_type BaseIOSocketBuffer<ContainerT>::overflow(int_type ch)
	{
		*pptr() = ch;

		pbump(1);

		type = IOType::output;

		if (this->sync() == -1)
		{
			return traits_type::eof();
		}

		return 0;
	}

	template<typename ContainerT>
	typename BaseIOSocketBuffer<ContainerT>::int_type BaseIOSocketBuffer<ContainerT>::underflow()
	{
		type = IOType::input;

		if (!eback())
		{
			if (this->sync() == -1)
			{
				return traits_type::eof();
			}
		}

		if (gptr() < egptr())
		{
			return *gptr();
		}

		setg(nullptr, nullptr, nullptr);

		return traits_type::eof();
	}

	template<typename ContainerT>
	std::streamsize BaseIOSocketBuffer<ContainerT>::xsputn(const char_type* s, std::streamsize count)
	{
		type = IOType::output;

		if (network->getResizeMode() == web::BaseNetwork<ContainerT>::receiveMode::allowResize && static_cast<std::streamsize>(outBuffer.size()) < count)
		{
			if constexpr (utility::checkResize<ContainerT>::value)
			{
				web::BaseNetwork<ContainerT>::resizeFunction(outBuffer, static_cast<size_t>(count));

				this->setPointers();
			}
		}

		for (std::streamsize i = 0; i < count; i++)
		{
			if (pptr() == epptr())
			{
				if (this->overflow(s[i]) == -1)
				{
					return -1;
				}
			}
			else
			{
				*pptr() = s[i];

				pbump(1);
			}
		}

		if (pptr() != pbase())
		{
			if (this->sync() == -1)
			{
				return -1;
			}
		}

		return count;
	}

	template<typename ContainerT>
	std::streamsize BaseIOSocketBuffer<ContainerT>::xsgetn(char_type* s, std::streamsize count)
	{
		type = IOType::input;

		if (!eback())
		{
			if (this->sync() == -1)
			{
				return -1;
			}
		}

		ptrdiff_t size = egptr() - eback();

		if (!size)
		{
			return 0;
		}

		for (std::streamsize i = 0; i < count; i++)
		{
			if (gptr() == egptr())
			{
				s[i] = *gptr();

				setg(nullptr, nullptr, nullptr);

				return size;
			}
			else
			{
				s[i] = *gptr();

				gbump(1);
			}
		}

		return count ? count : traits_type::eof();
	}

	template<typename ContainerT>
	int BaseIOSocketBuffer<ContainerT>::sync()
	{
		switch (type)
		{
		case IOType::input:
			lastPacketSize = network->receiveData(inBuffer);

			if (lastPacketSize == -1)
			{
				return -1;
			}

			setg(inBuffer.data(), inBuffer.data(), inBuffer.data() + lastPacketSize);

			break;

		case IOType::output:
		{
			ptrdiff_t size = pptr() - pbase();

			if constexpr (utility::checkBegin<ContainerT>::value && utility::checkEnd<ContainerT>::value && utility::checkInitializerListConstructor<ContainerT>::value)
			{
				lastPacketSize = network->sendData(this->dataPart());
			}
			else
			{
				lastPacketSize = network->sendData(outBuffer);
			}

			pbump(static_cast<int>(-size));

			if (lastPacketSize == -1)
			{
				return -1;
			}
		}

		break;
		}

		return 0;
	}

	template<typename ContainerT>
	ContainerT BaseIOSocketBuffer<ContainerT>::dataPart() noexcept
	{
		return ContainerT(pbase(), pptr());
	}

	template<typename ContainerT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(BaseIOSocketBuffer&& other) noexcept :
		outBuffer(std::move(other.outBuffer)),
		inBuffer(std::move(other.inBuffer)),
		network(std::move(other.network)),
		lastPacketSize(other.lastPacketSize),
		type(other.type)
	{

	}

	template<typename ContainerT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(SOCKET clientSocket) :
		network(std::make_unique<web::BaseNetwork<ContainerT>>(clientSocket))
	{
		this->setPointers();
	}

	template<typename ContainerT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(SOCKET clientSocket, size_t bufferSize) :
		network(std::make_unique<web::BaseNetwork<ContainerT>>(clientSocket, web::BaseNetwork<ContainerT>::receiveMode::prohibitResize)),
		outBuffer(bufferSize),
		inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	template<typename HostStringT, typename PortStringT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(const HostStringT& ip, const PortStringT& port, DWORD timeout, web::Network::receiveMode mode) :
		network(std::make_unique<web::BaseNetwork<ContainerT>>(ip, port, timeout, mode))
	{
		this->setPointers();
	}

	template<typename ContainerT>
	template<typename HostStringT, typename PortStringT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(const HostStringT& ip, const PortStringT& port, size_t bufferSize, DWORD timeout, web::Network::receiveMode mode) :
		network(std::make_unique<web::BaseNetwork<ContainerT>>(ip, port, timeout, mode, web::BaseNetwork<ContainerT>::receiveMode::prohibitResize)),
		outBuffer(bufferSize),
		inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(std::unique_ptr<web::BaseNetwork<ContainerT>>&& networkSubclass) :
		network(std::move(networkSubclass))
	{
		this->setPointers();
	}

	template<typename ContainerT>
	BaseIOSocketBuffer<ContainerT>::BaseIOSocketBuffer(std::unique_ptr<web::BaseNetwork<ContainerT>>&& networkSubclass, size_t bufferSize) :
		network(std::move(networkSubclass)),
		outBuffer(bufferSize),
		inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	BaseIOSocketBuffer<ContainerT>& BaseIOSocketBuffer<ContainerT>::operator = (BaseIOSocketBuffer&& other) noexcept
	{
		outBuffer = std::move(other.outBuffer);
		inBuffer = std::move(other.inBuffer);
		network = std::move(other.network);
		lastPacketSize = other.lastPacketSize;
		type = other.type;

		return *this;
	}

	template<typename ContainerT>
	void BaseIOSocketBuffer<ContainerT>::setInputType()
	{
		type = IOType::input;
	}

	template<typename ContainerT>
	void BaseIOSocketBuffer<ContainerT>::setOutputType()
	{
		type = IOType::output;
	}

	template<typename ContainerT>
	const std::unique_ptr<web::BaseNetwork<ContainerT>>& BaseIOSocketBuffer<ContainerT>::getNetwork() const
	{
		return network;
	}

	template<typename ContainerT>
	std::unique_ptr<web::BaseNetwork<ContainerT>>& BaseIOSocketBuffer<ContainerT>::getNetwork()
	{
		return network;
	}

	template<typename ContainerT>
	int BaseIOSocketBuffer<ContainerT>::getLastPacketSize()
	{
		return lastPacketSize;
	}

	using IOSocketBuffer = BaseIOSocketBuffer<std::vector<char>>;
}
