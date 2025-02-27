#include "IOSocketBuffer.h"

#include <iterator>
#include <limits>
#include <algorithm>
#include <chrono>
#include <thread>

namespace buffers
{
	size_t IOSocketBuffer::getAvailableInputSize() const
	{
		return gptr() ? egptr() - gptr() : 0;
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::overflow(int_type ch)
	{
		char character = ch;

		lastPacketSize = network->sendBytes(&character, sizeof(character), endOfStream);

		if (endOfStream)
		{
			return traits_type::eof();
		}

		return traits_type::eof() + 1;
	}

	typename IOSocketBuffer::int_type IOSocketBuffer::underflow()
	{
		web::utility::ContainerWrapper container(inputData);

		if (gptr())
		{
			setg(nullptr, nullptr, nullptr);

			return traits_type::eof();
		}

		lastPacketSize = network->receiveData(container, endOfStream);

		if (endOfStream)
		{
			return traits_type::eof();
		}

		setg(inputData.data(), inputData.data(), inputData.data() + static_cast<size_t>(lastPacketSize) + 1);

		return *gptr();
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
		if (size_t bufferSize = this->getAvailableInputSize(); bufferSize)
		{
			network->addReceiveBuffer(inputData.data() + bufferSize);

			gbump(std::min<int>(static_cast<int>(bufferSize), static_cast<int>(size)));
		}

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
		endOfStream(false)
	{

	}

	IOSocketBuffer::IOSocketBuffer(std::string_view ip, std::string_view port, DWORD timeout) :
		network(std::make_unique<web::Network>(ip, port, timeout)),
		lastPacketSize(0),
		endOfStream(false)
	{

	}

	IOSocketBuffer::IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass) :
		network(std::move(networkSubclass)),
		lastPacketSize(0),
		endOfStream(false)
	{

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
