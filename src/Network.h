#pragma once

#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <string>

#include "CheckAtCompileTime.h"
#include "WebException.h"

#pragma comment (lib,"ws2_32.lib")

namespace web
{
	template<typename CharT, typename ContainerT = std::vector<CharT>>
	class Network
	{
	protected:
		template<typename DataT>
		int_fast32_t sendBytes(const DataT* const data, int_fast32_t count);

		template<typename DataT>
		int_fast32_t receiveBytes(DataT* const data, int_fast32_t count);

		virtual void log(const char* message) = 0;

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
		virtual int_fast32_t sendData(const dataContainer& data);

		//return total number of bytes
		virtual int_fast32_t sendData(const std::basic_string_view<CharT>& data);

		//return total number of bytes
		virtual int_fast32_t receiveData(dataContainer& data);

		//return total number of bytes
		virtual int_fast32_t receiveData(std::basic_string<CharT>& data);

		template<typename Resizable>
		static void resizeFunction(Resizable& data, size_t newSize);

		virtual void setReceiveMode(ReceiveMode mode = ReceiveMode::allowResize) final;

		virtual ReceiveMode getResizeMode() final;

		virtual ~Network() = default;
	};

	template<typename CharT, typename ContainerT>
	template<typename DataT>
	int_fast32_t Network<CharT, ContainerT>::sendBytes(const DataT* const data, int_fast32_t count)
	{
		int_fast32_t  lastSend = 0;
		int_fast32_t totalSend = 0;

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
	int_fast32_t Network<CharT, ContainerT>::receiveBytes(DataT* const data, int_fast32_t count)
	{
		int_fast32_t lastReceive = 0;
		int_fast32_t totalReceive = 0;

		do
		{
			lastReceive = recv(clientSocket, reinterpret_cast<char*>(data) + totalReceive, count - totalReceive, NULL);

			if (lastReceive == SOCKET_ERROR)
			{
				throw WebException();
			}
			else if (!lastReceive)
			{
				return totalReceive;
			}

			totalReceive += lastReceive;

		} while (totalReceive < count);

		return totalReceive;
	}

	template<typename CharT, typename ContainerT>
	template<typename Resizable>
	void Network<CharT, ContainerT>::resizeFunction(Resizable& data, size_t newSize)
	{
		data.resize(newSize);
	}

	template<typename CharT, typename ContainerT>
	template<typename FirstStringT, typename SecondStringT>
	Network<CharT, ContainerT>::Network(const FirstStringT& ip, const SecondStringT& port, ReceiveMode mode) : mode(mode)
	{
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			std::cout << "Error in WSAStartup ";
#ifdef _DEBUG
			std::cout << " on line " << __LINE__ << " in file " << __FILE__ << " ";
#endif // _DEBUG
			std::cout << "WSAGetLastError: " << WSAGetLastError() << std::endl;
			return;
		}


		addrinfo* info = nullptr;
		addrinfo hints = {};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(ip.data(), port.data(), &hints, &info))
		{
			std::cout << "Error in getaddrinfo ";
#ifdef _DEBUG
			std::cout << " on line " << __LINE__ << " in file " << __FILE__ << " ";
#endif // _DEBUG
			std::cout << "WSAGetLastError: " << WSAGetLastError() << std::endl;
			return;
		}

		clientSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "Error in socket function ";
#ifdef _DEBUG
			std::cout << " on line " << __LINE__ << " in file " << __FILE__ << " ";
#endif // _DEBUG
			std::cout << "WSAGetLastError: " << WSAGetLastError() << std::endl;
			return;
		}

		if (connect(clientSocket, info->ai_addr, info->ai_addrlen))
		{
			std::cout << "Error in connect ";
#ifdef _DEBUG
			std::cout << " on line " << __LINE__ << " in file " << __FILE__ << " ";
#endif // _DEBUG
			std::cout << "WSAGetLastError: " << WSAGetLastError() << std::endl;
			return;
		}

		freeaddrinfo(info);
	}

	template<typename CharT, typename ContainerT>
	Network<CharT, ContainerT>::Network(SOCKET clientSocket, ReceiveMode mode) : clientSocket(clientSocket), mode(ReceiveMode::allowResize)
	{

	}

	template<typename CharT, typename ContainerT>
	int_fast32_t Network<CharT, ContainerT>::sendData(const dataContainer& data)
	{
		static_assert(utility::checkSize<dataContainer>::value, "Your dataContainer hasn't size method");
		static_assert(utility::checkData<dataContainer>::value, "Your dataContainer hasn't data method");

		try
		{
			const int_fast32_t size = data.size();

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
	int_fast32_t Network<CharT, ContainerT>::sendData(const std::basic_string_view<CharT>& data)
	{
		try
		{
			const int_fast32_t size = data.size();

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
	int_fast32_t Network<CharT, ContainerT>::receiveData(dataContainer& data)
	{
		static_assert(utility::checkData<dataContainer>::value, "Your dataContainer hasn't data method");

		try
		{
			int_fast32_t size;

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
	int_fast32_t Network<CharT, ContainerT>::receiveData(std::basic_string<CharT>& data)
	{
		try
		{
			int_fast32_t size;

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
}