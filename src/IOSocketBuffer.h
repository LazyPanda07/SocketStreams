#pragma once

#include <streambuf>

#include "Network.h"

namespace buffers
{
	template<typename CharT, typename ContainerT = std::vector<CharT>>
	class IOSocketBuffer : public std::basic_streambuf<CharT>
	{
	public:
		using dataContainer = typename ContainerT;
		using NetworkOperations = typename web::Network<CharT, ContainerT>;
		using typename std::basic_streambuf<CharT>::int_type;
		using typename std::basic_streambuf<CharT>::char_type;
		using typename std::basic_streambuf<CharT>::traits_type;

		using std::basic_streambuf<CharT>::pbase;
		using std::basic_streambuf<CharT>::pptr;
		using std::basic_streambuf<CharT>::epptr;
		using std::basic_streambuf<CharT>::setp;
		using std::basic_streambuf<CharT>::pbump;

		using std::basic_streambuf<CharT>::eback;
		using std::basic_streambuf<CharT>::gptr;
		using std::basic_streambuf<CharT>::egptr;
		using std::basic_streambuf<CharT>::setg;
		using std::basic_streambuf<CharT>::gbump;

	protected:
		dataContainer outBuffer;
		dataContainer inBuffer;
		NetworkOperations* network;
		int lastPacketSize;

	protected:
		void setPointers();

		int_type overflow(int_type ch) override;

		int_type underflow() override;

		std::streamsize xsputn(const char_type* s, std::streamsize count) override;

		std::streamsize xsgetn(char_type* s, std::streamsize count) override;

		int sync() override;

		dataContainer dataPart() noexcept;

	protected:
		enum class IOType : char
		{
			input,
			output
		} type;

	public:
		IOSocketBuffer(SOCKET clientSocket);

		IOSocketBuffer(SOCKET clientSocket, size_t bufferSize);

		template<typename FirstStringT, typename SecondStringT>
		IOSocketBuffer(const FirstStringT& ip, const SecondStringT& port);

		template<typename FirstStringT, typename SecondStringT>
		IOSocketBuffer(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize);

		IOSocketBuffer(NetworkOperations* networkSubclass);

		IOSocketBuffer(NetworkOperations* networkSubclass, size_t bufferSize);

		virtual void setInputType() final;

		virtual void setOutputType() final;

		virtual NetworkOperations* getNetwork() final;

		virtual int getLastPacketSize();

		virtual ~IOSocketBuffer();
	};

	template<typename CharT, typename ContainerT>
	void IOSocketBuffer<CharT, ContainerT>::setPointers()
	{
		setp(outBuffer.data(), outBuffer.data() + outBuffer.size() - 1);
	}

	template<typename CharT, typename ContainerT>
	typename IOSocketBuffer<CharT, ContainerT>::int_type IOSocketBuffer<CharT, ContainerT>::overflow(int_type ch)
	{
		*pptr() = ch;
		pbump(1);
		type = IOType::output;

		if (this->sync() == -1)
		{
			return traits_type::eof();
		}

		return ch;
	}

	template<typename CharT, typename ContainerT>
	typename IOSocketBuffer<CharT, ContainerT>::int_type IOSocketBuffer<CharT, ContainerT>::underflow()
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

	template<typename CharT, typename ContainerT>
	std::streamsize IOSocketBuffer<CharT, ContainerT>::xsputn(const char_type* s, std::streamsize count)
	{
		type = IOType::output;

		if (network->getResizeMode() == NetworkOperations::ReceiveMode::allowResize && outBuffer.size() < count)
		{
			if constexpr (utility::checkResize<dataContainer>::value)
			{
				NetworkOperations::resizeFunction(outBuffer, count);
				this->setPointers();
			}
		}

		for (size_t i = 0; i < count; i++)
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

	template<typename CharT, typename ContainerT>
	std::streamsize IOSocketBuffer<CharT, ContainerT>::xsgetn(char_type* s, std::streamsize count)
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

		for (size_t i = 0; i < count; i++)
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

	template<typename CharT, typename ContainerT>
	int IOSocketBuffer<CharT, ContainerT>::sync()
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

			if constexpr (utility::checkBegin<dataContainer>::value && utility::checkEnd<dataContainer>::value && utility::checkInitializerListConstructor<dataContainer>::value)
			{
				lastPacketSize = network->sendData(this->dataPart());
			}
			else
			{
				lastPacketSize = network->sendData(outBuffer);
			}

			if (lastPacketSize == -1)
			{
				return -1;
			}

			pbump(-size);
		}

		break;
		}

		return 0;
	}

	template<typename CharT, typename ContainerT>
	typename IOSocketBuffer<CharT, ContainerT>::dataContainer IOSocketBuffer<CharT, ContainerT>::dataPart() noexcept
	{
		return dataContainer(pbase(), pptr());
	}

	template<typename CharT, typename ContainerT>
	IOSocketBuffer<CharT, ContainerT>::IOSocketBuffer(SOCKET clientSocket) : network(new NetworkOperations(clientSocket))
	{
		this->setPointers();
	}

	template<typename CharT, typename ContainerT>
	IOSocketBuffer<CharT, ContainerT>::IOSocketBuffer(SOCKET clientSocket, size_t bufferSize) : network(new NetworkOperations(clientSocket, NetworkOperations::ReceiveMode::prohibitResize)), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename CharT, typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketBuffer<CharT, ContainerT>::IOSocketBuffer(const FirstStringT& ip, const SecondStringT& port) : network(new NetworkOperations(ip, port))
	{
		this->setPointers();
	}

	template<typename CharT, typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketBuffer<CharT, ContainerT>::IOSocketBuffer(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize) : network(new NetworkOperations(ip, port, NetworkOperations::ReceiveMode::prohibitResize)), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename CharT, typename ContainerT>
	IOSocketBuffer<CharT, ContainerT>::IOSocketBuffer(NetworkOperations* networkSubclass) : network(networkSubclass)
	{
		this->setPointers();
	}

	template<typename CharT, typename ContainerT>
	IOSocketBuffer<CharT, ContainerT>::IOSocketBuffer(NetworkOperations* networkSubclass, size_t bufferSize) : network(networkSubclass), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename CharT, typename ContainerT>
	void IOSocketBuffer<CharT, ContainerT>::setInputType()
	{
		type = IOType::input;
	}

	template<typename CharT, typename ContainerT>
	void IOSocketBuffer<CharT, ContainerT>::setOutputType()
	{
		type = IOType::output;
	}

	template<typename CharT, typename ContainerT>
	typename IOSocketBuffer<CharT, ContainerT>::NetworkOperations* IOSocketBuffer<CharT, ContainerT>::getNetwork()
	{
		return network;
	}

	template<typename CharT, typename ContainerT>
	int IOSocketBuffer<CharT, ContainerT>::getLastPacketSize()
	{
		return lastPacketSize;
	}

	template<typename CharT, typename ContainerT>
	IOSocketBuffer<CharT, ContainerT>::~IOSocketBuffer()
	{
		delete network;
	}
}