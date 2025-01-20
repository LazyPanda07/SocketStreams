#include "IOSocketBuffer.h"

#ifdef __LINUX__
#include <sys/mman.h>
#include <unistd.h>
#else
#include <Windows.h>
#endif

namespace buffers
{
	void IOSocketBuffer::BufferArray::free()
	{
		if (pageData)
		{
#ifdef __LINUX__
			munmap(pageData, totalSize);
#else
			VirtualFree(pageData, 0, MEM_RELEASE);
#endif

			pageData = nullptr;
		}
	}

	IOSocketBuffer::BufferArray::BufferArray() :
		totalSize(0),
		pageData(nullptr)
	{
#ifdef __LINUX__
		totalSize = sysconf(_SC_PAGESIZE);
		pageSize = totalSize;
		pageData = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
		SYSTEM_INFO sysInfo;

		GetSystemInfo(&sysInfo);

		totalSize = sysInfo.dwPageSize;
		pageSize = totalSize;
		pageData = VirtualAlloc(NULL, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
	}

	IOSocketBuffer::BufferArray::BufferArray(BufferArray&& other) noexcept
	{
		(*this) = std::move(other);
	}

	IOSocketBuffer::BufferArray& IOSocketBuffer::BufferArray::operator =(BufferArray&& other) noexcept
	{
		pageData = other.pageData;
		pageSize = other.pageSize;
		totalSize = other.totalSize;

		other.pageData = nullptr;

		return *this;
	}

	size_t IOSocketBuffer::BufferArray::size() const
	{
		return totalSize;
	}

	char* IOSocketBuffer::BufferArray::data()
	{
		return static_cast<char*>(pageData);
	}

	const char* IOSocketBuffer::BufferArray::data() const
	{
		return static_cast<const char*>(pageData);
	}

	void IOSocketBuffer::BufferArray::resize(size_t size)
	{
		if (size <= totalSize)
		{
			return;
		}

		void* newRegion = nullptr;
		int pages = size / pageSize;

		if (size % pageSize)
		{
			pages++;
		}

		size_t newSize = pages * pageSize;

#ifdef __LINUX__
		newRegion = mmap(NULL, newSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
		newRegion = VirtualAlloc(NULL, newSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif

		std::copy(this->data(), this->data() + totalSize, newRegion);

		totalSize = newSize;

		this->free();

		pageData = newRegion;
	}

	char& IOSocketBuffer::BufferArray::operator [](size_t index)
	{
		return *(static_cast<char*>(pageData) + index);
	}

	IOSocketBuffer::BufferArray::~BufferArray()
	{
		this->free();
	}
}
