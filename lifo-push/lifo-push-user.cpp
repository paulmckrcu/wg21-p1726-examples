// Copyright Facebook, 2019
// Author: Maged Michael

#include <cstddef>
#include <utility>
#include <atomic>
#include <thread>


struct Node {
public:
	Node *next;
	Value val;
	Node(Value v): val(v) {}
};

struct LifoPush {

	std::atomic<Node *> top{nullptr};

	void list_push(Node *node)
	{
		node->next = top->load();  // Might be dead pointer.
		while (!top.compare_exchange_weak(node->next, node));  // Might be dead pointer.
	}

	void list_pop_all()
	{
		return top.exchange(nullptr);
	}
} lifo;

// ----------


void foo_pop()
{
	Node *p = lifo.list_pop_all();

	while (p) {
		Node *n = p->next; // Might be dead pointer.

		foo(p);
		delete p;
		p = n;
	}
}

void foo_push(Value v)
{
	lifo.list_push(new Node(v));
}

// std::vector<std::thread> threads;
// for (int.tid = 0; tid < n; ++tid)
//	threads.emplace_back([&,tid] { child(tid); });

//...

// for (auto &t:thr) t.join();
