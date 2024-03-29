#include "Network.h"

namespace web
{
	Network::Network(std::string_view ip, std::string_view port, DWORD timeout) :
		clientSocket(INVALID_SOCKET)
	{
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

		if (clientSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol); clientSocket == INVALID_SOCKET)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		if (connect(clientSocket, info->ai_addr, static_cast<int>(info->ai_addrlen)) == SOCKET_ERROR)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}

		if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}

		freeaddrinfo(info);
	}

	Network::Network(SOCKET clientSocket) :
		clientSocket(clientSocket)
	{

	}

	int Network::sendData(const std::vector<char>& data, bool& endOfStream)
	{
		try
		{
			int size = static_cast<int>(data.size());

			int lastPacketSize = this->sendBytes(&size, sizeof(size), endOfStream);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			return this->sendBytes(data.data(), size, endOfStream);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	int Network::sendData(std::string_view data, bool& endOfStream)
	{
		try
		{
			int size = static_cast<int>(data.size());

			int lastPacketSize = this->sendBytes(&size, sizeof(size), endOfStream);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			return this->sendBytes(data.data(), size, endOfStream);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	int Network::receiveData(std::vector<char>& data, bool& endOfStream)
	{
		try
		{
			int size = 0;

			int lastPacketSize = this->receiveBytes(&size, sizeof(size), endOfStream);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			if (data.size() < size)
			{
				data.resize(static_cast<size_t>(size));
			}

			return this->receiveBytes(data.data(), size, endOfStream);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	int Network::receiveData(std::string& data, bool& endOfStream)
	{
		try
		{
			int size = 0;

			int lastPacketSize = this->receiveBytes(&size, sizeof(size), endOfStream);

			if (endOfStream)
			{
				return lastPacketSize;
			}

			if (data.size() < size)
			{
				data.resize(static_cast<size_t>(size));
			}

			return this->receiveBytes(data.data(), size, endOfStream);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			throw;
		}
	}

	void Network::log(const std::string& message, std::any&& data)
	{
		std::clog << message;
	}

	SOCKET Network::getClientSocket() const
	{
		return clientSocket;
	}

	Network::~Network()
	{
		closesocket(clientSocket);
	}
}
