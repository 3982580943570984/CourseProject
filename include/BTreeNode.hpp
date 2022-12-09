#ifndef BTREENODE_HPP
#define BTREENODE_HPP

#include <vector>
#include <string>
#include <cstdint>

struct BTreeNode {
        friend class BTree;
        private:
                std::vector<std::int32_t> keys{};
                std::vector<BTreeNode*> childs{};
                std::int32_t order{ 0 };
                bool leaf{ false };
        public:
                BTreeNode() = default;
                ~BTreeNode() = default;

                BTreeNode(std::int32_t order, bool leaf);

                void insert (std::int32_t value);
                void split (std::uint32_t link, BTreeNode* chid);
                void remove (std::int32_t value);
                void fill (std::int32_t index);
                void merge (std::int32_t index);
                bool search (std::int32_t value);
                void traverse ();
                std::string visualize (std::int32_t& counter, std::string& writer);
                bool isFull ();
};

#endif /* BTREENODE_HPP */
