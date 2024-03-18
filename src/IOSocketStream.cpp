#include "IOSocketStream.h"

namespace streams
{
	IOSocketStream::IOSocketStream() :
		std::iostream(nullptr)
	{

	}

	IOSocketStream::IOSocketStream(SOCKET clientSocket) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(clientSocket))
	{
		std::iostream::rdbuf(buffer.get());
	}

	IOSocketStream::IOSocketStream(SOCKET clientSocket, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(clientSocket, bufferSize))
	{
		std::iostream::rdbuf(buffer.get());
	}

	IOSocketStream::IOSocketStream(std::string_view ip, std::string_view port, DWORD timeout) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(ip, port, timeout))
	{
		std::iostream::rdbuf(buffer.get());
	}

	IOSocketStream::IOSocketStream(std::string_view ip, std::string_view port, size_t bufferSize, DWORD timeout) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(ip, port, bufferSize, timeout))
	{
		std::iostream::rdbuf(buffer.get());
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

	IOSocketStream::IOSocketStream(std::unique_ptr<web::Network>&& network, size_t bufferSize) :
		std::iostream(nullptr),
		buffer(std::make_unique<buffers::IOSocketBuffer>(std::move(network), bufferSize))
	{
		std::iostream::rdbuf(buffer.get());
	}

	IOSocketStream::IOSocketStream(IOSocketStream&& other) noexcept :
		std::iostream(nullptr),
		buffer(std::move(other.buffer))
	{
		std::iostream::rdbuf(buffer.get());
	}

	IOSocketStream& IOSocketStream::operator = (IOSocketStream&& other) noexcept
	{
		buffer = std::move(other.buffer);

		std::iostream::rdbuf(buffer.get());

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

	std::ostream& IOSocketStream::operator << (const std::vector<char>& data)
	{
		try 
		{
			if (buffer->sputn(data.data(), static_cast<std::streamsize>(data.size())) == -1)
			{
				setstate(std::ios_base::eofbit);
			}
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}
		
		return *this;
	}

	std::istream& IOSocketStream::operator >> (std::vector<char>& data)
	{
		buffer->setInputType();

		try
		{
			if (buffer->pubsync() == -1)
			{
				setstate(std::ios_base::eofbit);

				return *this;
			}

			data.resize(static_cast<size_t>(buffer->getLastPacketSize()));

			if (buffer->sgetn(data.data(), static_cast<std::streamsize>(data.size())) == -1)
			{
				setstate(std::ios_base::eofbit);
			}
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}

		return *this;
	}
	
	std::istream& IOSocketStream::operator >> (std::string& data)
	{
		buffer->setInputType();

		try
		{
			if (buffer->pubsync() == -1)
			{
				setstate(std::ios_base::eofbit);

				return *this;
			}

			data.resize(static_cast<size_t>(buffer->getLastPacketSize()));

			if (buffer->sgetn(data.data(), static_cast<std::streamsize>(data.size())) == -1)
			{
				setstate(std::ios_base::eofbit);
			}
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}

		return *this;
	}

	std::ostream& IOSocketStream::operator << (std::string_view data)
	{
		try
		{
			if (buffer->sputn(data.data(), static_cast<std::streamsize>(data.size())) == -1)
			{
				setstate(std::ios_base::eofbit);
			}
		}
		catch (const web::exceptions::WebException&)
		{
			setstate(std::ios_base::failbit);

			throw;
		}

		return *this;
	}
}
