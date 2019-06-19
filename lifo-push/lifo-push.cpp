// Copyright IBM Corporation, 2019
// Authors: Paul E. McKenney, IBM Linux Technology Center
//	Adapted from Maged Michael's pseudocode in WG14 N2369.

#include <cstddef>
#include <utility>
#include <atomic>
#include <thread>

template<typename T>
class LifoPush {

	class Node {
	public:
		T val;
		Node *next;
		Node(T v) : val(v) { }
	};

	std::atomic<Node *> top{nullptr};

public:

	LifoPush() {} // Needed for old compilers

	bool list_empty()
	{
		return top.load() == nullptr;
	}

	void list_push(T v)
	{
		Node *newnode = new Node(v);

		newnode->next = top.load(); // Maybe dead pointer here and below
		while (!top.compare_exchange_weak(newnode->next, newnode))
			;
	}

	template<typename F>
	void list_pop_all(F f)
	{
		Node *p = top.exchange(nullptr); // Cannot be dead pointer

		while (p) {
			Node *next = p->next; // Maybe dead pointer

			f(p->val); // Maybe dereference of dead pointer
			delete p;
			p = next;
		}
	}
};

#include "lifo-stress.hpp"
