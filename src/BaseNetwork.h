#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <any>

#ifdef __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif // __LINUX__

#include "CheckAtCompileTime.h"
#include "WebException.h"

#ifndef __LINUX__
#pragma comment (lib, "ws2_32.lib")
#endif // !__LINUX__

#ifdef __LINUX__
#define closesocket close
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define DWORD uint32_t
#endif // __LINUX__

namespace web
{
	/// @brief Base network class
	/// @tparam ContainerT 
	template<typename ContainerT = std::vector<char>>
	class BaseNetwork
	{
	public:
		enum class receiveMode
		{
			allowResize,
			prohibitResize
		};

	protected:
		SOCKET clientSocket;
		receiveMode mode;

	public:
		template<typename Resizable>
		static void resizeFunction(Resizable& data, size_t newSize);

	public:
		/// @brief Client side constructor
		/// @tparam HostStringT 
		/// @tparam PortStringT 
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		/// @exception WebException 
		template<typename HostStringT, typename PortStringT>
		BaseNetwork(const HostStringT& ip, const PortStringT& port, DWORD timeout = 30'000, receiveMode mode = receiveMode::allowResize);

		/// @brief Server side contructor
		/// @param clientSocket 
		/// @param mode 
		BaseNetwork(SOCKET clientSocket, receiveMode mode = receiveMode::allowResize);

		/// @brief Send data through network
		/// @param data 
		/// @return Total number of sended bytes 
		virtual int sendData(const ContainerT& data);

		/// @brief Send data through network
		/// @param data 
		/// @return Total number of sended bytes 
		virtual int sendData(std::string_view data);

		/// @brief Receive data through network
		/// @param data 
		/// @return Total number of received bytes 
		virtual int receiveData(ContainerT& data);

		/// @brief Receive data through network
		/// @param data 
		/// @return Total number of received bytes 
		virtual int receiveData(std::string& data);

		/// @brief Set currrent receive mode
		/// @param mode New receive mode
		void setReceiveMode(receiveMode mode = receiveMode::allowResize);

		/// @brief clientSocket getter
		/// @return clientSocket
		SOCKET getClientSocket() const;

		/// @brief Get current receive mode
		/// @return mode
		receiveMode getResizeMode();

		/// @brief Send raw bytes through network
		/// @tparam DataT 
		/// @param data 
		/// @param count 
		/// @return Total number of sended bytes 
		/// @exception WebException  
		template<typename DataT>
		int sendBytes(const DataT* const data, int count);

		/// @brief 
		/// @tparam DataT 
		/// @param data 
		/// @param count 
		/// @return Total number of received bytes 
		/// @exception WebException  
		template<typename DataT>
		int receiveBytes(DataT* const data, int count);

		/// @brief Errors logging, default implementation uses clog
		/// @param message Log message
		/// @param data Additional data
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
	template<typename HostStringT, typename PortStringT>
	BaseNetwork<ContainerT>::BaseNetwork(const HostStringT& ip, const PortStringT& port, DWORD timeout, receiveMode mode) :
		mode(mode),
		clientSocket(INVALID_SOCKET)
	{
#ifdef __LINUX__
		timeval timeoutValue;

		timeoutValue.tv_usec = timeout * 1000;
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

		if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}

		if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeoutValue), sizeof(timeoutValue)) == SOCKET_ERROR)
		{
			THROW_WEB_EXCEPTION;
		}

		freeaddrinfo(info);
	}

	template<typename ContainerT>
	BaseNetwork<ContainerT>::BaseNetwork(SOCKET clientSocket, receiveMode mode) :
		clientSocket(clientSocket),
		mode(mode)
	{

	}

	template<typename ContainerT>
	int BaseNetwork<ContainerT>::sendData(const ContainerT& data)
	{
		static_assert(utility::checkSize<ContainerT>::value, "Your ContainerT hasn't size method");
		static_assert(utility::checkData<ContainerT>::value, "Your ContainerT hasn't data method");

		try
		{
			int size = static_cast<int>(data.size());

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
	int BaseNetwork<ContainerT>::sendData(std::string_view data)
	{
		try
		{
			int size = static_cast<int>(data.size());

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
			int size = 0;

			this->receiveBytes(&size, sizeof(size));

			if constexpr (utility::checkResize<ContainerT>::value)
			{
				if (mode == receiveMode::allowResize)
				{
					this->resizeFunction(data, static_cast<size_t>(size));
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
			int size = 0;

			this->receiveBytes(&size, sizeof(size));

			if (mode == receiveMode::allowResize)
			{
				this->resizeFunction(data, static_cast<size_t>(size));
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
	void BaseNetwork<ContainerT>::setReceiveMode(BaseNetwork<ContainerT>::receiveMode mode)
	{
		this->mode = mode;
	}

	template<typename ContainerT>
	SOCKET BaseNetwork<ContainerT>::getClientSocket() const
	{
		return clientSocket;
	}

	template<typename ContainerT>
	typename BaseNetwork<ContainerT>::receiveMode BaseNetwork<ContainerT>::getResizeMode()
	{
		return mode;
	}

	template<typename ContainerT>
	template<typename DataT>
	int BaseNetwork<ContainerT>::sendBytes(const DataT* const data, int count)
	{
		int lastSend = 0;
		int totalSent = 0;

		do
		{
			lastSend = send(clientSocket, reinterpret_cast<const char*>(data) + totalSent, count - totalSent, NULL);

			if (lastSend == SOCKET_ERROR)
			{
				THROW_WEB_EXCEPTION;
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
				THROW_WEB_EXCEPTION;
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
