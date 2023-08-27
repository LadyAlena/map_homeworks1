#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <execution>
#include <random>

namespace for_vector {

	template <typename T>
	void sum(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& result, int start, int end) {
		
		if (a.size() != b.size()) {
			throw std::length_error("The size of vector a is not equal to the size of vector b.");
		}

		if (result.size() != a.size()) {
			throw std::length_error("Incorrect size of the result vector");
		}
		
		for (int i = start; i < end; ++i) {
			result[i] = a[i] + b[i];
		}
	}

}

int main(int argc, char** argv) {

	try {
		std::cout << "Количество аппаратных ядер: " << std::thread::hardware_concurrency() << std::endl << std::endl;

		std::vector<int> numbers_of_threads {1, 2, 4, 8, 16};

		std::vector<int> sizes_of_vector {1'000, 10'000, 100'000, 1'000'000};

		std::cout << "\t\t";

		for (const auto& size_of_vector : sizes_of_vector) {
			std::cout << size_of_vector << "\t";
		}

		std::cout << std::endl;

		for (const auto& number_of_threads : numbers_of_threads) {

			std::cout << number_of_threads << "\t" << "потоков" << "\t";

			for (const auto& size_of_vector : sizes_of_vector) {

				std::vector<std::thread> pull_threads;

				std::mt19937 gen;
				std::uniform_int_distribution<int> dist(0, size_of_vector);

				std::vector<int> v1(size_of_vector);
				std::generate(std::execution::par, v1.begin(), v1.end(), [&]() { return dist(gen); });

				std::vector<int> v2(size_of_vector);
				std::generate(std::execution::par, v2.begin(), v2.end(), [&]() { return dist(gen); });

				std::vector<int> result(size_of_vector);

				auto fragment_size = size_of_vector / number_of_threads;

				auto start_time = std::chrono::steady_clock::now();

				for (size_t i = 0; i < number_of_threads; ++i) {
					auto fragment_start = i * fragment_size;
					auto fragment_end = (i + 1) * fragment_size;

					pull_threads.push_back(std::thread(for_vector::sum<int>, std::cref(v1), std::cref(v2), std::ref(result), fragment_start, fragment_end));
				}

				for (auto& thread : pull_threads) {
					thread.join();
				}

				auto end_time = std::chrono::steady_clock::now();

				auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

				std::cout << static_cast<double>(delta)/1000 << "s" << "\t";	
			}

			std::cout << "\n";
		}
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}