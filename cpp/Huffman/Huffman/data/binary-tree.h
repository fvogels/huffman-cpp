#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "util.h"
#include <functional>
#include <assert.h>
#include <memory>

namespace data
{
    template<typename T>
    class Node
    {
    public:
        virtual ~Node() { }

        virtual bool equal_to(const Node<T>& other) const = 0;

        virtual bool is_branch() const = 0;

        bool is_leaf() const
        {
            return !is_branch();
        }
    };

    template<typename T>
    class Branch : public Node<T>
    {
    private:
        std::unique_ptr<Node<T>> m_left_child;
        std::unique_ptr<Node<T>> m_right_child;

    public:
        Branch(std::unique_ptr<Node<T>> left_child, std::unique_ptr<Node<T>> right_child) : m_left_child(std::move(left_child)), m_right_child(std::move(right_child))
        {
            assert(m_left_child != nullptr);
            assert(m_right_child != nullptr);
        }

        const Node<T>& left_child() const
        {
            return *this->m_left_child;
        }

        const Node<T>& right_child() const
        {
            return *this->m_right_child;
        }

        bool equal_to(const Node<T>& other) const override
        {
            if (!other.is_branch())
            {
                return false;
            }

            const Branch<T>& branch = static_cast<const Branch<T>&>(other);

            return m_left_child->equal_to(branch.left_child()) && m_right_child->equal_to(branch.right_child());
        }

        bool is_branch() const override
        {
            return true;
        }        
    };

    template<typename T>
    class Leaf : public Node<T>
    {
    private:
        T m_value;

    public:
        Leaf(const T& value) : m_value(value)
        {
            // NOP
        }

        const T& value() const
        {
            return this->m_value;
        }

        bool is_branch() const override
        {
            return false;
        }

        bool equal_to(const Node<T>& other) const override
        {
            if (!other.is_leaf())
            {
                return false;
            }

            const Leaf<T>& leaf = static_cast<const Leaf<T>&>(other);

            return m_value == leaf.value();
        }
    };

    // Needs to be external function because of OUT type parameter
    // It is impossible to create a template virtual method
    template<typename IN, typename OUT>
    std::unique_ptr<Node<OUT>> map(const Node<IN>& tree, std::function<OUT(const IN&)> function)
    {
        if (tree.is_leaf())
        {
            auto& leaf = static_cast<const Leaf<IN>&>(tree);

            return std::make_unique<Leaf<OUT>>(function(leaf.value()));
        }
        else
        {
            auto& branch = static_cast<const Branch<IN>&>(tree);
            auto left = map(branch.left_child(), function);
            auto right = map(branch.right_child(), function);

            return std::make_unique<Branch<OUT>>(std::move(left), std::move(right));
        }
    }
}

#endif
