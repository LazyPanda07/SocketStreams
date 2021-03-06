#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <any>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "CheckAtCompileTime.h"
#include "WebException.h"

#pragma comment (lib,"ws2_32.lib")

namespace web
{
	template<typename ContainerT = std::vector<char>>
	class BaseNetwork
	{
	public:
		enum class ReceiveMode 
		{
			allowResize,
			prohibitResize
		};

	protected:
		SOCKET clientSocket;
		ReceiveMode mode;

	public:
		template<typename FirstStringT, typename SecondStringT>
		BaseNetwork(const FirstStringT& ip, const SecondStringT& port, ReceiveMode mode = ReceiveMode::allowResize);

		BaseNetwork(SOCKET clientSocket, ReceiveMode mode = ReceiveMode::allowResize);

		//return total number of bytes
		virtual int sendData(const ContainerT& data);

		//return total number of bytes
		virtual int sendData(const std::string_view& data);

		//return total number of bytes
		virtual int receiveData(ContainerT& data);

		//return total number of bytes
		virtual int receiveData(std::string& data);

		template<typename Resizable>
		static void resizeFunction(Resizable& data, size_t newSize);

		virtual void setReceiveMode(ReceiveMode mode = ReceiveMode::allowResize) final;

		virtual ReceiveMode getResizeMode() final;

		template<typename DataT>
		int sendBytes(const DataT* const data, int count);

		template<typename DataT>
		int receiveBytes(DataT* const data, int count);

		//default implementation uses clog
		virtual void log(const std::string& message, std::any&& data = "");

		virtual ~BaseNetwork();
	};

	template<typename ContainerT>
	template<typename Resizable>
	void BaseNetwork<ContainerT>::resizeFunction(Resizable& data, size_t newSize)
	{
		data.resize(newSize);
	}

	template<typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	BaseNetwork<ContainerT>::BaseNetwork(const FirstStringT& ip, const SecondStringT& port, ReceiveMode mode) :
		mode(mode),
		clientSocket(INVALID_SOCKET)
	{
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			throw exceptions::WebException();
		}

		addrinfo* info = nullptr;
		addrinfo hints = {};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(ip.data(), port.data(), &hints, &info))
		{
			throw exceptions::WebException();
		}

		clientSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		if (clientSocket == INVALID_SOCKET)
		{
			freeaddrinfo(info);
			throw exceptions::WebException();
		}

		if (connect(clientSocket, info->ai_addr, info->ai_addrlen))
		{
			freeaddrinfo(info);
			throw exceptions::WebException();
		}

		freeaddrinfo(info);
	}

	template<typename ContainerT>
	BaseNetwork<ContainerT>::BaseNetwork(SOCKET clientSocket, ReceiveMode mode) : clientSocket(clientSocket), mode(ReceiveMode::allowResize)
	{

	}

	template<typename ContainerT>
	int BaseNetwork<ContainerT>::sendData(const ContainerT& data)
	{
		static_assert(utility::checkSize<ContainerT>::value, "Your ContainerT hasn't size method");
		static_assert(utility::checkData<ContainerT>::value, "Your ContainerT hasn't data method");

		try
		{
			const int size = data.size();

			this->sendBytes(&size, sizeof(size));

			return this->sendBytes(data.data(), size);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename ContainerT>
	int BaseNetwork<ContainerT>::sendData(const std::string_view& data)
	{
		try
		{
			const int size = data.size();

			this->sendBytes(&size, sizeof(size));

			return this->sendBytes(data.data(), size);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename ContainerT>
	int BaseNetwork<ContainerT>::receiveData(ContainerT& data)
	{
		static_assert(utility::checkData<ContainerT>::value, "Your ContainerT hasn't data method");

		try
		{
			int size;

			this->receiveBytes(&size, sizeof(size));

			if constexpr (utility::checkResize<ContainerT>::value)
			{
				if (mode == ReceiveMode::allowResize)
				{
					this->resizeFunction(data, size);
				}
			}

			return this->receiveBytes(data.data(), size);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename ContainerT>
	int BaseNetwork<ContainerT>::receiveData(std::string& data)
	{
		try
		{
			int size;

			this->receiveBytes(&size, sizeof(size));

			if (mode == ReceiveMode::allowResize)
			{
				this->resizeFunction(data, size);
			}

			return this->receiveBytes(data.data(), size);
		}
		catch (const exceptions::WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename ContainerT>
	void BaseNetwork<ContainerT>::setReceiveMode(BaseNetwork<ContainerT>::ReceiveMode mode)
	{
		this->mode = mode;
	}

	template<typename ContainerT>
	typename BaseNetwork<ContainerT>::ReceiveMode BaseNetwork<ContainerT>::getResizeMode()
	{
		return mode;
	}

	template<typename ContainerT>
	template<typename DataT>
	int BaseNetwork<ContainerT>::sendBytes(const DataT* const data, int count)
	{
		int  lastSend = 0;
		int totalSent = 0;

		do
		{
			lastSend = send(clientSocket, reinterpret_cast<const char*>(data) + totalSent, count - totalSent, NULL);

			if (lastSend == SOCKET_ERROR)
			{
				throw exceptions::WebException();
			}

			totalSent += lastSend;

		} while (totalSent < count);

		return totalSent;
	}

	template<typename ContainerT>
	template<typename DataT>
	int BaseNetwork<ContainerT>::receiveBytes(DataT* const data, int count)
	{
		int lastReceive = 0;
		int totalReceive = 0;

		do
		{
			lastReceive = recv(clientSocket, reinterpret_cast<char*>(data) + totalReceive, count - totalReceive, NULL);

			if (lastReceive == SOCKET_ERROR || !lastReceive)
			{
				throw exceptions::WebException();
			}

			totalReceive += lastReceive;

		} while (totalReceive < count);

		return totalReceive;
	}

	template<typename ContainerT>
	void BaseNetwork<ContainerT>::log(const std::string& message, std::any&& data)
	{
		std::clog << message;
	}

	template<typename ContainerT>
	BaseNetwork<ContainerT>::~BaseNetwork()
	{
		closesocket(clientSocket);
	}

	using Network = BaseNetwork<std::vector<char>>;
}
