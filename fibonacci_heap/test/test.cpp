#include "fibheap.h "
#include <cstdlib>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

void testInsertAndExtract() {
	priority_queue<int> pq;
	FibonacciHeap<int> fh;

	for (int j = 0; j < 100; j++) {
		for (int i = 0; i < 10000 ; i++) {
			int key = rand();

			pq.push(-key);
			fh.insert(key);
		}

		for (int i = 0; i < 5000 ; i++) {
			if (-pq.top() != fh.getMin()) {
				cout << "Fail" << endl;
			}

			pq.pop();
			fh.extractMin();
		}
	}

	if (fh.getSize() != 500000) {
		cout << "Fail" << endl;
	}
}

void testDecreaseKey() {
	priority_queue<int> pq;
	FibonacciHeap<int> fh;

	for (int j = 0; j < 100; j++) {
		vector<pair<FibonacciHeap<int>::Element, int> > decreaseTo;

		for (int i = 0; i < 10000 ; i++) {
			int key = rand() + 1;

			if (rand() % 2 == 0) {
				fh.insert(key);
				pq.push(-key);
			} else {
				int decr = rand() % key;

				pq.push(-decr);
				decreaseTo.push_back(make_pair(fh.insert(key), decr));
			}
		}

		for (size_t i = 0; i < decreaseTo.size(); i++) {
			fh.decreaseKey(decreaseTo[i].first, decreaseTo[i].second);
		}
	}

	while (!pq.empty()) {
		if (-pq.top() != fh.getMin()) {
			cout << "Fail"<< endl;
		}

		pq.pop();
		fh.extractMin();
	}

	if (!fh.isEmpty()) {
		cout << "Fail" << endl;
	}
}

void testDelete() {
	priority_queue<int> pq;
	FibonacciHeap<int> fh;

	for (int j = 0; j < 100; j++) {
		vector<FibonacciHeap<int>::Element> toBeDeleted;

		for (int i = 0; i < 10000 ; i++) {
			int key = rand();

			if (rand() % 2 == 0) {
				fh.insert(key);
				pq.push(-key);
			} else {
				toBeDeleted.push_back(fh.insert(key));
			}
		}

		for (size_t i = 0; i < toBeDeleted.size(); i++) {
			fh.remove(toBeDeleted[i]);
		}
	}

	while (!pq.empty()) {
		if (-pq.top() != fh.getMin()) {
			cout << "Fail"<< endl;
		}

		pq.pop();
		fh.extractMin();
	}

	if (!fh.isEmpty()) {
		cout << "Fail" << endl;
	}
}

int main2() {
	testInsertAndExtract();
	testDecreaseKey();
	testDelete();
}
