#pragma once

#include <streambuf>

#include "Network.h"

namespace buffers
{
	template<typename ContainerT = std::vector<char>>
	class IOSocketBuffer : public std::streambuf
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
		ContainerT outBuffer;
		ContainerT inBuffer;
		web::Network<ContainerT>* network;
		int lastPacketSize;

	protected:
		void setPointers();

		int_type overflow(int_type ch) override;

		int_type underflow() override;

		std::streamsize xsputn(const char_type* s, std::streamsize count) override;

		std::streamsize xsgetn(char_type* s, std::streamsize count) override;

		int sync() override;

		ContainerT dataPart() noexcept;

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

		IOSocketBuffer(web::Network<ContainerT>* networkSubclass);

		IOSocketBuffer(web::Network<ContainerT>* networkSubclass, size_t bufferSize);

		virtual void setInputType() final;

		virtual void setOutputType() final;

		virtual web::Network<ContainerT>* getNetwork() final;

		virtual int getLastPacketSize();

		virtual ~IOSocketBuffer();
	};

	template<typename ContainerT>
	void IOSocketBuffer<ContainerT>::setPointers()
	{
		setp(outBuffer.data(), outBuffer.data() + outBuffer.size() - 1);
	}

	template<typename ContainerT>
	typename IOSocketBuffer<ContainerT>::int_type IOSocketBuffer<ContainerT>::overflow(int_type ch)
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
	typename IOSocketBuffer<ContainerT>::int_type IOSocketBuffer<ContainerT>::underflow()
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
	std::streamsize IOSocketBuffer<ContainerT>::xsputn(const char_type* s, std::streamsize count)
	{
		type = IOType::output;

		if (network->getResizeMode() == web::Network<ContainerT>::ReceiveMode::allowResize && outBuffer.size() < count)
		{
			if constexpr (utility::checkResize<ContainerT>::value)
			{
				web::Network<ContainerT>::resizeFunction(outBuffer, count);
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

	template<typename ContainerT>
	std::streamsize IOSocketBuffer<ContainerT>::xsgetn(char_type* s, std::streamsize count)
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

	template<typename ContainerT>
	int IOSocketBuffer<ContainerT>::sync()
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

			pbump(-size);

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
	typename ContainerT IOSocketBuffer<ContainerT>::dataPart() noexcept
	{
		return ContainerT(pbase(), pptr());
	}

	template<typename ContainerT>
	IOSocketBuffer<ContainerT>::IOSocketBuffer(SOCKET clientSocket) : network(new web::Network<ContainerT>(clientSocket))
	{
		this->setPointers();
	}

	template<typename ContainerT>
	IOSocketBuffer<ContainerT>::IOSocketBuffer(SOCKET clientSocket, size_t bufferSize) : network(new web::Network<ContainerT>(clientSocket, web::Network<ContainerT>::ReceiveMode::prohibitResize)), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketBuffer<ContainerT>::IOSocketBuffer(const FirstStringT& ip, const SecondStringT& port) : network(new web::Network<ContainerT>(ip, port))
	{
		this->setPointers();
	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	IOSocketBuffer<ContainerT>::IOSocketBuffer(const FirstStringT& ip, const SecondStringT& port, size_t bufferSize) : network(new web::Network<ContainerT>(ip, port, web::Network<ContainerT>::ReceiveMode::prohibitResize)), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	IOSocketBuffer<ContainerT>::IOSocketBuffer(web::Network<ContainerT>* networkSubclass) : network(networkSubclass)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	IOSocketBuffer<ContainerT>::IOSocketBuffer(web::Network<ContainerT>* networkSubclass, size_t bufferSize) : network(networkSubclass), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename ContainerT>
	void IOSocketBuffer<ContainerT>::setInputType()
	{
		type = IOType::input;
	}

	template<typename ContainerT>
	void IOSocketBuffer<ContainerT>::setOutputType()
	{
		type = IOType::output;
	}

	template<typename ContainerT>
	web::Network<ContainerT>* IOSocketBuffer<ContainerT>::getNetwork()
	{
		return network;
	}

	template<typename ContainerT>
	int IOSocketBuffer<ContainerT>::getLastPacketSize()
	{
		return lastPacketSize;
	}

	template<typename ContainerT>
	IOSocketBuffer<ContainerT>::~IOSocketBuffer()
	{
		delete network;
	}
}