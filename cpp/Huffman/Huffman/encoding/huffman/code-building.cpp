#include "encoding/huffman/code-building.h"


namespace
{
    void build_codes(const data::Node<Datum>& node, std::vector<Datum>& prefix, std::vector<std::vector<Datum>>* result)
    {
        if (node.is_branch())
        {
            auto& branch = static_cast<const data::Branch<Datum>&>(node);

            prefix.push_back(0);
            build_codes(branch.left_child(), prefix, result);
            prefix.pop_back();
            prefix.push_back(1);
            build_codes(branch.right_child(), prefix, result);
            prefix.pop_back();
        }
        else
        {
            auto& leaf = static_cast<const data::Leaf<Datum>&>(node);
            auto& datum = leaf.value();

            (*result)[datum] = prefix; // don't forget ()!
        }
    }
}

std::vector<std::vector<Datum>> encoding::huffman::build_codes(const data::Node<Datum>& tree, u64 domain_size)
{
    std::vector<std::vector<Datum>> result(domain_size + 1);
    std::vector<Datum> prefix;

    ::build_codes(tree, prefix, &result);
    return result;
}
