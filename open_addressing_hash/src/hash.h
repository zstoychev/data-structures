#ifndef HASH_H
#define HASH_H

#include <functional>
#include <string>
#include <vector>
using std::size_t;

const int CAPACITIES[] = {
	2,
	5,
	11,
	17,
	37,
	67,
	131,
	257,
	521,
	1031,
	2053,
	4099,
	8209,
	16411,
	32771,
	65537,
	131101,
	262147,
	524309,
	1048583,
	2097169
	// ...
};

size_t hashCode(int i) {
	return (size_t) i;
}

size_t hashCode(const std::string& str) {
	size_t h = 0;

	for (size_t i = 0; i < str.size(); i++) {
		h = h * 31 + str[i];
	}

	return h;
}

template<typename T> 
struct defaulthash : public std::unary_function<T, size_t> {
  size_t operator()(T const& key) const {
	  return hashCode(key);
  }
};

template <typename Key, typename Value, typename HashFunction = defaulthash<Key>, typename EqualityPredicate = std::equal_to<Key> >
class Hash {
public:
	struct Entry {
		const Key key;
		Value value;

		Entry(const Key& key, const Value& value)
			: key(key), value(value) {
		}
	};

	class Iterator {
	public:
		Entry& operator*() {
			return hash->hashtable[index];
		}

		Entry* operator->() {
			return hash->hashtable + index;
		}

		Iterator& operator++() {
			do {
				index++;
			} while (index < hash->getCapacity() && hash->entriesState[index] != OCCUPIED);

			return *this;
		}

		Iterator operator++(int) const {
			return ++Iterator(*this);
		}

		bool operator==(const Iterator& iterator) const {
			return hash == iterator.hash && index == iterator.index;
		}

		bool operator!=(const Iterator& iterator) const {
			return !(*this == iterator);
		}
	private:
		Hash<Key, Value>* hash;
		size_t index;

		Iterator(Hash<Key, Value>& hash, size_t index)
			: hash(&hash), index(index) {
		}

		friend class Hash;
		// friend class ConstIterator;
	};

	// ConstIterator should be implemented in similar way

	Hash(const HashFunction& hash = HashFunction(), const EqualityPredicate& equal = EqualityPredicate(), float maxLoadFactor = 0.75)
		: hash(hash), equal(equal), size(0), capacityIndex(2), entriesState(CAPACITIES[2], EMPTY), maxLoadFactor(maxLoadFactor) {
		hashtable = static_cast<Entry*>(operator new[] (sizeof(Entry) * getCapacity()));
	}

	Hash(const Hash& h2) 
		: hash(h2.hash), equal(h2.equal), size(0), capacityIndex(2), entriesState(CAPACITIES[2], EMPTY), maxLoadFactor(h2.maxLoadFactor) {
		hashtable = static_cast<Entry*>(operator new[] (sizeof(Entry) * getCapacity()));

		for (size_t i = 0; i < h2.getCapacity(); i++) {
			if (h2.entriesState[i] == OCCUPIED) {
				put(h2.hashtable[i].key, h2.hashtable[i].value);
			}
		}
	}

	~Hash() {
		for (size_t i = 0; i < getCapacity(); i++) {
			if (entriesState[i] == OCCUPIED) {
				hashtable[i].~Entry();
			}
		}

		operator delete[](hashtable);
	}

	void put(const Key& key, const Value& value) {
		size_t keyHash = hash(key);

		Iterator i = get(key, keyHash);

		if (i != end()) {
			i->value = value;
		} else {
			size_t primaryHash = getPrimaryHash(keyHash);
			size_t index = primaryHash;

			if (entriesState[index] == OCCUPIED) {
				size_t secondaryHash = getSecondaryHash(keyHash);

				do {
					index = (index + secondaryHash) % getCapacity();
				} while (entriesState[index] == OCCUPIED);
			}

			addEntry(key, value, index);
		}
	}

	Iterator get(const Key& key) {
		return get(key, hash(key));
	}

	bool remove(const Key& key) {
		Iterator i = get(key);

		if (i != end()) {
			hashtable[i.index].~Entry();
			size--;
			entriesState[i.index] = DELETED;

			return true;
		} else {
			return false;
		}
	}

	Iterator begin() {
		return Iterator(*this, 0);
	}

	Iterator end() {
		return Iterator(*this, getCapacity());
	}

	size_t getSize() const {
		return size;
	}

	bool isEmpty() const {
		return size == 0;
	}

	size_t getCapacity() const {
		return CAPACITIES[capacityIndex];
	}

	void swap(Hash& h2) {
		std::swap(hash, h2.hash);
		std::swap(equal, h2.equal);
		std::swap(hashtable, h2.hashtable);
		entriesState.swap(h2.entriesState);
		std::swap(size, h2.size);
		std::swap(capacityIndex, h2.capacityIndex);
		std::swap(maxLoadFactor, h2.maxLoadFactor);
	}

	Hash& operator=(const Hash& h2) {
		if (this != &h2) {
			Hash temp(h2);
			swap(temp);
		}

		return *this;
	}
private:
	enum EntryState {EMPTY, DELETED, OCCUPIED};

	size_t getPrimaryHash(size_t key) {
		return key % getCapacity();
	}
	
	size_t getSecondaryHash(size_t key) {
		return 1 + (key % (getCapacity() - 1));
	}

	Iterator get(const Key& key, size_t keyHash) {
		size_t primaryHash = getPrimaryHash(keyHash);
		size_t index = primaryHash;

		if (entriesState[index] == DELETED || (entriesState[index] == OCCUPIED && !equal(key, hashtable[index].key)))  {
			size_t secondaryHash = getSecondaryHash(keyHash);

			do {
				index = (index + secondaryHash) % getCapacity();
				if (index == primaryHash) {
					return end();
				}
			} while (entriesState[index] == DELETED || (entriesState[index] == OCCUPIED && !equal(key, hashtable[index].key)));
		}

		if (entriesState[index] == OCCUPIED) {
			return Iterator(*this, index);
		} else {
			return end();
		}
	}

	void checkLoadFactor() {
		if (static_cast<float>(size) / getCapacity() >= maxLoadFactor) {
			Entry* oldHashTable = hashtable;
			std::vector<EntryState> oldEntriesState(entriesState);
			size_t oldCapacity = getCapacity();

			capacityIndex++;
			size = 0;

			hashtable = static_cast<Entry*>(operator new[] (sizeof(Entry) * getCapacity()));
			entriesState = std::vector<EntryState>(getCapacity(), EMPTY);

			for (size_t i = 0; i < oldCapacity; i++) {
				if (oldEntriesState[i] == OCCUPIED) {
					put(oldHashTable[i].key, oldHashTable[i].value);
					oldHashTable[i].~Entry();
				}
			}

			operator delete[](oldHashTable);
		}
	}

	void addEntry(const Key& key, const Value& value, size_t index) {
		new (hashtable + index) Entry(key, value);
		entriesState[index] = OCCUPIED;
		size++;
		checkLoadFactor();
	}

	HashFunction hash;
	EqualityPredicate equal;
	Entry* hashtable;
	std::vector<EntryState> entriesState;
	size_t size;
	size_t capacityIndex;
	float maxLoadFactor;
};

#endif
