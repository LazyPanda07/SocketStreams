#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <string>

#include "CheckAtCompileTime.h"

#pragma comment (lib,"ws2_32.lib")

namespace web
{
	template<typename CharT, typename container = std::vector<CharT>>
	class Network
	{
	public:
		using parent = typename Network<CharT, container>;
		using dataContainer = typename container;

	public:
		enum class ReceiveMode : char
		{
			allowResize,
			prohibitResize
		};

	protected:
		SOCKET clientSocket;
		ReceiveMode mode;

	public:
		Network(const std::string& ip, const std::string& port, ReceiveMode mode = ReceiveMode::allowResize);

		Network(SOCKET clientSocket, ReceiveMode mode = ReceiveMode::allowResize);

		//return total number of bytes
		virtual __int32 sendData(const dataContainer& data);

		//return total number of bytes
		virtual __int32 sendData(const std::basic_string<CharT>& data);

		//return total number of bytes
		virtual __int32 receiveData(dataContainer& data);

		//return total number of bytes
		virtual __int32 receiveData(std::basic_string<CharT>& data);

		template<typename Resizable>
		static void resizeFunction(Resizable& data, size_t newSize);

		virtual void setReceiveMode(ReceiveMode mode = ReceiveMode::allowResize) final;

		virtual ReceiveMode getResizeMode() final;

		virtual ~Network() = default;
	};

	template<typename CharT, typename container>
	template<typename Resizable>
	void Network<CharT, container>::resizeFunction(Resizable& data, size_t newSize)
	{
		data.resize(newSize);
	}

	template<typename CharT, typename container>
	Network<CharT, container>::Network(const std::string& ip, const std::string& port, ReceiveMode mode) : mode(mode)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		addrinfo* info;
		addrinfo hints = {};

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		getaddrinfo(ip.data(), port.data(), &hints, &info);

		clientSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		connect(clientSocket, info->ai_addr, info->ai_addrlen);

		freeaddrinfo(info);
	}

	template<typename CharT, typename container>
	Network<CharT, container>::Network(SOCKET clientSocket, ReceiveMode mode) : clientSocket(clientSocket), mode(ReceiveMode::allowResize)
	{

	}

	template<typename CharT, typename container>
	__int32 Network<CharT, container>::sendData(const dataContainer& data)
	{
		static_assert(utility::checkSize<dataContainer>::value, "Your dataContainer hasn't size method");
		static_assert(utility::checkData<dataContainer>::value, "Your dataContainer hasn't data method");

		const __int32 size = data.size();
		char status;

		send(clientSocket, reinterpret_cast<const char*>(&size), sizeof(__int32), NULL);
		recv(clientSocket, &status, sizeof(char), NULL);

		__int32 bytes = send(clientSocket, data.data(), size, NULL);
		recv(clientSocket, &status, sizeof(char), NULL);

		return bytes;
	}

	template<typename CharT, typename container>
	__int32 Network<CharT, container>::sendData(const std::basic_string<CharT>& data)
	{
		const __int32 size = data.size();
		char status;

		send(clientSocket, reinterpret_cast<const char*>(&size), sizeof(__int32), NULL);
		recv(clientSocket, &status, sizeof(char), NULL);

		__int32 bytes = send(clientSocket, data.data(), size, NULL);
		recv(clientSocket, &status, sizeof(char), NULL);

		return bytes;
	}

	template<typename CharT, typename container>
	__int32 Network<CharT, container>::receiveData(dataContainer& data)
	{
		static_assert(utility::checkData<dataContainer>::value, "Your dataContainer hasn't data method");

		__int32 size;
		const char status = 1;

		recv(clientSocket, reinterpret_cast<char*>(&size), sizeof(__int32), NULL);
		send(clientSocket, &status, sizeof(char), NULL);

		if constexpr (utility::checkResize<dataContainer>::value)
		{
			if (mode == ReceiveMode::allowResize)
			{
				this->resizeFunction(data, size);
			}
		}

		__int32 bytes = recv(clientSocket, data.data(), size, NULL);
		send(clientSocket, &status, sizeof(char), NULL);

		return bytes;
	}

	template<typename CharT, typename container>
	__int32 Network<CharT, container>::receiveData(std::basic_string<CharT>& data)
	{
		__int32 size;
		const char status = 1;

		recv(clientSocket, reinterpret_cast<char*>(&size), sizeof(__int32), NULL);
		send(clientSocket, &status, sizeof(char), NULL);

		if (mode == ReceiveMode::allowResize)
		{
			this->resizeFunction(data, size);
		}

		__int32 bytes = recv(clientSocket, data.data(), size, NULL);
		send(clientSocket, &status, sizeof(char), NULL);

		return bytes;
	}

	template<typename CharT, typename container>
	void Network<CharT, container>::setReceiveMode(Network<CharT, container>::ReceiveMode mode)
	{
		this->mode = mode;
	}

	template<typename CharT, typename container>
	typename Network<CharT, container>::ReceiveMode Network<CharT, container>::getResizeMode()
	{
		return mode;
	}
}