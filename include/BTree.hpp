#ifndef BTREE_HPP
#define BTREE_HPP

#include "include/BTreeNode.hpp"

#include <cstdint>
#include <string>

class BTree {
        private:
                std::int32_t order { 0 };
                BTreeNode* root { nullptr };
        public:
                BTree () noexcept = default;
                ~BTree () noexcept;

                BTree(std::int32_t order);

                void insert (std::int32_t value);
                void remove (std::int32_t value);
                void modify (std::int32_t elem, std::int32_t value);
                bool search (std::int32_t value);

                void traverse ();
                std::string visualize ();
};

#endif /* BTREE_HPP */
