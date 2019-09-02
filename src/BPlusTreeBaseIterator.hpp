#ifndef BPLUSTREEBASEITERATOR_H
#define BPLUSTREEBASEITERATOR_H


#include "BPlusTreeBaseNode.hpp"
#include <utility>


template < class Key, class T > class BPlusTreeBase;

template <class Key, class T>
class BPlusTreeBaseIterator
{
	public:
		typedef std::pair<Key, T> entry_item;
		typedef BPlusTreeBaseNode<Key, T> Node;
		typedef BPlusTreeBaseIterator<Key, T> self_type;
		typedef entry_item value_type;
		typedef entry_item& reference;
		typedef entry_item* pointer;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef int difference_type;
		typedef BPlusTreeBase<Key, T> instance_type;
		
		self_type operator++();
		self_type operator++(int);
		self_type operator--();
		self_type operator--(int);
		reference operator*();
		pointer operator->();
		bool operator==(const self_type &r);
		bool operator!=(const self_type &r);
		
		BPlusTreeBaseIterator();
        BPlusTreeBaseIterator(Node* node, Key key, instance_type* base);
        virtual ~BPlusTreeBaseIterator();
        Key get_key();
        T get_value();
        
    protected:
        Node* node;
        Key key;
        instance_type* base;
        
};


template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator()
{
    this->node = nullptr;
    this->base = nullptr;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(Node* node, Key key, instance_type* base)
{
    this->node = node;
    this->key = key;
    this->base = base;
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

template <class Key, class T>
T BPlusTreeBaseIterator<Key, T>::get_value()
{
	return (node->get(node->get_index(key)))->second;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator++()
{
	int index = node->get_index(key)+1;
	if(index >= node->size()){
		node = node->next_leaf();
		index = 0;
	}
	if(node){
		base->processSearchNodeStart(node);
		key = node->get(index)->first;
		base->processSearchNodeEnd(node);
	}
	return *this;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator++(int)
{
	self_type n = *this;
	int index = node->get_index(key)+1;
	if(index >= node->size()){
		node = node->next_leaf();
		index = 0;
	}
	if(node){
		base->processSearchNodeStart(node);
		key = node->get(index)->first;
		base->processSearchNodeEnd(node);
	}
	return n;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator--()
{
	int index = node->get_index(key)-1;
	if(index < 0){
		node = node->prev_leaf();
		index = node->size()-1;
	}
	if(node){
		base->processSearchNodeStart(node);
		key = node->get(index)->first;
		base->processSearchNodeEnd(node);
	}
	return *this;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator--(int)
{
	self_type n = *this;
	int index = node->get_index(key)-1;
	if(index < 0){
		node = node->prev_leaf();
		index = node->size()-1;
	}
	if(node){
		base->processSearchNodeStart(node);
		key = node->get(index)->first;
		base->processSearchNodeEnd(node);
	}
	return n;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::reference BPlusTreeBaseIterator<Key, T>::operator*()
{
	int index = node->get_index(key);
	return *(node->get(index));
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::pointer BPlusTreeBaseIterator<Key, T>::operator->()
{
	int index = node->get_index(key);
	return (node->get(index));
}

template <class Key, class T>
bool BPlusTreeBaseIterator<Key, T>::operator==(const self_type &r)
{
	if(!node && !r.node)
		return base == r.base;
	return (node == r.node && key == r.key);
}

template <class Key, class T>
bool BPlusTreeBaseIterator<Key, T>::operator!=(const self_type &r)
{
	if(!node && !r.node)
		return base != r.base;
	return node != r.node || key != r.key;
}








#endif // BPLUSTREEBASEITERATOR_H
