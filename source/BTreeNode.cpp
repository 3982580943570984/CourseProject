#include "include/BTreeNode.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>
#include <utility>

BTreeNode::BTreeNode(std::int32_t order, bool leaf) : order(order), leaf(leaf) {
	childs = std::vector<BTreeNode*>(order);
};

void BTreeNode::insert(std::int32_t value) {
	std::int32_t place = keys.size() - 1;
	if (!keys.empty())
		while (place >= 0 && keys[place] > value)
			--place;

	if (leaf) {
		if (!keys.empty())
			keys.insert(keys.cbegin() + (place + 1), value);
		else
			keys.push_back(value);
	}
	else {
		if (childs[place + 1]->isFull()) {
			split(place + 1, childs[place + 1]);
			// Decide which of the two childs will have new value
			if (keys[place + 1] < value)
				++place;
		}

		childs[place + 1]->insert(value);
	}
};

void BTreeNode::split(std::uint32_t link, BTreeNode* child) {
	BTreeNode* newChild = new BTreeNode(child->order, child->leaf);

	// Insert median key from child to parent node
	keys.insert(keys.cbegin() + link, child->keys[(order >> 1) - 1]);

	// Split child keys with new node
	// [median + 1, end)
	newChild->keys = std::vector<int>{ child->keys.cbegin() + (order >> 1), child->keys.cend() };
	// [start, median)
	child->keys = std::vector<int>{ child->keys.cbegin() , child->keys.cbegin() + (order >> 1) - 1 };

	if (!child->leaf) {
		for (std::size_t i { 0 }; i < order / 2; ++i) {
			newChild->childs[i] = child->childs[i + (order >> 1)];
			child->childs[i + (order >> 1)] = nullptr;
		}
	}

	childs[link + 1] = newChild;
};

void BTreeNode::remove(std::int32_t value) {
	std::int32_t index = 0;
	while (index < keys.size() && keys[index] < value)
		++index;

	// Value to be removed is present in current node
	if (index < keys.size() && keys[index] == value) {
		if (leaf) {
			std::cout << "\n---leaf---\n" << std::flush;
			keys.erase(keys.cbegin() + index);
		} else {
			// Borrow max value from left sub-tree
			if (childs[index]->keys.size() >= (order >> 1)) {
				std::int32_t predecessor = 0;

				auto cur = childs[index];
				while (!cur->leaf)
					cur = cur->childs.back();

				predecessor = cur->keys.back();

				keys[index] = predecessor;
				childs[index]->remove(predecessor);

				return;
			}

			// Borrow min value from right sub-tree
			if (childs[index + 1]->keys.size() >= (order >> 1)) {
				std::int32_t successor = 0;

				auto cur = childs[index + 1];
				while (!cur->leaf)
					cur = cur->childs.front();

				successor = cur->keys.front();

				keys[index] = successor;
				childs[index + 1]->remove(successor);

				return;
			}

			std::int32_t key = keys[index];
			merge(index);
			childs[index]->remove(key);

		}
	} else {
		// Value doesn't exist in tree
		if (leaf)
			return;

		// If last child of this node contains value
		bool flag = (index == keys.size() ? true : false);

		if (childs[index]->keys.size() < (order >> 1))
			fill(index);

		if (flag && index > keys.size())
			childs[index - 1]->remove(value);
		else
			childs[index]->remove(value);
	}
}

void BTreeNode::fill(std::int32_t index) {

	// Borrow key from previous child
	if (index != 0 && childs[index - 1]->keys.size() >= (order >> 1)) {
		auto current = childs[index];
		auto previous = childs[index - 1];

		current->keys.insert(current->keys.cbegin(), keys[index - 1]);
		
		if (!current->leaf)
			current->childs.insert(current->childs.cbegin(), previous->childs.back());

		keys[index - 1] = previous->keys.back();
		previous->keys.pop_back();

		return;
	}

	// Borrow key from next child
	if (index != keys.size() && childs[index + 1]->keys.size() >= (order >> 1)) {
		auto current = childs[index];
		auto next = childs[index + 1];

		current->keys.push_back(keys[index]);

		if (!current->leaf)
			current->childs.push_back(next->childs.front());

		keys[index] = next->keys.front();
		next->keys = { next->keys.cbegin() + 1, next->keys.cend() };

		if (!next->leaf)
			next->childs = { next->childs.cbegin() + 1, next->childs.cend() };

		return;
	}

	index != keys.size()
		? merge(index)
		: merge(index - 1);
}

// Merge childs[index] with childs[index + 1]
void BTreeNode::merge(std::int32_t index) {
	auto left = childs[index];
	auto right = childs[index + 1];

	// Copy keys from right to left
	left->keys.push_back(keys[index]);
	for (auto key : right->keys)
		left->keys.push_back(key);

	// Copy childs from right to left
	if (!left->leaf)
		for (auto child : right->childs)
			left->childs.push_back(child);

	keys.erase(keys.cbegin() + index);
	childs.erase(childs.cbegin() + (index + 1));

	delete right;
}

bool BTreeNode::search(std::int32_t value) {
	auto result = std::lower_bound(keys.cbegin(), keys.cend(), value);
	std::int32_t place = result - keys.cbegin();

	if (result != keys.cend())
		if (keys[place] == value)
			return true;

	if (leaf)
		return false;

	return childs[place]->search(value);
};

void BTreeNode::traverse() {
	std::cout << "New node" << std::endl;
	for (auto e : keys) {
		std::cout << e << ' ';
	} std::cout << std::endl;

	for (auto e : childs)
		if (e != nullptr)
			e->traverse();
};

std::string BTreeNode::visualize (std::int32_t& counter, std::string& writer) {
	std::string nodeId = "node" + std::to_string(counter);
	++counter;
	writer += nodeId + "[label = \"";
	for (std::int32_t key { 0 }; key < keys.size(); ++key) {
		writer += "<f" + std::to_string(key) + "> |" + std::to_string(this->keys[key]) + "|";
		if (key == keys.size() - 1)
			writer += "<f" + std::to_string(key + 1) + ">\"];\n";
	}

	for (std::int32_t childNumber { 0 }; auto child : childs) {
		if (child != nullptr) {
			std::string childId = child->visualize(counter, writer);
			writer += "\"" + nodeId + "\":f" + std::to_string(childNumber) + " -> \"" + childId + "\"\n";
			++childNumber;
		}
	}

	return nodeId;
};

bool BTreeNode::isFull() {
	return keys.size() == order - 1;
};
