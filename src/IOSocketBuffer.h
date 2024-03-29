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
		std::unique_ptr<web::Network> network;
		int lastPacketSize;
		bool endOfStream;

	protected:
		int_type overflow(int_type ch) override;

		int_type underflow() override;

		std::streamsize xsputn(const char_type* s, std::streamsize count) override;

		std::streamsize xsgetn(char_type* s, std::streamsize count) override;

	public:
		IOSocketBuffer() = default;

		/// @brief Deleted copy contructor
		IOSocketBuffer(const IOSocketBuffer&) = delete;

		/// @brief Move constructor
		/// @param other 
		IOSocketBuffer(IOSocketBuffer&& other) noexcept = default;

		/// @brief Server side constructor
		/// @param clientSocket 
		IOSocketBuffer(SOCKET clientSocket);

		/// @brief Client side constructor
		/// @param ip Remote address to connect to
		/// @param port Remote port to connect to
		/// @param timeout Timeout for receive and send calls in milliseconds
		/// @param mode Receive mode
		IOSocketBuffer(std::string_view ip, std::string_view port, DWORD timeout = 30'000);

		IOSocketBuffer(std::unique_ptr<web::Network>&& networkSubclass);

		/// @brief Deleted copy assignment operator
		/// @param  
		/// @return 
		IOSocketBuffer& operator = (const IOSocketBuffer&) = delete;

		/// @brief Move assignment operator 
		/// @param other 
		/// @return Self
		IOSocketBuffer& operator = (IOSocketBuffer&& other) noexcept = default;

		const std::unique_ptr<web::Network>& getNetwork() const noexcept;

		std::unique_ptr<web::Network>& getNetwork() noexcept;

		int getLastPacketSize() const noexcept;

		bool getEndOfStream() const noexcept;

		virtual ~IOSocketBuffer() = default;
	};
}
