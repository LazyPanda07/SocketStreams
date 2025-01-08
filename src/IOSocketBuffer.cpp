#include "IOSocketBuffer.h"

#include <iterator>
#include <limits>

namespace buffers
{
	void IOSocketBuffer::setPointers()
	{
		setg(&lastInputCharacter, &lastInputCharacter, nullptr);
		setp(&lastOutputCharacter, nullptr);
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::overflow(int_type ch)
	{
		lastOutputCharacter = ch;

		lastPacketSize = network->sendBytes(&lastOutputCharacter, sizeof(lastOutputCharacter), endOfStream);

		if (endOfStream)
		{
			lastOutputCharacter = traits_type::eof();
		}
		else
		{
			lastOutputCharacter = traits_type::eof() + 1;

			setp(&lastOutputCharacter, nullptr);
		}

		return lastOutputCharacter;
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::underflow()
	{
		lastPacketSize = network->receiveBytes(&lastInputCharacter, sizeof(lastInputCharacter), endOfStream);

		if (endOfStream)
		{
			lastInputCharacter = traits_type::eof();
		}
		else
		{
			setg(&lastInputCharacter, &lastInputCharacter, nullptr);
		}

		return lastInputCharacter;
	}

	std::streamsize IOSocketBuffer::xsputn(const char_type* s, std::streamsize size)
	{
		if (size == (std::numeric_limits<std::streamsize>::max)())
		{
			const web::utility::ContainerWrapper& container = *(reinterpret_cast<const web::utility::ContainerWrapper*>(s));

			lastPacketSize = network->sendData(container, endOfStream);
		}
		else
		{
			lastPacketSize = network->sendRawData(s, static_cast<int>(size), endOfStream);
		}

		return endOfStream ? traits_type::eof() : lastPacketSize;
	}

	std::streamsize IOSocketBuffer::xsgetn(char_type* s, std::streamsize size)
	{
		if (size == (std::numeric_limits<std::streamsize>::max)())
		{
			web::utility::ContainerWrapper& container = *(reinterpret_cast<web::utility::ContainerWrapper*>(s));

			lastPacketSize = network->receiveData(container, endOfStream);
		}
		else
		{
			lastPacketSize = network->receiveRawData(s, static_cast<int>(size), endOfStream);
		}

		return endOfStream ? traits_type::eof() : lastPacketSize;
	}

	IOSocketBuffer::IOSocketBuffer(SOCKET clientSocket) :
		network(std::make_unique<web::Network>(clientSocket)),
		lastPacketSize(0),
		lastInputCharacter('\0'),
		lastOutputCharacter('\0'),
		endOfStream(false)
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(std::string_view ip, std::string_view port, DWORD timeout) :
		network(std::make_unique<web::Network>(ip, port, timeout)),
		lastPacketSize(0),
		lastInputCharacter('\0'),
		lastOutputCharacter('\0'),
		endOfStream(false)
	{
		this->setPointers();
	}

	IOSocketBuffer::IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass) :
		network(std::move(networkSubclass)),
		lastPacketSize(0),
		lastInputCharacter('\0'),
		lastOutputCharacter('\0'),
		endOfStream(false)
	{
		this->setPointers();
	}

	const std::unique_ptr<web::Network>& IOSocketBuffer::getNetwork() const noexcept
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
