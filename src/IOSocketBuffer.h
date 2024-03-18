#pragma once

#include <streambuf>
#include <memory>

#include "Network.h"

namespace buffers
{
	/// @brief Base input/output socket buffer
	class IOSocketBuffer : public std::streambuf
	{
	public:
		using typename std::streambuf::int_type;
		using typename std::streambuf::char_type;
		using typename std::streambuf::traits_type;

		using std::streambuf::pbase;
		using std::streambuf::pptr;
		using std::streambuf::epptr;
		using std::streambuf::setp;
		using std::streambuf::pbump;

		using std::streambuf::eback;
		using std::streambuf::gptr;
		using std::streambuf::egptr;
		using std::streambuf::setg;
		using std::streambuf::gbump;

	protected:
		enum class IOType
		{
			input,
			output
		};

	protected:
		std::vector<char> outBuffer;
		std::vector<char> inBuffer;
		std::unique_ptr<web::Network> network;
		int lastPacketSize;
		bool endOfStream;
		IOType type;

	protected:
		void setPointers();

		int_type overflow(int_type ch) override;

		int_type underflow() override;

		std::streamsize xsputn(const char_type* s, std::streamsize count) override;

		std::streamsize xsgetn(char_type* s, std::streamsize count) override;

		int sync() override;

	public:
		IOSocketBuffer() = default;

		/// @brief Deleted copy contructor
		IOSocketBuffer(const IOSocketBuffer&) = delete;

		/// @brief Move constructor
		/// @param other 
		IOSocketBuffer(IOSocketBuffer&& other) noexcept;

		/// @brief Server side constructor
		/// @param clientSocket 
		IOSocketBuffer(SOCKET clientSocket);

		/// @brief Server side constructor
		/// @param clientSocket 
		/// @param bufferSize Fixed buffer size
		IOSocketBuffer(SOCKET clientSocket, size_t bufferSize);

		/// @brief Client side constructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		IOSocketBuffer(std::string_view ip, std::string_view port, DWORD timeout = 30'000);

		/// @brief Client side constructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param bufferSize 
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		IOSocketBuffer(std::string_view ip, std::string_view port, size_t bufferSize, DWORD timeout = 30'000);

		IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass);

		IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass, size_t bufferSize);

		/// @brief Deleted copy assignment operator
		/// @param  
		/// @return 
		IOSocketBuffer& operator = (const IOSocketBuffer&) = delete;

		/// @brief Move assignment operator 
		/// @param other 
		/// @return Self
		IOSocketBuffer& operator = (IOSocketBuffer&& other) noexcept;

		void setInputType() noexcept;

		void setOutputType() noexcept;

		const std::unique_ptr<web::Network>& getNetwork() const;

		std::unique_ptr<web::Network>& getNetwork();

		int getLastPacketSize() const noexcept;

		bool getEndOfStream() const noexcept;

		virtual ~IOSocketBuffer() = default;
	};
}
