#include "encoding/predictive/trie-oracle.h"
#include "encoding/predictive/oracle.h"
#include "data/trie.h"
#include <algorithm>
#include <memory>
#include <deque>
#include "util.h"


namespace
{
    class TrieOracle : public encoding::predictive::Oracle
    {
    private:
        typedef data::Trie<Datum, u64> trie;

        std::unique_ptr<trie> m_trie;
        unsigned m_max_depth;
        std::deque<Datum> m_queue;

    public:
        TrieOracle(unsigned max_depth) : m_max_depth(max_depth), m_trie(std::make_unique<trie>())
        {
            // NOP
        }

        void reset() override
        {
            m_trie = std::make_unique<trie>();
            m_queue.clear();
        }

        void tell(Datum datum) override
        {
            m_queue.push_back(datum);

            for (size_t n = 0; n < m_queue.size(); ++n)
            {
                auto trie = find_subtree(n);
                trie->data++;
            }
            
            if (m_queue.size() > m_max_depth)
            {
                m_queue.pop_front();
            }
        }

        Datum predict() const override
        {
            for (size_t i = 0; i != m_queue.size(); ++i)
            {
                auto subtree = find_subtree(i);
                auto keys = subtree->keys();

                if (keys.size() > 0)
                {
                    return *std::max_element(keys.begin(), keys.end(), [&](Datum k1, Datum k2) -> bool { return (*subtree)[k1].data < (*subtree)[k2].data; });
                }
            }

            return 0;
        }


    private:
        trie* find_subtree(size_t start_index) const
        {
            auto trie = m_trie.get();

            for ( size_t i = start_index; i < m_queue.size(); ++i )
            {
                auto datum = m_queue[i];
                trie = &(*trie)[datum];
            }

            return trie;
        }
    };
}

std::unique_ptr<encoding::predictive::Oracle> encoding::predictive::trie_oracle(unsigned max_depth)
{
    return std::make_unique<TrieOracle>(max_depth);
}
