#include "ContainerWrapper.h"

namespace web::utility
{
	ContainerWrapper::ContainerWrapper
	(
		const std::function<char* ()>& dataImplementation,
		const std::function<const char* ()>& constDataImplementation,
		const std::function<size_t()>& sizeImplementation,
		const std::function<void(size_t)>& resizeImplementation,
		const std::function<char& (size_t)>& operatorImplementation
	) :
		dataImplementation(dataImplementation),
		constDataImplementation(constDataImplementation),
		sizeImplementation(sizeImplementation),
		resizeImplementation(resizeImplementation),
		operatorImplementation(operatorImplementation)
	{

	}

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
