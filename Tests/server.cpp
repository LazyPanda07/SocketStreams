#include <iostream>

#include <BaseTCPServer.h>

#include "IOSocketStream.h"

class TestServer : public web::BaseTCPServer
{
private:
	void clientConnection(const std::string& ip, SOCKET clientSocket, sockaddr address, std::function<void()>& cleanup) override
	{
		streams::IOSocketStream stream = streams::IOSocketStream::createStream<web::Network>(clientSocket);

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
		BaseTCPServer("8080", "127.0.0.1")
	{

	}
};

void runServer(bool& isRunning)
{
	TestServer server;

	server.start(true, [&isRunning]() { std::cout << "Server is running" << std::endl; isRunning = true; });
}
