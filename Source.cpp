#define _USE_MATH_DEFINES
#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>
#include <cassert>

unsigned int doneCount = 0;


double work(unsigned int idx, double input) {
	doneCount = doneCount + 1;
	return abs(sin(input * idx)) * input;
}

void doWork(double* data, unsigned int count, unsigned int offset = 0) {
	for (unsigned int i = offset; i < (offset + count); i++) {
		data[i] = work(i, data[i]);
	}
}

void doWorkMT(double* data, unsigned int count, unsigned int threadCount) {
	//work per thread
	unsigned int wpt = count / threadCount;

	//schedule work
	std::vector<std::thread> threads;


	for (unsigned int i = 0; i < threadCount; i++) {
		//create threads
		threads.push_back(std::thread(doWork, data, wpt, i * wpt));
		
	}
	//wait for threads to finish
	for (auto it = threads.begin(); it != threads.end(); it++) {
		it->join();
	}
}

int main() {
	//heavy work count
	const unsigned int workCount = 1024 * 1024 * 128;
	std::cout << "===GENERATE WORK===" << std::endl;
	//generate work
	double* workData = (double*) malloc(sizeof(double) * workCount);


	for (unsigned int i = 0; i < workCount; i++) {
		workData[i] = std::rand() / (double)RAND_MAX;
	}
	
	std::cout << "===START WORK===" << std::endl;
	auto tStart = std::chrono::steady_clock::now();
	doWorkMT(workData, workCount, 1);
	auto tStop = std::chrono::steady_clock::now();

	//print timing result
	std::cout << std::endl << "timings: " << std::endl
		<< "NS: " << std::chrono::duration_cast<std::chrono::nanoseconds>(tStop - tStart).count() << std::endl
		<< "US: " << std::chrono::duration_cast<std::chrono::microseconds>(tStop - tStart).count() << std::endl
		<< "MS: " << std::chrono::duration_cast<std::chrono::milliseconds>(tStop - tStart).count() << std::endl
		<< "S: " << std::chrono::duration_cast<std::chrono::seconds>(tStop - tStart).count() << std::endl;

	//sassert my work
	assert(doneCount == workCount && "Work Success");



	free(workData);
	return 0;
}


