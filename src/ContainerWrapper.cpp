#include "ContainerWrapper.h"

namespace web
{
	namespace utility
	{
		char* ContainerWrapper::data()
		{
			return dataImplementation();
		}

		const char* ContainerWrapper::data() const
		{
			return constDataImplementation();
		}

		size_t ContainerWrapper::size() const
		{
			return sizeImplementation();
		}

		void ContainerWrapper::resize(size_t newSize)
		{
			resizeImplementation(newSize);
		}

		char& ContainerWrapper::operator [](size_t index)
		{
			return operatorImplementation(index);
		}
	}
}
