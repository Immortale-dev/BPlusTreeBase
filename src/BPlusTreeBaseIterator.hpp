#ifndef BPLUSTREEBASEITERATOR_H
#define BPLUSTREEBASEITERATOR_H

#include "BPlusTreeBaseNode.hpp"
#include <utility>

template <class Key, class T>
class BPlusTreeBaseIterator
{
    typedef BPlusTreeBaseNode<Key, T> Node;
    typedef BPlusTreeBaseIterator<Key, T> self_type;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef int difference_type;
    typedef std::pair<Key, T> entry_item;


    public:
        BPlusTreeBaseIterator(Node* node, Key key);
        virtual ~BPlusTreeBaseIterator();
        Key get_key();
    protected:
        Node* node;
        Key key;
        entry_item* item;
        int index;
};


template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(Node* node, Key key)
{
    this->node = node;
    this->key = key;
}

template <class Key, class T>
Key BPlusTreeBaseIterator<Key, T>::get_key()
{
    return this->key;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::~BPlusTreeBaseIterator()
{

}


#endif // BPLUSTREEBASEITERATOR_H
