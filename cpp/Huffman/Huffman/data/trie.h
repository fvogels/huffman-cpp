#ifndef TRIE_H
#define TRIE_H

#include <memory>
#include <map>


namespace data
{
    template<typename T, typename U>
    class Trie
    {
    private:
        std::map<T, std::unique_ptr<Trie<T, U>>> m_children;

    public:
        U data;

        Trie() : data() { }

        Trie<T, U>& operator[](const T& key)
        {
            if (!contains(key))
            {
                m_children[key] = std::make_unique<Trie<T, U>>();
            }

            return *m_children[key];
        }

        bool contains(const T& key) const
        {
            return m_children.find(key) != m_children.end();
        }

        std::vector<T> keys() const
        {
            std::vector<T> result;

            for ( auto it = m_children.begin(); it != m_children.end(); ++it )
            {
                result.push_back(it->first);
            }

            return result;
        }
    };
}

#endif
