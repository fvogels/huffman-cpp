#include "encoding/huffman/tree-builder.h"
#include <algorithm>


namespace
{
    typedef std::pair<Datum, u64> wpair;
    typedef data::Node<wpair>     wnode;
    typedef data::Leaf<wpair>     wleaf;
    typedef data::Branch<wpair>   wbranch;

    std::vector<std::unique_ptr<wnode>> create_nodes(const data::FrequencyTable<Datum>& frequencies)
    {
        std::vector<std::unique_ptr<wnode>> result;

        for (auto& datum : frequencies.values())
        {
            auto frequency = frequencies[datum];
            wpair pair(datum, frequency);
            std::unique_ptr<wnode> node = std::make_unique<wleaf>(pair);
            result.push_back(std::move(node));
        }

        return result;
    }

    u64 weight(const wnode& node)
    {
        if (node.is_leaf())
        {
            const wleaf& leaf = static_cast<const wleaf&>(node);
            return leaf.value().second;
        }
        else
        {
            const wbranch& branch = static_cast<const wbranch&>(node);
            return weight(branch.left_child()) + weight(branch.right_child());
        }
    }

    void sort_queue(std::vector<std::unique_ptr<wnode>>& nodes)
    {
        std::sort(nodes.begin(), nodes.end(), [](const std::unique_ptr<wnode>& p, const std::unique_ptr<wnode>& q) {
            return weight(*p) > weight(*q); // sort from heavy to light!
        });
    }
}

std::unique_ptr<data::Node<Datum>> encoding::huffman::build_tree(const data::FrequencyTable<Datum>& frequencies)
{
    auto queue = create_nodes(frequencies);
    assert(queue.size() > 0);

    while (queue.size() > 1)
    {
        sort_queue(queue);

        assert(weight(*queue[0]) >= weight(*queue[1]));

        auto left_node = std::move(queue.back());
        queue.pop_back();
        auto right_node = std::move(queue.back());
        queue.pop_back();

        auto branch = std::make_unique<wbranch>(std::move(left_node), std::move(right_node));

        queue.push_back(std::move(branch));
    }

    assert(queue.size() == 1);
    auto tree = std::move(queue.back());

    return data::map<wpair, Datum>(*tree, [](const wpair& pair) -> Datum { return pair.first; });
}