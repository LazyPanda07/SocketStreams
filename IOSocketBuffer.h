#pragma once

#include <streambuf>

#include "Network.h"

namespace buffers
{
	template<typename CharT, typename container = std::vector<CharT>>
	class IOSocketBuffer : public std::basic_streambuf<CharT>
	{
	public:
		using dataContainer = typename container;
		using typename std::basic_streambuf<CharT>::int_type;
		using typename std::basic_streambuf<CharT>::char_type;
		using NetworkOperations = typename web::Network<CharT, container>;

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


	protected:
		dataContainer outBuffer;
		dataContainer inBuffer;
		NetworkOperations* network;
		__int32 lastPacketSize;

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

		IOSocketBuffer(const std::string& ip, const std::string& port);

		IOSocketBuffer(const std::string& ip, const std::string& port, size_t bufferSize);

		IOSocketBuffer(NetworkOperations* networkSubclass);

		IOSocketBuffer(NetworkOperations* networkSubclass, size_t bufferSize);

		virtual void setInputType() final;

		virtual void setOutputType() final;

		virtual __int32 getLastPacketSize();

		virtual ~IOSocketBuffer();
	};

	template<typename CharT, typename container>
	void IOSocketBuffer<CharT, container>::setPointers()
	{
		setp(outBuffer.data(), outBuffer.data() + outBuffer.size() - 1);
	}

	template<typename CharT, typename container>
	typename IOSocketBuffer<CharT, container>::int_type IOSocketBuffer<CharT, container>::overflow(int_type ch)
	{
		*pptr() = ch;
		pbump(1);
		type = IOType::output;

		this->sync();

		return ch;
	}

	template<typename CharT, typename container>
	typename IOSocketBuffer<CharT, container>::int_type IOSocketBuffer<CharT, container>::underflow()
	{
		type = IOType::input;

		if (!eback())
		{
			this->sync();
		}

		if (gptr() < egptr())
		{
			return *gptr();
		}

		setg(nullptr, nullptr, nullptr);

		return std::basic_streambuf<CharT>::traits_type::eof();
	}

	template<typename CharT, typename container>
	std::streamsize IOSocketBuffer<CharT, container>::xsputn(const char_type* s, std::streamsize count)
	{
		type = IOType::output;

		if (network->getResizeMode() == NetworkOperations::ReceiveMode::allowResize && !pptr())
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
				this->overflow(s[i]);
			}
			else
			{
				*pptr() = s[i];
				pbump(1);
			}
		}

		if (pptr() != pbase())
		{
			this->sync();
		}

		return count;
	}

	template<typename CharT, typename container>
	std::streamsize IOSocketBuffer<CharT, container>::xsgetn(char_type* s, std::streamsize count)
	{
		type = IOType::input;

		if (!eback())
		{
			this->sync();
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

		return count ? count : std::basic_streambuf<CharT>::traits_type::eof();
	}

	template<typename CharT, typename container>
	int IOSocketBuffer<CharT, container>::sync()
	{
		switch (type)
		{
		case IOType::input:
		{
			lastPacketSize = network->receiveData(inBuffer);
			setg(inBuffer.data(), inBuffer.data(), inBuffer.data() + lastPacketSize);

			break;
		}

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

			pbump(-size);
		}

		break;
		}

		return 0;
	}

	template<typename CharT, typename container>
	typename IOSocketBuffer<CharT, container>::dataContainer IOSocketBuffer<CharT, container>::dataPart() noexcept
	{
		return dataContainer(pbase(), pptr());
	}

	template<typename CharT, typename container>
	IOSocketBuffer<CharT, container>::IOSocketBuffer(SOCKET clientSocket) : network(new NetworkOperations(clientSocket))
	{
		this->setPointers();
	}

	template<typename CharT, typename container>
	IOSocketBuffer<CharT, container>::IOSocketBuffer(SOCKET clientSocket, size_t bufferSize) : network(new NetworkOperations(clientSocket, NetworkOperations::ReceiveMode::prohibitResize)), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename CharT, typename container>
	IOSocketBuffer<CharT, container>::IOSocketBuffer(const std::string& ip, const std::string& port) : network(new NetworkOperations(ip, port))
	{
		this->setPointers();
	}

	template<typename CharT, typename container>
	IOSocketBuffer<CharT, container>::IOSocketBuffer(const std::string& ip, const std::string& port, size_t bufferSize) : network(new NetworkOperations(ip, port, NetworkOperations::ReceiveMode::prohibitResize)), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename CharT, typename container>
	IOSocketBuffer<CharT, container>::IOSocketBuffer(NetworkOperations* networkSubclass) : network(networkSubclass)
	{
		this->setPointers();
	}

	template<typename CharT, typename container>
	IOSocketBuffer<CharT, container>::IOSocketBuffer(NetworkOperations* networkSubclass, size_t bufferSize) : network(networkSubclass), outBuffer(bufferSize), inBuffer(bufferSize)
	{
		this->setPointers();
	}

	template<typename CharT, typename container>
	void IOSocketBuffer<CharT, container>::setInputType()
	{
		type = IOType::input;
	}

	template<typename CharT, typename container>
	void IOSocketBuffer<CharT, container>::setOutputType()
	{
		type = IOType::output;
	}

	template<typename CharT, typename container>
	__int32 IOSocketBuffer<CharT, container>::getLastPacketSize()
	{
		return lastPacketSize;
	}
	
	template<typename CharT,typename container>
	IOSocketBuffer<CharT, container>::~IOSocketBuffer()
	{
		delete network;
	}
}