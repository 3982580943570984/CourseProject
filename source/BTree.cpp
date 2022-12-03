#include "include/BTree.hpp"
#include "include/BTreeNode.hpp"

#include <queue>

BTree::BTree(std::int32_t order) : order(order) {};

BTree::~BTree() noexcept {
	if (root != nullptr) {
		std::queue<BTreeNode*> nodes;
		nodes.push(root);
		while (!nodes.empty()) {
			auto node = nodes.front();
			nodes.pop();
			for (auto child : node->childs)
				if (child != nullptr) 
					nodes.push(child);
			delete node;
		}
	}
};

void BTree::insert (std::int32_t value) {
	// If tree is empty
	if (root == nullptr) {
		// Create root
		root = new BTreeNode(order, true);
		// Add value
		root->keys.push_back(value);
		return;
	}

	// If tree isn't empty and root isn't full
	if (!root->isFull()) {
		root->insert(value);
		return;
	}

	// If tree isn't empty and root full
	if (root->isFull()) {
		BTreeNode* newRoot = new BTreeNode(order, false);
		newRoot->childs[0] = root;
		newRoot->split(0, root);
		newRoot->keys.front() < value
			? newRoot->childs[1]->insert(value)
			: newRoot->childs[0]->insert(value);
		root = newRoot;
		return;
	}
};

void BTree::remove (std::int32_t value) {
	if (root != nullptr) {
		root->remove(value);
		if (root->keys.empty()) {
			auto del = root;
			if (root->leaf)
				root = nullptr;
			else
				root = root->childs[0];
			delete del;
		}
	}
};

void BTree::modify (std::int32_t elem, std::int32_t value) {
	remove(elem);
	insert(value);
};

bool BTree::search (std::int32_t value) {
	if (root != nullptr)
		return root->search(value);

	return false;
};

void BTree::traverse () {
	if (root != nullptr)
		root->traverse();
};

void BTree::visualize() {

};
