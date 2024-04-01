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

#include "WebException.h"
#include "ContainerWrapper.h"

#ifndef __LINUX__
#pragma comment (lib, "ws2_32.lib")
#endif // !__LINUX__

#ifdef __LINUX__
#ifndef WINDOWS_STYLE_DEFINITION
#define WINDOWS_STYLE_DEFINITION

#define closesocket close
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define DWORD uint32_t

#endif // WINDOWS_STYLE_DEFINITION
#endif // __LINUX__

namespace web
{
	/// @brief Base network class
	class Network
	{
	protected:
		SOCKET clientSocket;

	protected:
		virtual int sendBytesImplementation(const char* data, int size, int flags = NULL);

		virtual int receiveBytesImplementation(char* data, int size, int flags = NULL);

	public:
		/// @brief Client side constructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		/// @exception WebException 
		Network(std::string_view ip, std::string_view port, DWORD timeout = 30'000);

		/// @brief Server side contructor
		/// @param clientSocket 
		/// @param mode 
		Network(SOCKET clientSocket);

		virtual int sendData(const utility::ContainerWrapper& data, bool& endOfStream);

		virtual int sendRawData(const char* data, int size, bool& endOfStream);

		/// @brief Receive data through network
		/// @param data 
		/// @return Total number of received bytes 
		virtual int receiveData(utility::ContainerWrapper& data, bool& endOfStream);

		virtual int receiveRawData(char* data, int size, bool& endOfStream);

		/// @brief Errors logging, default implementation uses clog
		/// @param message Log message
		/// @param data Additional data
		virtual void log(const std::string& message, std::any&& data = "");

		/// @brief clientSocket getter
		/// @return clientSocket
		SOCKET getClientSocket() const;

		/// @brief Send raw bytes through network
		/// @tparam DataT 
		/// @param data 
		/// @param size 
		/// @return Total number of sended bytes 
		/// @exception WebException  
		template<typename DataT>
		int sendBytes(const DataT* data, int size, bool& endOfStream);

		/// @brief 
		/// @tparam DataT 
		/// @param data 
		/// @param size 
		/// @return Total number of received bytes 
		/// @exception WebException  
		template<typename DataT>
		int receiveBytes(DataT* data, int size, bool& endOfStream);

		virtual ~Network();
	};

	template<typename DataT>
	int Network::sendBytes(const DataT* data, int size, bool& endOfStream)
	{
		int lastSend = 0;
		int totalSent = 0;

		endOfStream = false;

		do
		{
			lastSend = this->sendBytesImplementation(reinterpret_cast<const char*>(data) + totalSent, size - totalSent);

			if (lastSend == SOCKET_ERROR)
			{
				THROW_WEB_EXCEPTION;
			}
			else if (!lastSend)
			{
				endOfStream = true;

				return totalSent;
			}

			totalSent += lastSend;

		} while (totalSent < size);

		return totalSent;
	}

	template<typename DataT>
	int Network::receiveBytes(DataT* data, int size, bool& endOfStream)
	{
		int lastReceive = 0;
		int totalReceive = 0;

		endOfStream = false;

		do
		{
			lastReceive = this->receiveBytesImplementation(reinterpret_cast<char*>(data) + totalReceive, size - totalReceive);

			if (lastReceive == SOCKET_ERROR)
			{
				THROW_WEB_EXCEPTION;
			}
			else if (!lastReceive)
			{
				endOfStream = true;

				return totalReceive;
			}

			totalReceive += lastReceive;

		} while (totalReceive < size);

		return totalReceive;
	}
}
