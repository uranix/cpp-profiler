#include <iostream>
#include <chrono>
#include <thread>

#include <cmath>

#include "Profiler.h"

double foo(int m) {
	PROFILE_ME;

	double sum = 0;
	for (int i = 0; i < 1000000; i++) {
		sum += std::cos(42. * m * i * i);
	}
	for (int i = 0; i < 100000; i++) {
		PROFILE_ME_AS("nop");
	}
	return std::abs(sum);
}

double bar(int delay_us) {
	PROFILE_ME;
	std::cout << "[bar] Sleeping for " << delay_us << "us twice" << std::endl;
	std::this_thread::sleep_for(std::chrono::microseconds(delay_us));
	PROFILE_END; // Early profile stop for bar
	std::this_thread::sleep_for(std::chrono::microseconds(delay_us));
}

int fibb(int n) {
	PROFILE_ME;
	std::this_thread::sleep_for(std::chrono::microseconds(1000));
	if (n == 0 || n == 1)
		return 1;
	return fibb(n-1) + fibb(n-2);
}

int rec2(int n);

int rec1(int n) {
	PROFILE_ME;
	std::this_thread::sleep_for(std::chrono::microseconds(100));
	if (n == 0 || n == 1)
		return 1;
	return rec1(n-1) + rec2(n-2);
}

int rec2(int n) {
	PROFILE_ME;
	std::this_thread::sleep_for(std::chrono::microseconds(100));
	if (n == 0 || n == 1)
		return n + 1;
	return rec2(n-1) + rec1(n-2);
}

int main() {
	PROFILE_ME;
	double total = 0;
	{
		PROFILE_ME_AS("foo for loop");
		for (int m = 1; m <= 10; m++)
			total += foo(m);
	}

	{
		PROFILE_ME_AS("bar wait");
		bar(10 * total);
	}

	{
		PROFILE_ME_AS("fibb");
		fibb(10);
	}

	{
		PROFILE_ME_AS("rec1 + rec2");
		rec1(10);
	}

	PROFILE_END; // Early profile stop to finalize results for main

	std::cout << profiler::getInstance() << std::endl;
	return 0;
}
