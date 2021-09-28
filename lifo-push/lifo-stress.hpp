////////////////////////////////////////////////////////////////////////
//
// Stress-test code
//
// Also serves as a crude high-contention performance test.
//
// Copyright IBM Corporation, 2019
// Authors: Paul E. McKenney, IBM Linux Technology Center
//	Adapted from similar tests in "Is Parallel Programming Hard,
//	And, If So, What Can You Do About It?":
//	git://git.kernel.org/pub/scm/linux/kernel/git/paulmck/perfbook.git
//	Further adapted from code samples from WG14 N2369:
//	https://github.com/paulmckrcu/wg14-n2369-examples.git
//	C++-ification with help from Maged Michael.

#include <vector>
#include <chrono>
#include <iostream>

LifoPush<char *> lifo;

#define N_PUSH 2
#define N_POP  2
#define N_ELEM (10 * 1000 * 1000L)

char s[N_PUSH * N_ELEM] = { 0 };
std::atomic<int> goflag;

void push_em(char *my_s)
{
	long i;

	while (!goflag.load())
		continue;
	for (i = 0; i < N_ELEM; i++)
		lifo.list_push(&my_s[i]);
}

void *pop_em()
{
	while (!goflag.load())
		continue;
	while (goflag.load() == 1)
		lifo.list_pop_all([](char *my_s) { (*my_s)++; });
	return nullptr;
}

int main(int argc, char *argv[])
{
	long i;
	std::vector<std::thread> pushthreads;
	std::vector<std::thread> popthreads;

	for (i = 0; i < N_PUSH; i++)
		pushthreads.emplace_back([i] { push_em(&s[N_ELEM * i]); });
	for (i = 0; i < N_POP; i++)
		popthreads.emplace_back([] { pop_em(); });
	goflag.store(1);
	for (auto &t:pushthreads) {
		std::cout << "Join with pushthread on line " << __LINE__ << "\n";
		t.join();
	}
	while (!lifo.list_empty())
		std::this_thread::sleep_for(std::chrono::seconds(1));
	goflag.store(2);
	for (auto &t:popthreads) {
		std::cout << "Join with popthread\n";
		t.join();
	}
	for (i = 0; i < N_PUSH * N_ELEM; i++)
		if (s[i] != 1) {
			fprintf(stderr, "Entry %ld left set\n", i);
			abort();
		}
}
