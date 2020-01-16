#ifndef BPLUSTREEBASEITERATOR_H
#define BPLUSTREEBASEITERATOR_H


#include <utility>
#include <memory>
#include <stdexcept>
#include "BPlusTreeBaseNode.hpp"


template < class Key, class T > class BPlusTreeBase;

template <class Key, class T>
class BPlusTreeBaseIterator
{
	public:
		typedef std::pair<const Key, T> entry_item;
		typedef BPlusTreeBaseNode<Key, T> Node;
		typedef typename Node::child_item_type_ptr child_item_ptr;
		typedef std::shared_ptr<Node> node_ptr;
		typedef BPlusTreeBaseIterator<Key, T> self_type;
		typedef entry_item value_type;
		typedef entry_item& reference;
		typedef std::shared_ptr<entry_item> pointer;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef int difference_type;
		typedef BPlusTreeBase<Key, T> instance_type;
		typedef typename Node::childs_type_iterator childs_type_iterator;
		
		self_type operator++();
		self_type operator++(int);
		self_type operator--();
		self_type operator--(int);
		reference operator*();
		pointer operator->();
		bool operator==(const self_type &r);
		bool operator!=(const self_type &r);
		
		BPlusTreeBaseIterator();
		BPlusTreeBaseIterator(const self_type& iter);
		self_type& operator=(const self_type& iter);
		BPlusTreeBaseIterator(self_type&& iter);
		self_type& operator=(self_type&& iter);
        BPlusTreeBaseIterator(child_item_ptr item, instance_type* base);
        virtual ~BPlusTreeBaseIterator();
        Key get_key();
        T get_value();
        
    protected:
        child_item_ptr item;
        instance_type* base;
};


template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(const self_type& iter)
{
	this->item = iter.item;
	this->base = iter.base;
	
	if(item && item->node){
		// reserve node
		base->processIteratorNodeReserved(item->node);
	}
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type& BPlusTreeBaseIterator<Key, T>::operator=(const self_type& iter)
{
	if(item && item->node){
		// release node
		base->processIteratorNodeReleased(item->node);
	}
	
	this->item = iter.item;
	this->base = iter.base;
	
	if(item && item->node){
		// reserve node
		base->processIteratorNodeReserved(item->node);
	}
	
	return *this;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(self_type&& iter)
{
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type& BPlusTreeBaseIterator<Key, T>::operator=(self_type&& iter)
{
	if(item && item->node){
		// release node
		base->processIteratorNodeReleased(item->node);
	}
	
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
	return *this;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator()
{
    this->base = nullptr;
    this->item = nullptr;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(child_item_ptr item, instance_type* base)
{
    this->item = item;
    this->base = base;
    if(item && item->node){
		// reserve node
		base->processIteratorNodeReserved(item->node);
	}
}

template <class Key, class T>
Key BPlusTreeBaseIterator<Key, T>::get_key()
{
	if(!item || !item->item)
		throw std::out_of_range("Could Not get key from end()");
    return this->item->item->first;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::~BPlusTreeBaseIterator()
{
	if(item && item->node){
		base->processIteratorNodeReleased(item->node);
	}
}

template <class Key, class T>
T BPlusTreeBaseIterator<Key, T>::get_value()
{
	if(!item || !item->item)
		throw std::out_of_range("Could Not get value from end()");
	return this->item->item->second;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator++()
{	
	// Process Move Start
	base->processIteratorMoveStart(item, 1);
	
	node_ptr node = nullptr;
	if(item && item->node)
		node = item->node;
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		if(!tmp->size()){
			item = nullptr;
		}
		else{
			item = tmp->first_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move End
		base->processIteratorMoveEnd(item, 1);
		
		return *this;
	}
	if(node->childs_size() == item->pos+1){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->next_leaf();
		
		item = nullptr;
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node);
			
			item = node->first_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
		
		// Process Move End
		base->processIteratorMoveEnd(item, 1);
		
		return *this;
	}
	
	item = node->get(item->pos+1);
	
	// Process Move End
	base->processIteratorMoveEnd(item, 1);
	
	return *this;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator++(int)
{
	// Process Move Start
	base->processIteratorMoveStart(item, 1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(item && item->node)
		node = item->node;
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		if(!tmp->size()){
			item = nullptr;
		}
		else{
			item = tmp->first_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move End
		base->processIteratorMoveEnd(item, 1);
		
		return n;
	}
	if(node->childs_size() == item->pos+1){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->next_leaf();
		
		item = nullptr;
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node);
			
			item = node->first_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
		
		// Process Move End
		base->processIteratorMoveEnd(item, 1);
		
		return n;
	}
	
	item = node->get(item->pos+1);
	
	// Process Move End
	base->processIteratorMoveEnd(item, 1);
	
	return n;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator--()
{
	// Process Move Start
	base->processIteratorMoveStart(item, -1);
	
	node_ptr node = nullptr;
	if(item && item->node)
		node = item->node;
	
	if(!node){
		node_ptr tmp = base->max_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		if(!tmp->size()){
			item = nullptr;
		}
		else{
			item = tmp->last_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move End
		base->processIteratorMoveEnd(item, -1);
		
		return *this;
	}
	if(item->pos == 0){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->prev_leaf();
		
		item = nullptr;
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node);
			
			item = node->last_child();

			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
		
		// Process Move Start
		base->processIteratorMoveEnd(item, -1);
		
		return *this;
	}
	
	item = node->get(item->pos-1);
	
	// Process Move Start
	base->processIteratorMoveEnd(item, -1);
	
	return *this;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator--(int)
{
	// Process Move Start
	base->processIteratorMoveStart(item, -1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(item && item->node)
		node = item->node;
		
	if(!node){
		node_ptr tmp = base->max_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		if(!tmp->size()){
			item = nullptr;
		}
		else{
			item = tmp->last_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move Start
		base->processIteratorMoveEnd(item, -1);
		
		return n;
	}
	if(item->pos == 0){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->prev_leaf();
		
		item = nullptr;
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node);
			
			item = node->last_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
		
		// Process Move Start
		base->processIteratorMoveEnd(item, -1);
		
		return n;
	}
	
	item = node->get(item->pos-1);
	
	// Process Move Start
	base->processIteratorMoveEnd(item, -1);
	
	return n;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::reference BPlusTreeBaseIterator<Key, T>::operator*()
{
	return *(item->item);
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::pointer BPlusTreeBaseIterator<Key, T>::operator->()
{
	return item->item;
}

template <class Key, class T>
bool BPlusTreeBaseIterator<Key, T>::operator==(const self_type &r)
{
	if(!item && !r.item){
		return base == r.base;
	}
	if(!item || !r.item){
		return false;
	}
	return (item->node.get() == r.item->node.get() && item->pos == r.item->pos);
}

template <class Key, class T>
bool BPlusTreeBaseIterator<Key, T>::operator!=(const self_type &r)
{
	if(!item && !r.item){
		return base != r.base;
	}
	if(!item || !r.item){
		return true;
	}
	return item->node.get() != r.item->node.get() || item->pos != r.item->pos;
}


#endif // BPLUSTREEBASEITERATOR_H
