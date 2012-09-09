#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include <exception>

template <typename Key, typename Value>
class RedBlackTree {
	struct Node {
		Key key;
		Value value;
		bool isRed;
		Node* links[2];

		static const int LEFT_INDEX = 0;
		static const int RIGHT_INDEX = 1;
		static int opposite(int dirIndex) {
			if (dirIndex == LEFT_INDEX) {
				return RIGHT_INDEX;
			} else {
				return LEFT_INDEX;
			}
		}

		Node(const Key& key, const Value& value)
			: key(key), value(value), isRed(true) {
				links[LEFT_INDEX] = 0;
				links[RIGHT_INDEX] = 0;
		}
	};

	Node* root;

	bool isRed(const Node* node) const {
		return node != 0 && node->isRed;
	}

	void rotate(Node*& root, int dirIndex) const {
		Node* newRoot = root->links[Node::opposite(dirIndex)];

		root->links[Node::opposite(dirIndex)] = newRoot->links[dirIndex];
		newRoot->links[dirIndex] = root;

		root->isRed = true;
		newRoot->isRed = false;

		root = newRoot;
	}

	void insert(Node*& root, const Key& key, const Value& value) {
		if (root == 0) {
			root = new Node(key, value);
		} else if (key == root->key) {
			root->value = value;
		} else {
			int dirIndex = key < root->key ? Node::LEFT_INDEX : Node::RIGHT_INDEX;

			insert(root->links[dirIndex], key, value);
			if (isRed(root->links[dirIndex])) {
				if (isRed(root->links[Node::opposite(dirIndex)])) {
					root->isRed = true;
					root->links[Node::LEFT_INDEX]->isRed = false;
					root->links[Node::RIGHT_INDEX]->isRed = false;
				} else if (isRed(root->links[dirIndex]->links[dirIndex])) {
					rotate(root, Node::opposite(dirIndex));
				} else if (isRed(root->links[dirIndex]->links[Node::opposite(dirIndex)])) {
					rotate(root->links[dirIndex], dirIndex);
					rotate(root, Node::opposite(dirIndex));
				}
			}
		}
	}

	void remove(Node*& root, const Key& key) {
		Node temp(root->key, root->value);
		temp.isRed = false;
		Node* current;
		Node* parent;
		Node* grandparent;
		Node* founded = 0;
		int dirIndex = Node::RIGHT_INDEX;
		int lastDirIndex;

		current = &temp;
		current->links[dirIndex] = root;
		grandparent = parent = 0;

		while (current->links[dirIndex] != 0) {
			grandparent = parent;
			parent = current;
			current = current->links[dirIndex];
			lastDirIndex = dirIndex;
			dirIndex = current->key < key ? Node::RIGHT_INDEX : Node::LEFT_INDEX;

			if (current->key == key) {
				founded = current;
			}

			if (!isRed(current) && !isRed(current->links[dirIndex])) {
				if (isRed(current->links[Node::opposite(dirIndex)])) {
					Node* tempCurrent = current;
					rotate(tempCurrent, dirIndex);
					parent = parent->links[lastDirIndex] = tempCurrent;
				} else {
					Node* sibling = parent->links[Node::opposite(lastDirIndex)];

					if (sibling != 0) {
						if (!isRed(sibling->links[Node::LEFT_INDEX]) && !isRed(sibling->links[Node::RIGHT_INDEX])) {
							parent->isRed = false;
							sibling->isRed = true;
							current->isRed = true;
						} else {
							int beforeLastDirIndex = grandparent->links[Node::LEFT_INDEX] == parent ? Node::LEFT_INDEX : Node::RIGHT_INDEX;
						
							if (isRed(sibling->links[lastDirIndex])) {
								Node* tempParent = parent;
								rotate(tempParent->links[Node::opposite(lastDirIndex)], Node::opposite(lastDirIndex));
								rotate(tempParent, lastDirIndex);
								grandparent->links[beforeLastDirIndex] = tempParent;
							} else if (isRed(sibling->links[Node::opposite(lastDirIndex)])) {
								Node* tempParent = parent;
								rotate(tempParent, lastDirIndex);
								grandparent->links[beforeLastDirIndex] = tempParent;
							}

							current->isRed = grandparent->links[beforeLastDirIndex]->isRed = true;
							grandparent->links[beforeLastDirIndex]->links[Node::LEFT_INDEX]->isRed = false;
							grandparent->links[beforeLastDirIndex]->links[Node::RIGHT_INDEX]->isRed = false;
						}
					}
				}
			}
		}

		founded->key = current->key;
		founded->value = current->value;
		parent->links[parent->links[Node::RIGHT_INDEX] == current ? Node::RIGHT_INDEX : Node::LEFT_INDEX] = current->links[current->links[Node::LEFT_INDEX] == 0 ? Node::RIGHT_INDEX : Node::LEFT_INDEX];
		delete current;

		root = temp.links[Node::RIGHT_INDEX];
		if (root != 0) {
			root->isRed = false;
		}
	}

	void deleteTree(Node*& root) const {
		if (root == 0) {
			return;
		}
		deleteTree(root->links[Node::LEFT_INDEX]);
		deleteTree(root->links[Node::RIGHT_INDEX]);
		delete root;
	}

	void buildFromTree(Node*& root, Node*& from) {
		if (from != 0) {
			root = new Node(from->key, from->value);

			buildFromTree(root->left, from->left);
			buildFromTree(root->right, from->right);
		}
	}

public:
	RedBlackTree()
		: root(0) {
	}

	RedBlackTree(const RedBlackTree& redBlackTree) 
		:root(0) {
			buildFromTree(root, redBlackTree.root);
	}

	~RedBlackTree() {
		deleteTree(root);
	}

	void swap(RedBlackTree& tree) {
		std::swap(root, tree->root);
	}

	RedBlackTree& operator=(const RedBlackTree& tree) {
		if (this != &tree) {
			RedBlackTree temp(tree);
			swap(temp);
		}

		return *this;
	}

	bool isEmpty() const {
		return root == 0;
	}

	void put(const Key& key, const Key& value) {
		insert(root, key, value);
		root->isRed = false;
	}

	void remove(const Key& key) {
		if (!contains(key)) {
			throw std::exception();
		}
		remove(root, key);
	}

	Value get(const Key& key) {
		Node* current = root;

		while(current != 0 && current->key != key) {
			current = current->links[key < current->key ? Node::LEFT_INDEX : Node::RIGHT_INDEX];
		}

		if (current == 0) {
			throw std::exception();
		}

		return current->value;
	}

	bool contains(const Key& key) {
		Node* current = root;

		while(current != 0 && current->key != key) {
			current = current->links[key < current->key ? Node::LEFT_INDEX : Node::RIGHT_INDEX];
		}

		return current != 0;
	}
};

#endif
