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
	template<typename T>
	concept Timeout = requires(T value)
	{
		{ std::chrono::duration_cast<std::chrono::milliseconds>(value) };
	};

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
		void setTimeout(int64_t timeout);

		template<typename FunctionT, typename... Args>
		auto callInNonBlockingMode(const FunctionT& functor, Args&&... args) const -> decltype(std::declval<FunctionT>()(std::forward<Args>(args)...));

	protected:
		Network(std::string_view ip, std::string_view port, int64_t timeout);

	public:
		/// @brief Client side constructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls
		/// @exception WebException 
		template<Timeout T = std::chrono::seconds>
		Network(std::string_view ip, std::string_view port, T timeout = 30s);

		/// @brief Server side contructor
		/// @param clientSocket 
		template<Timeout T = std::chrono::seconds>
		Network(SOCKET clientSocket, T timeout = 30s);

		/**
		 * @brief Check if Network contains data
		 * @param availableBytes Get number of available bytes(optional)
		 * @param hasConnection Check if client still connected(optional)
		 * @return
		 */
		bool isDataAvailable(int* availableBytes = nullptr, bool* hasConnection = nullptr) const;

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

}

namespace web
{
	template<typename FunctionT, typename... Args>
	auto Network::callInNonBlockingMode(const FunctionT& functor, Args&&... args) const -> decltype(std::declval<FunctionT>()(std::forward<Args>(args)...))
	{
		SOCKET socket = this->getClientSocket();

#ifdef __LINUX__
		int flags = fcntl(socket, F_GETFL, 0);

		if (flags == -1)
		{
			THROW_WEB_EXCEPTION();
		}

		int isBlocking = !(flags & O_NONBLOCK);

		if (isBlocking)
		{
			fcntl(socket, F_SETFL, flags | O_NONBLOCK);
		}
#else
		u_long originalMode = 0;
		u_long nonblock = 1;

		ioctlsocket(socket, FIONBIO, &originalMode);
		ioctlsocket(socket, FIONBIO, &nonblock);
#endif

		auto result = functor(std::forward<Args>(args)...);

#ifdef __LINUX__
		if (isBlocking)
		{
			fcntl(socket, F_SETFL, flags);
		}
#else
		ioctlsocket(socket, FIONBIO, &originalMode);
#endif

		return result;
	}

	template<Timeout T>
	Network::Network(std::string_view ip, std::string_view port, T timeout) :
		Network(ip, port, std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count())
	{

	}

	template<Timeout T>
	Network::Network(SOCKET clientSocket, T timeout)
	{
#ifndef __LINUX__
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			THROW_WEB_EXCEPTION;
		}
#endif // !__LINUX__

		handle = std::shared_ptr<SOCKET>(new SOCKET(clientSocket), [](SOCKET* ptr) { closesocket(*ptr); delete ptr; });

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
			std::string_view& receiveBuffer = buffers.front();
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
