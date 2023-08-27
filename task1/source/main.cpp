#include <iostream>
#include <thread>
#include "Queue/Queue.h"

int main(int argc, char** argv) {
	Queue queue(10);

	std::cout << "start of queue" << std::endl;

	std::thread client([&queue]() { queue.add_client(); });
	std::thread handler([&queue]() {queue.process_client(); });

	client.join();
	handler.join();

	std::cout << "end of queue" << std::endl;

	return 0;
}