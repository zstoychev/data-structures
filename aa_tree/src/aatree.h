#ifndef AATREE_H
#define AATREE_H

#include <exception>

template <typename Key, typename Value>
class AATree {
	struct Node {
		Node* left;
		Node* right;
		Key key;
		Value value;
		int level;

		Node(const Key& key, const Value& value)
			: key(key), value(value), left(0), right(0), level(1) {
		}
	};

	Node* root;

	void skew(Node*& root) {
		if (root != 0 && root->left != 0 &&
			root->level == root->left->level) {
			Node* newRoot = root->left;
			root->left = newRoot->right;
			newRoot->right = root;
			root = newRoot;
		}
	}

	void split(Node*& root) {
		if (root != 0 && root->right != 0 && root->right->right != 0 &&
			root->level == root->right->right->level) {
			Node* newRoot = root->right;
			root->right = newRoot->left;
			newRoot->left = root;
			newRoot->level++;
			root = newRoot;
		}
	}

	void insert(Node*& root, const Key& key, const Value& value) {
		if (root == 0) {
			root = new Node(key, value);

			return;
		}

		if (key < root->key) {
			insert(root->left, key, value);
		} else if (key > root->key) {
			insert(root->right, key, value);
		} else {
			root->value = value;
		}

		skew(root);
		split(root);
	}

	int nlevel(Node*& node) {
		if (node == 0) {
			return 0;
		} else {
			return node->level;
		}
	}

	void remove(Node*& root, const Key& key) {
		if (key < root->key) {
			remove(root->left, key);
		} else if (key > root->key) {
			remove(root->right, key);
		} else {
			if (root->left != 0 && root->right != 0) {
				Node* predecessor = root->left;
				while (predecessor->right != 0) {
					predecessor = predecessor->right;
				}

				root->value = predecessor->value;

				remove(root->left, root->value);
			} else {
				Node* heir;
				if (root->left == 0) {
					heir = root->right;
				} else {
					heir = root->left;
				}

				delete root;
				root = heir;
				return;
			}
		}

		int newLevel = std::min(nlevel(root->left), nlevel(root->right)) + 1;
		if (newLevel < root->level) {
			root->level = newLevel;
			
			if (newLevel < nlevel(root->right)) {
				root->right->level = newLevel;
			}

			skew(root);
			skew(root->right);
			if (root->right != 0) {
				skew(root->right->right);
			}
			split(root);
			split(root->right);
		}
	}

	void deleteTree(Node*& root) const {
		if (root == 0) {
			return;
		}

		deleteTree(root->left);
		deleteTree(root->right);

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
	AATree()
		: root(0) {
	}

	AATree(const AATree& aaTree) 
		:root(0) {
			buildFromTree(root, aaTree.root);
	}

	~AATree() {
		deleteTree(root);
	}

	void swap(AATree& tree) {
		std::swap(root, tree->root);
	}

	AATree& operator=(const AATree& tree) {
		if (this != &tree) {
			AATree temp(tree);
			swap(temp);
		}

		return *this;
	}

	bool isEmpty() const {
		return root == 0;
	}

	void put(const Key& key, const Key& value) {
		insert(root, key, value);
	}

	void remove(const Key& key) {
		if (!contains(key)) {
			throw std::exception();
		}
		remove(root, key);
	}

	Value get(const Key& key) {
		Node* current = root;

		while (current != 0 && key != current->key) {
			if (key < current->key) {
				current = current->left;
			} else {
				current = current->right;
			}
		}

		if (current == 0) {
			throw std::exception();
		}

		return current->value;
	}

	bool contains(const Key& key) {
		Node* current = root;

		while (current != 0 && key != current->key) {
			if (key < current->key) {
				current = current->left;
			} else {
				current = current->right;
			}
		}

		return current != 0;
	}
};

#endif
