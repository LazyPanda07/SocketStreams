#include "Network.h"

namespace web
{
	int Network::sendBytesImplementation(const char* data, int size, int flags)
	{
		return send(this->getClientSocket(), data, size, flags);
	}

	int Network::receiveBytesImplementation(char* data, int size, int flags)
	{
		return recv(this->getClientSocket(), data, size, flags);
	}

	void Network::throwException(int line, std::string_view file) const
	{
		throw exceptions::WebException(line, file);
	}

	Network::Network(std::string_view ip, std::string_view port, DWORD timeout) :
		clientSocket(nullptr)
	{
		SOCKET tempSocket = INVALID_SOCKET;

#ifdef __LINUX__
		timeval timeoutValue;

		timeoutValue.tv_sec = timeout / 1000;
		timeoutValue.tv_usec = (timeout - timeoutValue.tv_sec * 1000) * 1000;
#else
		WSADATA wsaData;
		DWORD timeoutValue = timeout;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			THROW_WEB_EXCEPTION;
		}
#endif // __LINUX__

		addrinfo* info = nullptr;
		addrinfo hints = {};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(ip.data(), port.data(), &hints, &info))
		{
			THROW_WEB_EXCEPTION;
		}

		if (tempSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol); tempSocket == INVALID_SOCKET)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		if (setsockopt(tempSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		if (setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		if (connect(tempSocket, info->ai_addr, static_cast<int>(info->ai_addrlen)) == SOCKET_ERROR)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		clientSocket = std::shared_ptr<SOCKET>(new SOCKET(tempSocket), [](SOCKET* ptr) { closesocket(*ptr); delete ptr; });

		freeaddrinfo(info);
	}

	Network::Network(SOCKET clientSocket) :
		clientSocket(new SOCKET(clientSocket), [](SOCKET* ptr) { closesocket(*ptr); delete ptr; })
	{

	}

	bool Network::isDataAvailable(int* availableBytes) const
	{
#ifdef __LINUX__
		int result = 0;

		if (ioctl(this->getClientSocket(), FIONREAD, &result) < 0)
		{
			THROW_WEB_EXCEPTION;
		}
#else
		u_long result = 0;

		if (ioctlsocket(this->getClientSocket(), FIONREAD, &result) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}
#endif

		if (availableBytes)
		{
			*availableBytes = static_cast<int>(result);
		}

		return result > 0;
	}

	int Network::sendData(const utility::ContainerWrapper& data, bool& endOfStream, int flags)
	{
		try
		{
			int size = static_cast<int>(data.size());
			int lastPacketSize = this->sendBytes(&size, sizeof(size), endOfStream, flags);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			return this->sendBytes(data.data(), size, endOfStream, flags);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	int Network::sendRawData(const char* data, int size, bool& endOfStream, int flags)
	{
		try
		{
			int lastPacketSize = this->sendBytes(&size, sizeof(size), endOfStream, flags);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			return this->sendBytes(data, size, endOfStream, flags);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	int Network::receiveData(utility::ContainerWrapper& data, bool& endOfStream, int flags)
	{
		try
		{
			int size = 0;
			int lastPacketSize = this->receiveBytes(&size, sizeof(size), endOfStream, flags);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			if (data.size() < size)
			{
				data.resize(static_cast<size_t>(size));
			}

			return this->receiveBytes(data.data(), size, endOfStream, flags);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	int Network::receiveRawData(char* data, int size, bool& endOfStream, int flags)
	{
		try
		{
			int inputSize = 0;
			int lastPacketSize = this->receiveBytes(&inputSize, sizeof(inputSize), endOfStream, flags);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			if (size < inputSize)
			{
				std::cerr << "In " << __FUNCTION__ << " passed size(" << size << ") < actual data size(" << inputSize << ')' << std::endl;
			}

			return this->receiveBytes(data, size, endOfStream, flags);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	void Network::addReceiveBuffer(std::string_view buffer)
	{
		buffers.push(buffer);
	}

	void Network::log(const std::string& message, std::any&& data)
	{
		std::clog << message;
	}

	SOCKET Network::getClientSocket() const
	{
		if (clientSocket)
		{
			return *clientSocket;
		}

		return INVALID_SOCKET;
	}
}
