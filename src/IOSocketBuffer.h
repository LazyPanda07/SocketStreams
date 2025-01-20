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

	private:
		class BufferArray
		{
		private:
			size_t totalSize;
			size_t pageSize;
			void* pageData;

		private:
			void free();

		public:
			BufferArray();

			BufferArray(const BufferArray& other) = delete;

			BufferArray(BufferArray&& other) noexcept;

			BufferArray& operator =(const BufferArray& other) = delete;

			BufferArray& operator =(BufferArray&& other) noexcept;

			size_t size() const;

			char* data();

			const char* data() const;

			void resize(size_t size);

			char& operator [](size_t index);

			~BufferArray();
		};

	protected:
		size_t getAvailableInputSize() const;

	protected:
		std::unique_ptr<web::Network> network;
		int lastPacketSize;
		BufferArray inputData;
		bool endOfStream;

	protected:
		int_type overflow(int_type ch) override;

		int_type underflow() override;

		std::streamsize xsputn(const char_type* s, std::streamsize size) override;

		std::streamsize xsgetn(char_type* s, std::streamsize size) override;

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

		int getLastPacketSize() const noexcept;

		bool getEndOfStream() const noexcept;

		~IOSocketBuffer() = default;
	};
}
