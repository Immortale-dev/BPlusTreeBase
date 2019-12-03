#ifndef BPLUSTREEBASEITERATOR_H
#define BPLUSTREEBASEITERATOR_H


#include "BPlusTreeBaseNode.hpp"
#include <utility>
#include <memory>


template < class Key, class T > class BPlusTreeBase;

template <class Key, class T>
class BPlusTreeBaseIterator
{
	public:
		typedef std::pair<const Key, T> entry_item;
		typedef BPlusTreeBaseNode<Key, T> Node;
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
        BPlusTreeBaseIterator(node_ptr node, childs_type_iterator it, instance_type* base);
        virtual ~BPlusTreeBaseIterator();
        Key get_key();
        T get_value();
        
    protected:
        node_ptr node;
        childs_type_iterator item;
        instance_type* base;
};


template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(const self_type& iter)
{
	this->item = iter.item;
	this->node = iter.node;
	this->base = iter.base;
	
	if(this->node){
		// reserve node
		base->processIteratorNodeReserved(node);
	}
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type& BPlusTreeBaseIterator<Key, T>::operator=(const self_type& iter)
{
	if(this->node){
		// release node
		base->processIteratorNodeReleased(node);
	}
	
	this->item = iter.item;
	this->node = iter.node;
	this->base = iter.base;
	
	if(this->node){
		// reserve node
		base->processIteratorNodeReserved(node);
	}
	
	return *this;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(self_type&& iter)
{
	this->item = std::move(iter.item);
	this->node = std::move(iter.node);
	this->base = std::move(iter.base);
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type& BPlusTreeBaseIterator<Key, T>::operator=(self_type&& iter)
{
	if(this->node){
		// release node
		base->processIteratorNodeReleased(this->node);
	}
	
	this->item = std::move(iter.item);
	this->node = std::move(iter.node);
	this->base = std::move(iter.base);
	return *this;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator()
{
    this->node = nullptr;
    this->base = nullptr;
    //this->item = nullptr;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator(node_ptr node, childs_type_iterator it, instance_type* base)
{
    this->node = node;
    this->base = base;
    this->item = it;
    if(this->node){
		// reserve node
		base->processIteratorNodeReserved(node);
	}
}

template <class Key, class T>
Key BPlusTreeBaseIterator<Key, T>::get_key()
{
    return (*(this->item))->first;
}

template <class Key, class T>
BPlusTreeBaseIterator<Key, T>::~BPlusTreeBaseIterator()
{
	if(this->node){
		base->processIteratorNodeReleased(this->node);
	}
}

template <class Key, class T>
T BPlusTreeBaseIterator<Key, T>::get_value()
{
	return (*(this->item))->second;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator++()
{	
	// Process Move Start
	base->processIteratorMoveStart(*this, node, 1);
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		item = tmp->childs_iterator();
		if(!tmp->size()){
			node = nullptr;
		}
		else{
			node = tmp;
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move End
		base->processIteratorMoveEnd(*this, node, 1);
		
		return *this;
	}
	if(node->childs_iterator_end() == ++item){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->next_leaf();
		
		if(node){
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search start
			base->processSearchNodeStart(node);
			
			item = node->childs_iterator();
			
			// Process search end
			base->processSearchNodeEnd(node);
		}		
	}
	
	// Process Move End
	base->processIteratorMoveEnd(*this, node, 1);
	
	return *this;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator++(int)
{
	// Process Move Start
	base->processIteratorMoveStart(*this, node, 1);
	
	self_type n = *this;
	if(!node){
		node_ptr tmp = base->min_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		item = tmp->childs_iterator();
		if(!tmp->size()){
			node = nullptr;
		}
		else{
			node = tmp;
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move End
		base->processIteratorMoveEnd(*this, node, 1);
		
		return n;
	}
	if(node->childs_iterator_end() == ++item){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->next_leaf();
		
		if(node){
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search start
			base->processSearchNodeStart(node);
		
			item = node->childs_iterator();
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
	}
	
	// Process Move End
	base->processIteratorMoveEnd(*this, node, 1);
	
	return n;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator--()
{
	// Process Move Start
	base->processIteratorMoveStart(*this, node, -1);
	
	if(!node){
		node_ptr tmp = base->max_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		item = tmp->childs_iterator()+(tmp->size()-1);
		if(!tmp->size()){
			node = nullptr;
		}
		else{
			node = tmp;
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move End
		base->processIteratorMoveEnd(*this, node, -1);
		
		return *this;
	}
	if(node->childs_iterator() == item--){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->prev_leaf();
		
		if(node){
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search start
			base->processSearchNodeStart(node);
			
			item = node->childs_iterator()+(node->size()-1);
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
	}
	
	// Process Move Start
	base->processIteratorMoveEnd(*this, node, -1);
	
	return *this;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::self_type BPlusTreeBaseIterator<Key, T>::operator--(int)
{
	// Process Move Start
	base->processIteratorMoveStart(*this, node, -1);
	
	self_type n = *this;
	if(!node){
		node_ptr tmp = base->max_node();
		
		// Process search start
		base->processSearchNodeStart(tmp);
		
		item = tmp->childs_iterator()+(tmp->size()-1);
		if(!tmp->size()){
			node = nullptr;
		}
		else{
			node = tmp;
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp);
		
		// Process Move Start
		base->processIteratorMoveEnd(*this, node, -1);
		
		return n;
	}
	if(node->childs_iterator() == item--){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->prev_leaf();
		
		if(node){
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search start
			base->processSearchNodeStart(node);
		
			item = node->childs_iterator()+(node->size()-1);
			
			// Process search end
			base->processSearchNodeEnd(node);
		}
	}
	
	// Process Move Start
	base->processIteratorMoveEnd(*this, node, -1);
	
	return n;
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::reference BPlusTreeBaseIterator<Key, T>::operator*()
{
	return *(*item);
}

template <class Key, class T>
typename BPlusTreeBaseIterator<Key, T>::pointer BPlusTreeBaseIterator<Key, T>::operator->()
{
	return *item;
}

template <class Key, class T>
bool BPlusTreeBaseIterator<Key, T>::operator==(const self_type &r)
{
	if(!node && !r.node)
		return base == r.base;
	return (node.get() == r.node.get() && item == r.item);
}

template <class Key, class T>
bool BPlusTreeBaseIterator<Key, T>::operator!=(const self_type &r)
{
	if(!node && !r.node)
		return base != r.base;
	return node.get() != r.node.get() || item != r.item;
}








#endif // BPLUSTREEBASEITERATOR_H
