#include "IOSocketBuffer.h"

#include <iterator>

namespace buffers
{
	typename IOSocketBuffer::int_type IOSocketBuffer::overflow(int_type ch)
	{
		char data = ch;

		lastPacketSize = network->sendBytes(&data, sizeof(data), endOfStream);

		return endOfStream ? traits_type::eof() : 0;
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::underflow()
	{
		char data;

		lastPacketSize = network->receiveBytes(&data, sizeof(data), endOfStream);

		return endOfStream ? traits_type::eof() : data;
	}

	std::streamsize IOSocketBuffer::xsputn(const char_type* s, std::streamsize count)
	{
		lastPacketSize = network->sendBytes(s, count, endOfStream);
		
		return endOfStream ? traits_type::eof() : lastPacketSize;
	}

	std::streamsize IOSocketBuffer::xsgetn(char_type* s, std::streamsize count)
	{
		lastPacketSize = network->receiveBytes(s, count, endOfStream);

		return endOfStream ? traits_type::eof() : lastPacketSize;
	}

	IOSocketBuffer::IOSocketBuffer(SOCKET clientSocket) :
		network(std::make_unique<web::Network>(clientSocket))
	{
		
	}

	IOSocketBuffer::IOSocketBuffer(std::string_view ip, std::string_view port, DWORD timeout) :
		network(std::make_unique<web::Network>(ip, port, timeout))
	{
		
	}

	IOSocketBuffer::IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass) :
		network(std::move(networkSubclass))
	{
		
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
