#pragma once

#include <string>

namespace web::utility
{
	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;

	/**
	 * @brief Get curret version of SocketStreams library
	 * @return
	 */
	std::string getSocketStreamsVersion();
}
