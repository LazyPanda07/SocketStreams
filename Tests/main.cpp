#include <thread>

#include "gtest/gtest.h"

#include "IOSocketStream.h"

extern void runServer();

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
	std::thread(runServer).detach();

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
