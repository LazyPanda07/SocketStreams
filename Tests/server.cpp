#include <iostream>

#include "IOSocketStream.h"

#include "BaseTCPServer.h"

class TestServer : public web::BaseTCPServer
{
private:
	void clientConnection(const std::string& ip, SOCKET clientSocket, sockaddr address, std::function<void()>& cleanup) override
	{
		streams::IOSocketStream stream(clientSocket);

		while (true)
		{
			std::string data;

			if (stream.eof() || stream.bad())
			{
				break;
			}

			stream >> data;

			if (stream.eof() || stream.bad())
			{
				break;
			}

			stream << data;
		}
	}

public:
	TestServer() :
		BaseTCPServer("127.0.0.1", "8080")
	{

	}
};

void runServer()
{
	TestServer server;

	server.start(true, []() { std::cout << "Server is running" << std::endl; });
}
