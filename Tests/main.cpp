#include <thread>
#include <chrono>

#include "gtest/gtest.h"

#include "IOSocketStream.h"

extern void runServer(bool& isRunning);

TEST(Streams, DefaultNetwork)
{
	streams::IOSocketStream stream = streams::IOSocketStream::createStream<web::Network>("127.0.0.1", "8080");
	std::string data = "some data";

	{
		std::string result;

		stream << data;

		stream >> result;

		ASSERT_EQ(data, result);
	}

	data = "another data";

	{	
		std::string result;

		stream << data;

		stream >> result;

		ASSERT_EQ(data, result);
	}
}

int main(int argc, char** argv)
{
	bool isRunning = false;

	std::thread(runServer, std::ref(isRunning)).detach();

	testing::InitGoogleTest(&argc, argv);

	while (!isRunning)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::cout << "Wait server..." << std::endl;
	}

	return RUN_ALL_TESTS();
}
