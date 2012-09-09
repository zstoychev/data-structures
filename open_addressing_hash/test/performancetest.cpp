#include "hash.h"
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

vector<pair<int, int> > ints;
vector<pair<string, string> > strings;
const int STRING_TEST_SIZES[] = {5, 50, 250, 500, 1000, 15733};
const int INT_TEST_SIZES[] = {5, 50, 250, 500, 1000, 10000, 100000, 1000000};
const float TESTED_LOAD_FACTORS[] = {0.25f, 0.50f, 0.60f, 0.65f, 0.70f, 0.75f, 0.80f, 0.85f, 0.90f, 0.95f};
const int ITERATIONS = 64;

struct StringWithPrecomputedHash {
	const string* str;
	mutable size_t hash;

	StringWithPrecomputedHash(const string* str);

	bool operator==(const StringWithPrecomputedHash& s2) const {
		return *str == *s2.str;
	}
};

size_t hashCode(const StringWithPrecomputedHash& str) {
	if (str.hash == 0) {
		str.hash = hashCode(*str.str);
	}

	return str.hash;
}

StringWithPrecomputedHash::StringWithPrecomputedHash(const string* str)
	 : str(str), hash(0) {
	 hash = hashCode(*this);
}


vector<pair<StringWithPrecomputedHash, string> > advancedStrings;

void testInts(size_t count, float loadFactor) {
	clock_t initial = clock();

	for (int j = 0; j < ITERATIONS; j++) {
		Hash<int, int> h(defaulthash<int>(), equal_to<int>(), loadFactor);

		vector<pair<int, int> >::const_iterator i = ints.begin();
		for (size_t k = 0; k < count && i != ints.end(); ++k, ++i) {
			h.put(i->first, i->second);
		}

		i = ints.begin();
		for (size_t k = 0; k < count && i != ints.end(); ++k, ++i) {
			h.get(i->first);
		}
	}
		
	clock_t current = clock();

	cout << loadFactor << "lf " << (current - initial) << "ms" << endl;
}

void testStrings(size_t count, float loadFactor) {
	clock_t initial = clock();

	for (int j = 0; j < ITERATIONS; j++) {
		Hash<string, string> h(defaulthash<std::string>(), equal_to<std::string>(), loadFactor);

		vector<pair<string, string> >::const_iterator i = strings.begin();
		for (size_t k = 0; k < count && i != strings.end(); ++k, ++i) {
			h.put(i->first, i->second);
		}

		i = strings.begin();
		for (size_t k = 0; k < count && i != strings.end(); ++k, ++i) {
			h.get(i->first);
		}
	}
		
	clock_t current = clock();

	cout << loadFactor << "lf " << (current - initial) << "ms" << endl;
}

void testStringsWithPrecomputedHash(size_t count, float loadFactor) {
	clock_t initial = clock();

	for (int j = 0; j < ITERATIONS; j++) {
		Hash<StringWithPrecomputedHash, string> h(defaulthash<StringWithPrecomputedHash>(), equal_to<StringWithPrecomputedHash>(), loadFactor);
		
		vector<pair<StringWithPrecomputedHash, string> >::const_iterator i = advancedStrings.begin();
		for (size_t k = 0; k < count && i != advancedStrings.end(); ++k, ++i) {
			h.put(i->first, i->second);
		}
		i = advancedStrings.begin();
		for (size_t k = 0; k < count && i != advancedStrings.end(); ++k, ++i) {
			h.get(i->first);
		}
	}
		
	clock_t current = clock();

	cout << loadFactor << "lf " << (current - initial) << "ms" << endl;
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

	for (size_t i = 0; i < strings.size(); i++) {
		advancedStrings.push_back(make_pair(StringWithPrecomputedHash(&strings[i].first), strings[i].second));
	}

	for (int i = 0; i < 1000000; i++) {
		ints.push_back(make_pair(rand(), rand()));
	}

	cout << "Testing ints: " << endl;
	for (int i = 0; i < sizeof(INT_TEST_SIZES)/sizeof(INT_TEST_SIZES[0]); i++) {
		cout << "Testing with size of " << INT_TEST_SIZES[i] << endl;

		for (int j = 0; j < sizeof(TESTED_LOAD_FACTORS)/sizeof(TESTED_LOAD_FACTORS[0]); j++) {
			testInts(INT_TEST_SIZES[i], (TESTED_LOAD_FACTORS[j]));
		}

		cout << endl;
	}

	cout << "Testing strings: " << endl;
	for (int i = 0; i < sizeof(STRING_TEST_SIZES)/sizeof(STRING_TEST_SIZES[0]); i++) {
		cout << "Testing with size of " << STRING_TEST_SIZES[i] << endl;

		for (int j = 0; j < sizeof(TESTED_LOAD_FACTORS)/sizeof(TESTED_LOAD_FACTORS[0]); j++) {
			testStrings(STRING_TEST_SIZES[i], (TESTED_LOAD_FACTORS[j]));
		}

		cout << endl;
	}

	cout << "Testing string with precomputed hash: " << endl;
	for (int i = 0; i < sizeof(STRING_TEST_SIZES)/sizeof(STRING_TEST_SIZES[0]); i++) {
		cout << "Testing with size of " << STRING_TEST_SIZES[i] << endl;

		for (int j = 0; j < sizeof(TESTED_LOAD_FACTORS)/sizeof(TESTED_LOAD_FACTORS[0]); j++) {
			testStringsWithPrecomputedHash(STRING_TEST_SIZES[i], (TESTED_LOAD_FACTORS[j]));
		}

		cout << endl;
	}

	return 0;
};
