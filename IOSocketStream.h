#pragma once

#include <istream>

#include "IOSocketBuffer.h"

namespace streams
{
	template<typename CharT, typename container = std::vector<CharT>>
	class IOSocketStream : public std::basic_iostream<CharT>
	{
	public:
		using dataContainer = typename container;
		using ioBuffer = typename buffers::IOSocketBuffer<CharT, container>;

	protected:
		buffers::IOSocketBuffer<CharT, container>* buffer;

	public:
		IOSocketStream(SOCKET clientSocket);

		IOSocketStream(SOCKET clientSocket, size_t bufferSize);

		IOSocketStream(const std::string& ip, const std::string& port);

		IOSocketStream(const std::string& ip, const std::string& port, size_t bufferSize);

		IOSocketStream(ioBuffer* IOSocketBufferSubclass);

#pragma region Deleted
		std::basic_iostream<CharT>& operator << (short) = delete;
		std::basic_iostream<CharT>& operator << (int) = delete;
		std::basic_iostream<CharT>& operator << (long) = delete;
		std::basic_iostream<CharT>& operator << (long long) = delete;

		std::basic_iostream<CharT>& operator << (unsigned short) = delete;
		std::basic_iostream<CharT>& operator << (unsigned int) = delete;
		std::basic_iostream<CharT>& operator << (unsigned long) = delete;
		std::basic_iostream<CharT>& operator << (unsigned long long) = delete;

		std::basic_iostream<CharT>& operator << (float) = delete;
		std::basic_iostream<CharT>& operator << (double) = delete;
		std::basic_iostream<CharT>& operator << (long double) = delete;

		std::basic_iostream<CharT>& operator >> (short&) = delete;
		std::basic_iostream<CharT>& operator >> (int&) = delete;
		std::basic_iostream<CharT>& operator >> (long&) = delete;
		std::basic_iostream<CharT>& operator >> (long long&) = delete;

		std::basic_iostream<CharT>& operator >> (unsigned short&) = delete;
		std::basic_iostream<CharT>& operator >> (unsigned int&) = delete;
		std::basic_iostream<CharT>& operator >> (unsigned long&) = delete;
		std::basic_iostream<CharT>& operator >> (unsigned long long&) = delete;

		std::basic_iostream<CharT>& operator >> (float&) = delete;
		std::basic_iostream<CharT>& operator >> (double&) = delete;
		std::basic_iostream<CharT>& operator >> (long double&) = delete;
#pragma endregion

		virtual std::basic_iostream<CharT>& operator << (const dataContainer& data);

		virtual std::basic_iostream<CharT>& operator >> (dataContainer& data);

		virtual ~IOSocketStream();
	};

	template<typename CharT, typename container>
	IOSocketStream<CharT, container>::IOSocketStream(SOCKET clientSocket) : buffer(new ioBuffer(clientSocket)), std::basic_iostream<CharT>(&buffer)
	{

	}

	template<typename CharT, typename container>
	IOSocketStream<CharT, container>::IOSocketStream(SOCKET clientSocket, size_t bufferSize) : buffer(new ioBuffer(clientSocket, bufferSize)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename container>
	IOSocketStream<CharT, container>::IOSocketStream(const std::string& ip, const std::string& port) : buffer(new ioBuffer(ip, port)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename container>
	IOSocketStream<CharT, container>::IOSocketStream(const std::string& ip, const std::string& port, size_t bufferSize) : buffer(new ioBuffer(ip, port, bufferSize)), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename container>
	IOSocketStream<CharT, container>::IOSocketStream(IOSocketStream<CharT, container>::ioBuffer* IOSocketBufferSubclass) : buffer(IOSocketBufferSubclass), std::basic_iostream<CharT>(buffer)
	{

	}

	template<typename CharT, typename container>
	std::basic_iostream<CharT>& IOSocketStream<CharT, container>::operator << (const IOSocketStream<CharT, container>::dataContainer& data)
	{
		buffer->sputn(data.data(), data.size());

		return *this;
	}

	template<typename CharT, typename container>
	std::basic_iostream<CharT>& IOSocketStream<CharT, container>::operator >> (IOSocketStream<CharT, container>::dataContainer& data)
	{
		if constexpr (utility::checkResize<dataContainer>::value)
		{
			buffer->setInputType();
			buffer->pubsync();
			data.resize(buffer->getLastPacketSize());
		}

		buffer->sgetn(data.data(), data.size());

		return *this;
	}

	template<typename CharT, typename container>
	IOSocketStream<CharT, container>::~IOSocketStream()
	{
		delete buffer;
	}
}