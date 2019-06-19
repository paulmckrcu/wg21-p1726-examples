// Copyright IBM Corporation, 2019
// Authors: Paul E. McKenney, IBM Linux Technology Center
//	Adapted from Maged Michael's pseudocode in WG14 N2369.
//	And further adapted to use uintptr_t to avoid dead pointer zap.

#include <cstddef>
#include <utility>
#include <atomic>
#include <thread>

template<typename T>
class LifoPush {

	class Node {
	public:
		T val;
		uintptr_t next;
		Node(T v) { val = v; }
	};

	std::atomic<uintptr_t> top{0};

public:

	LifoPush() {} // Needed for old compilers

	bool list_empty()
	{
		return top.load() == 0;
	}

	void list_push(T v)
	{
		Node *newnode = new Node(v);
		uintptr_t u = (uintptr_t)(newnode);

		newnode->next = top.load();
		while (!top.compare_exchange_weak(newnode->next, u));
	}

	template<typename F>
	void list_pop_all(F f)
	{
		uintptr_t u = top.exchange(0);
		Node *p = (Node *)(top.exchange(0));

		while (p) {
			Node *next = (Node *)(p->next);
			f(p->val);
			delete p;
			p = next;
		}
	}
};

#include "lifo-stress.hpp"
