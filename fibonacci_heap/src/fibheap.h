#ifndef FIBHEAP_H
#define FIBHEAP_H

#include <cmath>
#include <vector>
#include <limits>

const double GOLDEN_RATIO = (1 + sqrt(5.0))/2;
const double LOG_GOLDEN_RATION = log(GOLDEN_RATIO);

template <typename T>
class NegativeInfinity {
public:
	static T get() {
		return std::numeric_limits<T>::min();
	}
};

template <typename Key>
class FibonacciHeap {
public:
	struct Node {
		Node* parent;
		Node* left;
		Node* right;
		Node* children;
		int degree;
		bool isMarked;
		Key key;

		Node(const Key& key)
			: degree(0), isMarked(false), parent(0), left(0), right(0), children(0), key(key) {
		}
	};

	Node* min;
	size_t size;

	void insertInList(Node*& list, Node* node) {
		if (list == 0) {
			node->left = node->right = node;
			list = node;
		} else {
			list->right->left = node;
			node->right = list->right;
			list->right = node;
			node->left = list;
		}
	}

	void extractFromList(Node*& list, Node* node) {
		if (node->right == node) {
			list = 0;
		} else {
			node->left->right = node->right;
			node->right->left = node->left;
			list = node->right;
		}
	}

	void merge(Node* first, Node* second) {
		Node* temp = first->right;
		first->right = second->right;
		second->right->left = first;
		temp->left = second;
		second->right = temp;
	}

	void insertInRootList(Node* node) {
		insertInList(min, node);
		if (node->key < min->key) {
			min = node;
		}
	}

	void consolidate() {
		int maxD = static_cast<int>(log(static_cast<double>(size))/LOG_GOLDEN_RATION) + 1;
		std::vector<Node*> degreeToNode(maxD, 0);

		min->left->right = 0;
		min->left = 0;
		Node* currentRootNode = min;
		while (currentRootNode != 0) {
			Node* current = currentRootNode;
			currentRootNode = currentRootNode->right;

			while(degreeToNode[current->degree] != 0) {
				Node* sameDegreeNode = degreeToNode[current->degree];
				
				if (sameDegreeNode->key < current->key) {
					std::swap(current, sameDegreeNode);
				}
				insertInList(current->children, sameDegreeNode);
				sameDegreeNode->parent = current;
				current->degree++;
				sameDegreeNode->isMarked = false;

				degreeToNode[sameDegreeNode->degree] = 0;
			}

			current->parent = 0;
			degreeToNode[current->degree] = current;
		}

		min = 0;
		for (size_t i = 0; i < degreeToNode.size(); i++) {
			if (degreeToNode[i] != 0) {
				insertInRootList(degreeToNode[i]);
			}
		}
	}

	void cutNode(Node* node) {
		Node* parent = node->parent;

		extractFromList(parent->children, node);
		parent->degree--;
		node->parent = 0;
		node->isMarked = false;
		insertInRootList(node);

		if (parent->parent != 0) {
			if (parent->isMarked) {
				cutNode(parent);
			} else {
				parent->isMarked = true;
			}
		}
	}

	void deleteList(Node* list) {
		if (list != 0) {
			list->left->right = 0;
			list->left = 0;

			Node* current = list;
			while (current != 0) {
				deleteList(current->children);
				Node* temp = current->right;
				delete current;
				current = temp;
			}
		}
	}

	void copyFromList(Node* list) {
		if (list != 0) {
			Node* current = list;

			while (current->right != list) {
				copyFromList(current->children);
				insert(current->key);
			}
		}
	}

public:
	class Element {
		Node* node;
		
		Element(Node* node)
			: node(node) {
		}
	public:
		const Key& getKey() {
			return node->key;
		}

	friend class FibonacciHeap;
	};

	FibonacciHeap()
		: min(0), size(0) {
	}

	~FibonacciHeap() {
		deleteList(min);
	}

	FibonacciHeap(const FibonacciHeap& heap)
		: min(0), size(0) {
			copyFromList(heap.min);
	}

	void swap(FibonacciHeap& heap) {
		std::swap(min, heap.min);
		std::swap(size, heap.size);
	}

	FibonacciHeap& operator=(const FibonacciHeap& heap) {
		if (this != &heap) {
			FibonacciHeap temp(heap);
			swap(temp);
		}

		return *this;
	}

	Element insert(const Key& key) {
		Node* newNode = new Node(key);

		insertInRootList(newNode);
		size++;

		return Element(newNode);
	}

	Element getMinElement() const {
		return Element(min);
	}

	const Key& getMin() const {
		return min->key;
	}

	void mergeWith(FibonacciHeap<Key>& heap) {
		if (min == 0) {
			min = heap.min;
		} else if (heap.min != 0) {
			merge(min, heap.min);
			if (heap.min->key < min->key) {
				min = heap.min;
			}
		}

		size += heap.size;

		heap.min = 0;
		heap.size = 0;
	}

	bool isEmpty() const {
		return min == 0;
	}

	size_t getSize() const {
		return size;
	}

	void extractMin() {
		if (min->children != 0) {
			Node* currentChild = min->children;
			while (currentChild->right != min->children) {
				currentChild->parent = 0;
				currentChild->isMarked = false;

				currentChild = currentChild->right;
			}

			merge(min, min->children);
			min->children = 0;
		}

		Node* temp = min;
		extractFromList(min, temp);
		delete temp;
		size--;

		if (min != 0) {
			consolidate();
		}
	}

	void decreaseKey(Element element, const Key& newKey) {
		Node* node = element.node;

		node->key = newKey;

		if (node->parent != 0) {
			if (node->key < node->parent->key) {
				cutNode(node);
			}
		} else if (newKey < min->key) {
			min = node;
		}
	}

	void remove(Element element) {
		decreaseKey(element, NegativeInfinity<Key>::get());
		extractMin();
	}
};

#endif
