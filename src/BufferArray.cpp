#include "IOSocketBuffer.h"

#ifdef __LINUX__
#include <sys/mman.h>
#include <unistd.h>
#else
#include <Windows.h>
#endif

namespace buffers
{
	IOSocketBuffer::BufferArray::BufferArray() :
		dataSize(0),
		data(nullptr)
	{
#ifdef __LINUX__
		dataSize = sysconf(_SC_PAGESIZE);
		data =  mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
		SYSTEM_INFO sysInfo;

		GetSystemInfo(&sysInfo);

		dataSize = sysInfo.dwPageSize;
		data = VirtualAlloc(NULL, dataSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
	}

	IOSocketBuffer::BufferArray::BufferArray(BufferArray&& other) noexcept
	{
		(*this) = std::move(other);
	}

	IOSocketBuffer::BufferArray& IOSocketBuffer::BufferArray::operator =(BufferArray&& other) noexcept
	{
		data = other.data;
		dataSize = other.dataSize;

		other.data = nullptr;

		return *this;
	}

	size_t IOSocketBuffer::BufferArray::size() const
	{
		return dataSize;
	}

	char* IOSocketBuffer::BufferArray::get(size_t offset)
	{
		return static_cast<char*>(data) + offset;
	}

	IOSocketBuffer::BufferArray::~BufferArray()
	{
		if (data)
		{
#ifdef __LINUX__
			munmap(data, dataSize);
#else
			VirtualFree(data, 0, MEM_RELEASE);
#endif
		}
	}
}
