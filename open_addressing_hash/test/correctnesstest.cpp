#include "hash.h"
#include <string>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

vector<pair<int, int> > ints;
map<int, int> uniqueInts;
vector<pair<string, string> > strings;


void testAdd() {
	Hash<int, int> h;

	h.put(42, 11);

	if (h.get(42) == h.end()) {
		cout << "testAdd failed\n";
	}
}

void testReplace() {
	Hash<int, int> h;

	h.put(42, 11);
	h.put(32, 10);
	h.put(90, 3);
	h.put(42, 9);
	h.put(50, 3);

	if (h.get(42)->value != 9) {
		cout << "testReplace failed\n";
	}
}

void testRemove() {
	Hash<int, int> h;

	for (vector<pair<int, int> >::const_iterator i = ints.begin(); i != ints.begin() + ints.size() / 2; ++i) {
		h.put(i->first, i->second);
	}

	for (vector<pair<int, int> >::const_iterator i = ints.begin(); i != ints.begin() + ints.size() / 2; ++i) {
		h.remove(i->first);
	}

	if (!h.isEmpty()) {
		cout << "testRemove failed\n";
	}

	for (vector<pair<int, int> >::const_iterator i = ints.begin(); i != ints.end(); ++i) {
		h.put(i->first, i->second);
	}

	for (map<int, int>::const_iterator i = uniqueInts.begin(); i != uniqueInts.end(); ++i) {
		if (h.get(i->first)->value != i->second) {
			cout << "Fail on remove test with number " << i->first << ". Expected: " << i->second << " Got: " << h.get(i->first)->value << endl;
		}
	}
}

void testCopy() {
	Hash<string, string> h;

	for (vector<pair<string, string> >::const_iterator i = strings.begin(); i != strings.end(); ++i) {
		h.put(i->first, i->second);
	}

	Hash<string, string> h2;
	h2 = h;

	for (vector<pair<string, string> >::const_iterator i = strings.begin(); i != strings.end(); ++i) {
		if (h2.get(i->first)->value != i->second) {
			cout << "Fail on copy test with string " << i->first << ". Expected: " << i->second << " Got: " << h.get(i->first)->value << endl;
		}
	}
}

void testManyInts() {
	Hash<int, int> h;

	for (vector<pair<int, int> >::const_iterator i = ints.begin(); i != ints.end(); ++i) {
		h.put(i->first, i->second);
	}

	for (map<int, int>::const_iterator i = uniqueInts.begin(); i != uniqueInts.end(); ++i) {
		if (h.get(i->first)->value != i->second) {
			cout << "Fail on int test with number " << i->first << ". Expected: " << i->second << " Got: " << h.get(i->first)->value << endl;
		}
	}
}

void testManyStrings() {
	Hash<string, string> h;

	for (vector<pair<string, string> >::const_iterator i = strings.begin(); i != strings.end(); ++i) {
		h.put(i->first, i->second);
	}

	for (vector<pair<string, string> >::const_iterator i = strings.begin(); i != strings.end(); ++i) {
		if (h.get(i->first)->value != i->second) {
			cout << "Fail on string test with string " << i->first << ". Expected: " << i->second << " Got: " << h.get(i->first)->value << endl;
		}
	}
}

int main() {
	std::ifstream stringInput("moby-hash.txt");

	while(stringInput) {
		string key;
		string value;
		getline(stringInput, key);
		getline(stringInput, value);
		if (stringInput) {
			strings.push_back(make_pair(key, value));
			
		}
	}

	for (int i = 0; i < 1000000; i++) {
		int a = rand();
		int b = rand();
		ints.push_back(make_pair(a, b));
		uniqueInts[a] = b;
	}

	testAdd();
	testReplace();
	testRemove();
	testCopy();
	testManyInts();
	testManyStrings();

	return 0;
};
