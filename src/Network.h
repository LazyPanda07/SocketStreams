#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <any>
#include <memory>
#include <chrono>

#ifdef __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
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
	using namespace std::chrono_literals;

	/// @brief Base network class
	class Network
	{
	protected:
		std::shared_ptr<SOCKET> handle;
		std::queue<std::string_view> buffers;

	protected:
		virtual int sendBytesImplementation(const char* data, int size, int flags = 0);

		virtual int receiveBytesImplementation(char* data, int size, int flags = 0);

		virtual void throwException(int line, std::string_view file) const;

	protected:
		void setTimeout(uint32_t timeout);

	protected:
		Network(std::string_view ip, std::string_view port, uint32_t timeout);

	public:
		/// @brief Client side constructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls
		/// @exception WebException 
		template<typename RepT, typename PeriodT>
		Network(std::string_view ip, std::string_view port, std::chrono::duration<RepT, PeriodT> timeout = 30s);

		/// @brief Server side contructor
		/// @param clientSocket 
		template<typename RepT, typename PeriodT>
		Network(SOCKET clientSocket, std::chrono::duration<RepT, PeriodT> timeout = 30s);

		/**
		 * @brief Check if Network contains data
		 * @return
		 */
		bool isDataAvailable(int* availableBytes = nullptr) const;

		/**
		* @brief Send data through network
		* @param data Actual data with some useful methods. Called with std::vector<char> or std::string
		* @param endOfStream Is connection closed
		* @return Total number of bytes send
		*/
		virtual int sendData(const utility::ContainerWrapper& data, bool& endOfStream, int flags = 0);

		/**
		* @brief Send raw data through network
		* @param data Actual data. Called from write method
		* @param endOfStream Is connection closed
		* @return Total number bytes send
		*/
		virtual int sendRawData(const char* data, int size, bool& endOfStream, int flags = 0);

		/**
		* @brief Receive data through network
		* @param data Actual data with some useful methods. Called with std::vector<char> or std::string
		* @param endOfStream Is connection closed
		* @return Total number of bytes receive
		*/
		virtual int receiveData(utility::ContainerWrapper& data, bool& endOfStream, int flags = 0);

		/**
		* @brief Receive data through network
		* @param data Actual data. Called from read method
		* @param endOfStream Is connection closed
		* @return Total number of bytes receive
		*/
		virtual int receiveRawData(char* data, int size, bool& endOfStream, int flags = 0);

		/// @brief Errors logging, default implementation uses clog
		/// @param message Log message
		/// @param data Additional data
		virtual void log(const std::string& message, std::any&& data = "");

		/**
		 * @brief Add additional data that uses before getting bytes from network
		 * @param buffer
		 */
		void addReceiveBuffer(std::string_view buffer);

		/// @brief clientSocket getter
		/// @return clientSocket
		SOCKET getClientSocket() const;

		/// @brief Send raw bytes through network
		/// @tparam DataT 
		/// @param data 
		/// @param size 
		/// @param endOfStream 
		/// @param flags 
		/// @return Total number of sended bytes 
		/// @exception WebException  
		template<typename DataT>
		int sendBytes(const DataT* data, int size, bool& endOfStream, int flags = 0);

		/// @brief 
		/// @tparam DataT 
		/// @param data 
		/// @param size 
		/// @param endOfStream 
		/// @param flags
		/// @return Total number of received bytes 
		/// @exception WebException  
		template<typename DataT>
		int receiveBytes(DataT* data, int size, bool& endOfStream, int flags = 0);

		virtual ~Network() = default;
	};

	template<typename RepT, typename PeriodT>
	Network::Network(std::string_view ip, std::string_view port, std::chrono::duration<RepT, PeriodT> timeout) :
		Network(ip, port, std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count())
	{

	}

	template<typename RepT, typename PeriodT>
	Network::Network(SOCKET clientSocket, std::chrono::duration<RepT, PeriodT> timeout)
	{
#ifndef __LINUX__
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			THROW_WEB_EXCEPTION;
		}
#endif // !__LINUX__

		handle = new SOCKET(clientSocket), [](SOCKET* ptr) { closesocket(*ptr); delete ptr; };

		this->setTimeout(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
	}

	template<typename DataT>
	int Network::sendBytes(const DataT* data, int size, bool& endOfStream, int flags)
	{
		int lastSend = 0;
		int totalSent = 0;

		endOfStream = false;

		do
		{
			lastSend = this->sendBytesImplementation(reinterpret_cast<const char*>(data) + totalSent, size - totalSent, flags);

			if (lastSend == SOCKET_ERROR)
			{
				this->throwException(__LINE__, __FILE__);
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
	int Network::receiveBytes(DataT* data, int size, bool& endOfStream, int flags)
	{
		int receive = 0;
		char* actualData = reinterpret_cast<char*>(data);

		while (buffers.size())
		{
			std::string_view receiveBuffer = buffers.front();
			int fromBufferSize = std::min<int>(static_cast<int>(receiveBuffer.size()), size);

			std::copy(receiveBuffer.data(), receiveBuffer.data() + fromBufferSize, actualData);

			receiveBuffer = std::string_view(receiveBuffer.data() + fromBufferSize, receiveBuffer.size() - fromBufferSize);

			actualData += fromBufferSize;
			receive += fromBufferSize;
			size -= fromBufferSize;

			if (receiveBuffer.empty())
			{
				buffers.pop();
			}
		}

		if (size)
		{
			receive += this->receiveBytesImplementation(actualData, size, flags);
		}

		endOfStream = !static_cast<bool>(receive);

		if (receive == SOCKET_ERROR)
		{
			this->throwException(__LINE__, __FILE__);
		}

		return receive;
	}
}
