#pragma once

#include <string>
#include <stdexcept>

namespace web
{
	namespace exceptions
	{
		/// @brief Network exception
		class WebException : public std::runtime_error
		{
		protected:
			std::string data;
			std::string_view file;
			int errorCode;
			int line;

		private:
			WebException();

		public:
			WebException(int line, std::string_view file);

			WebException(const WebException& other) = default;

			WebException(WebException&& other) noexcept = default;

			WebException& operator = (const WebException& other) = default;

			WebException& operator = (WebException&& other) noexcept = default;

			const char* what() const noexcept override;

			int getErrorCode() const noexcept;

			int getLine() const noexcept;

			std::string_view getFile() const noexcept;

			virtual ~WebException() = default;
		};
	}
}

#define THROW_WEB_EXCEPTION { throw web::exceptions::WebException(__LINE__, __FILE__); }
