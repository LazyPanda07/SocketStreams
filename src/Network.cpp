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

	void Network::setTimeout(int64_t timeout)
	{
#ifdef __LINUX__
		timeval timeoutValue;

		timeoutValue.tv_sec = timeout / 1000;
		timeoutValue.tv_usec = (timeout - timeoutValue.tv_sec * 1000) * 1000;
#else
		DWORD timeoutValue = static_cast<DWORD>(timeout);
#endif

		if (setsockopt(this->getClientSocket(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}

		if (setsockopt(this->getClientSocket(), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}
	}

	Network::Network(std::string_view ip, std::string_view port, int64_t timeout)
	{
		SOCKET tempSocket = INVALID_SOCKET;

#ifndef __LINUX__
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			THROW_WEB_EXCEPTION;
		}
#endif // !__LINUX__

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

		if (connect(tempSocket, info->ai_addr, static_cast<int>(info->ai_addrlen)) == SOCKET_ERROR)
		{
			freeaddrinfo(info);

			THROW_WEB_EXCEPTION;
		}

		handle = std::shared_ptr<SOCKET>(new SOCKET(tempSocket), [](SOCKET* ptr) { closesocket(*ptr); delete ptr; });

		freeaddrinfo(info);

		this->setTimeout(timeout);
	}

	bool Network::isDataAvailable(int* availableBytes, bool* hasConnection) const
	{
		SOCKET socket = this->getClientSocket();

		if (hasConnection)
		{
			char buffer;
			int result = this->callInNonBlockingMode
			(
				std::bind(&Network::receiveBytesImplementation, const_cast<Network*>(this), &buffer, static_cast<int>(sizeof(buffer)), MSG_PEEK)
			);

			if (result > 0)
			{
				*hasConnection = true;
			}
			else if (result < 0)
			{
#ifdef __LINUX__
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					*hasConnection = true;
				}
				else
				{
					if (availableBytes)
					{
						*availableBytes = 0;
					}

					*hasConnection = false;

					return false;
				}
#else
				if (int errorCode = WSAGetLastError(); errorCode == WSAEWOULDBLOCK)
				{
					*hasConnection = true;
				}
				else
				{
					if (availableBytes)
					{
						*availableBytes = 0;
					}

					*hasConnection = false;

					return false;
				}
#endif
			}
			else
			{
				if (availableBytes)
				{
					*availableBytes = 0;
				}

				*hasConnection = false;

				return false;
			}
		}

#ifdef __LINUX__
		int result = 0;

		if (ioctl(socket, FIONREAD, &result) < 0)
		{
			THROW_WEB_EXCEPTION;
		}
#else
		u_long result = 0;

		if (ioctlsocket(socket, FIONREAD, &result) == SOCKET_ERROR)
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
		int size = static_cast<int>(data.size());
		int lastPacketSize = this->sendBytes(&size, sizeof(size), endOfStream, flags);

		if (endOfStream)
		{
			return lastPacketSize;
		}

		return this->sendBytes(data.data(), size, endOfStream, flags);
	}

	int Network::sendRawData(const char* data, int size, bool& endOfStream, int flags)
	{
		int lastPacketSize = this->sendBytes(&size, sizeof(size), endOfStream, flags);

		if (endOfStream)
		{
			return lastPacketSize;
		}

		return this->sendBytes(data, size, endOfStream, flags);
	}

	int Network::receiveData(utility::ContainerWrapper& data, bool& endOfStream, int flags)
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

	int Network::receiveRawData(char* data, int size, bool& endOfStream, int flags)
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

	void Network::addReceiveBuffer(std::string_view buffer)
	{
		buffers.push(buffer);
	}

	SOCKET Network::getClientSocket() const
	{
		if (handle)
		{
			return *handle;
		}

		return INVALID_SOCKET;
	}
}
