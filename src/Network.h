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
	template<typename CharT, typename ContainerT = std::vector<CharT>>
	class Network
	{
	public:
		using parent = typename Network<CharT, ContainerT>;
		using dataContainer = typename ContainerT;

	public:
		enum class ReceiveMode : uint_fast8_t
		{
			allowResize,
			prohibitResize
		};

	protected:
		SOCKET clientSocket;
		ReceiveMode mode;

	public:
		template<typename FirstStringT, typename SecondStringT>
		Network(const FirstStringT& ip, const SecondStringT& port, ReceiveMode mode = ReceiveMode::allowResize);

		Network(SOCKET clientSocket, ReceiveMode mode = ReceiveMode::allowResize);

		//return total number of bytes
		virtual int sendData(const dataContainer& data);

		//return total number of bytes
		virtual int sendData(const std::basic_string_view<CharT>& data);

		//return total number of bytes
		virtual int receiveData(dataContainer& data);

		//return total number of bytes
		virtual int receiveData(std::basic_string<CharT>& data);

		template<typename Resizable>
		static void resizeFunction(Resizable& data, size_t newSize);

		virtual void setReceiveMode(ReceiveMode mode = ReceiveMode::allowResize) final;

		virtual ReceiveMode getResizeMode() final;

		template<typename DataT>
		int sendBytes(const DataT* const data, int count);

		template<typename DataT>
		int receiveBytes(DataT* const data, int count);

		virtual void log(std::string&& message, std::any&& data = "") noexcept = 0;

		virtual ~Network();
	};

	template<typename CharT, typename ContainerT>
	template<typename Resizable>
	void Network<CharT, ContainerT>::resizeFunction(Resizable& data, size_t newSize)
	{
		data.resize(newSize);
	}

	template<typename CharT, typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	Network<CharT, ContainerT>::Network(const FirstStringT& ip, const SecondStringT& port, ReceiveMode mode) :
		mode(mode),
		clientSocket(INVALID_SOCKET)
	{
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			throw WebException();
		}


		addrinfo* info = nullptr;
		addrinfo hints = {};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(ip.data(), port.data(), &hints, &info))
		{
			throw WebException();
		}

		clientSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		if (clientSocket == INVALID_SOCKET)
		{
			freeaddrinfo(info);
			throw WebException();
		}

		if (connect(clientSocket, info->ai_addr, info->ai_addrlen))
		{
			freeaddrinfo(info);
			throw WebException();
		}

		freeaddrinfo(info);
	}

	template<typename CharT, typename ContainerT>
	Network<CharT, ContainerT>::Network(SOCKET clientSocket, ReceiveMode mode) : clientSocket(clientSocket), mode(ReceiveMode::allowResize)
	{

	}

	template<typename CharT, typename ContainerT>
	int Network<CharT, ContainerT>::sendData(const dataContainer& data)
	{
		static_assert(utility::checkSize<dataContainer>::value, "Your dataContainer hasn't size method");
		static_assert(utility::checkData<dataContainer>::value, "Your dataContainer hasn't data method");

		try
		{
			const int size = data.size();

			this->sendBytes(&size, sizeof(size));

			return this->sendBytes(data.data(), size);
		}
		catch (const WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename CharT, typename ContainerT>
	int Network<CharT, ContainerT>::sendData(const std::basic_string_view<CharT>& data)
	{
		try
		{
			const int size = data.size();

			this->sendBytes(&size, sizeof(size));

			return this->sendBytes(data.data(), size);
		}
		catch (const WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename CharT, typename ContainerT>
	int Network<CharT, ContainerT>::receiveData(dataContainer& data)
	{
		static_assert(utility::checkData<dataContainer>::value, "Your dataContainer hasn't data method");

		try
		{
			int size;

			this->receiveBytes(&size, sizeof(size));

			if constexpr (utility::checkResize<dataContainer>::value)
			{
				if (mode == ReceiveMode::allowResize)
				{
					this->resizeFunction(data, size);
				}
			}

			return this->receiveBytes(data.data(), size);
		}
		catch (const WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename CharT, typename ContainerT>
	int Network<CharT, ContainerT>::receiveData(std::basic_string<CharT>& data)
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
		catch (const WebException& e)
		{
			this->log(e.what());

			return -1;
		}
	}

	template<typename CharT, typename ContainerT>
	void Network<CharT, ContainerT>::setReceiveMode(Network<CharT, ContainerT>::ReceiveMode mode)
	{
		this->mode = mode;
	}

	template<typename CharT, typename ContainerT>
	typename Network<CharT, ContainerT>::ReceiveMode Network<CharT, ContainerT>::getResizeMode()
	{
		return mode;
	}

	template<typename CharT, typename ContainerT>
	template<typename DataT>
	int Network<CharT, ContainerT>::sendBytes(const DataT* const data, int count)
	{
		int  lastSend = 0;
		int totalSend = 0;

		do
		{
			lastSend = send(clientSocket, reinterpret_cast<const char*>(data) + totalSend, count - totalSend, NULL);

			if (lastSend == SOCKET_ERROR)
			{
				throw WebException();
			}

			totalSend += lastSend;

		} while (totalSend < count);

		return totalSend;
	}

	template<typename CharT, typename ContainerT>
	template<typename DataT>
	int Network<CharT, ContainerT>::receiveBytes(DataT* const data, int count)
	{
		int lastReceive = 0;
		int totalReceive = 0;

		do
		{
			lastReceive = recv(clientSocket, reinterpret_cast<char*>(data) + totalReceive, count - totalReceive, NULL);

			if (lastReceive == SOCKET_ERROR || !lastReceive)
			{
				throw WebException();
			}

			totalReceive += lastReceive;

		} while (totalReceive < count);

		return totalReceive;
	}

	template<typename CharT, typename ContainerT>
	Network<CharT, ContainerT>::~Network()
	{
		closesocket(clientSocket);
	}
}