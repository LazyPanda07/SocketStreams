#include "IOSocketBuffer.h"

#include <iterator>

namespace buffers
{
	void IOSocketBuffer::setPointers()
	{
		setp(outBuffer.data(), outBuffer.data() + outBuffer.size() - 1);
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::overflow(int_type ch)
	{
		*pptr() = ch;

		pbump(1);

		this->setOutputType();

		if (this->sync() == -1)
		{
			return traits_type::eof();
		}

		return 0;
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::underflow()
	{
		this->setInputType();

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

	std::streamsize IOSocketBuffer::xsputn(const char_type* s, std::streamsize count)
	{
		this->setOutputType();

		if (static_cast<std::streamsize>(outBuffer.size()) < count)
		{
			outBuffer.resize(static_cast<size_t>(count));

			this->setPointers();
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

	std::streamsize IOSocketBuffer::xsgetn(char_type* s, std::streamsize count)
	{
		this->setInputType();

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

	int IOSocketBuffer::sync()
	{
		switch (type)
		{
		case IOType::input:
			lastPacketSize = network->receiveData(inBuffer, endOfStream);

			if (endOfStream)
			{
				return -1;
			}

			setg(inBuffer.data(), inBuffer.data(), inBuffer.data() + lastPacketSize);

			break;

		case IOType::output:
			int size = static_cast<int>(std::distance(pbase(), pptr()));

			lastPacketSize = network->sendBytes(pbase(), size, endOfStream);

			pbump(-size);

			if (endOfStream)
			{
				return -1;
			}

			break;
		}

		return 0;
	}

	IOSocketBuffer::IOSocketBuffer(IOSocketBuffer&& other) noexcept :
		outBuffer(std::move(other.outBuffer)),
		inBuffer(std::move(other.inBuffer)),
		network(std::move(other.network)),
		lastPacketSize(other.lastPacketSize),
		type(other.type),
		endOfStream(other.endOfStream)
	{

	}

	IOSocketBuffer::IOSocketBuffer(SOCKET clientSocket) :
		network(std::make_unique<web::Network>(clientSocket))
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(SOCKET clientSocket, size_t bufferSize) :
		network(std::make_unique<web::Network>(clientSocket)),
		outBuffer(bufferSize),
		inBuffer(bufferSize)
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(std::string_view ip, std::string_view port, DWORD timeout) :
		network(std::make_unique<web::Network>(ip, port, timeout))
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(std::string_view ip, std::string_view port, size_t bufferSize, DWORD timeout) :
		network(std::make_unique<web::Network>(ip, port, timeout)),
		outBuffer(bufferSize),
		inBuffer(bufferSize)
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass) :
		network(std::move(networkSubclass))
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass, size_t bufferSize) :
		network(std::move(networkSubclass)),
		outBuffer(bufferSize),
		inBuffer(bufferSize)
	{
		this->setPointers();
	}

	IOSocketBuffer& IOSocketBuffer::operator = (IOSocketBuffer&& other) noexcept
	{
		outBuffer = std::move(other.outBuffer);
		inBuffer = std::move(other.inBuffer);
		network = std::move(other.network);
		lastPacketSize = other.lastPacketSize;
		type = other.type;

		return *this;
	}

	void IOSocketBuffer::setInputType() noexcept
	{
		type = IOType::input;
	}

	void IOSocketBuffer::setOutputType() noexcept
	{
		type = IOType::output;
	}

	const std::unique_ptr<web::Network>& IOSocketBuffer::getNetwork() const
	{
		return network;
	}

	std::unique_ptr<web::Network>& IOSocketBuffer::getNetwork()
	{
		return network;
	}

	int IOSocketBuffer::getLastPacketSize() const noexcept
	{
		return lastPacketSize;
	}

	bool IOSocketBuffer::getEndOfStream() const noexcept
	{
		return endOfStream;
	}
}
