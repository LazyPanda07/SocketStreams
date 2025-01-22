#include "IOSocketStream.h"

namespace streams
{
	int IOSocketStream::sendFundamentalImplementation(const char* value, int valueSize, bool& endOfStream)
	{	
		return buffer->getNetwork()->sendBytes(value, valueSize, endOfStream);
	}

	int IOSocketStream::receiveFundamentalImplementation(char* value, int valueSize, bool& endOfStream)
	{
		return buffer->getNetwork()->receiveBytes(value, valueSize, endOfStream);
	}

	IOSocketStream::IOSocketStream(std::unique_ptr<buffers::IOSocketBuffer>&& buffer) :
		std::iostream(nullptr),
		buffer(std::move(buffer))
	{
		std::iostream::rdbuf(this->buffer.get());
	}

	IOSocketStream::IOSocketStream(std::unique_ptr<web::Network>&& network) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(std::move(network)))
	{
		std::iostream::rdbuf(buffer.get());
	}

	IOSocketStream::IOSocketStream(IOSocketStream&& other) noexcept :
		std::iostream(nullptr),
		buffer(std::move(other.buffer))
	{
		std::iostream::rdbuf(buffer.get());

		other.rdbuf(nullptr);
	}

	IOSocketStream& IOSocketStream::operator = (IOSocketStream&& other) noexcept
	{
		buffer = std::move(other.buffer);

		std::iostream::rdbuf(buffer.get());

		other.rdbuf(nullptr);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (bool value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (short value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (int value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (long value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (long long value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (unsigned short value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (unsigned int value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (unsigned long value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (unsigned long long value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (float value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (double value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::ostream& IOSocketStream::operator << (long double value)
	{
		this->sendFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (bool& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (short& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (int& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (long& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (long long& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (unsigned short& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (unsigned int& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (unsigned long& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (unsigned long long& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (float& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (double& value)
	{
		this->receiveFundamental(value);

		return *this;
	}

	std::istream& IOSocketStream::operator >> (long double& value)
	{
		this->receiveFundamental(value);

		return *this;
	}
}
