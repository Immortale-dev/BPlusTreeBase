#ifndef BPLUSTREEBASEITERATOR_H
#define BPLUSTREEBASEITERATOR_H


#include <utility>
#include <memory>
#include <stdexcept>
#include "BPlusTreeBaseNode.hpp"


template < class Key, class T, class D > class BPlusTreeBase;

template <class Key, class T, class D>
class BPlusTreeBaseIterator
{
	public:
		typedef std::pair<const Key, T> entry_item;
		typedef BPlusTreeBase<Key, T, D> instance_type;
		typedef typename instance_type::Node Node;
		typedef typename Node::child_item_type child_item_type;
		typedef typename Node::child_item_type_ptr child_item_type_ptr;
		typedef std::weak_ptr<child_item_type> child_item_type_wptr;
		typedef std::shared_ptr<Node> node_ptr;
		typedef BPlusTreeBaseIterator<Key, T, D> self_type;
		typedef entry_item value_type;
		typedef entry_item& reference;
		typedef std::shared_ptr<entry_item> pointer;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef int difference_type;
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
        BPlusTreeBaseIterator(child_item_type_wptr item, instance_type* base);
        virtual ~BPlusTreeBaseIterator();
        Key get_key();
        T get_value();
        
    protected:
        child_item_type_wptr item;
        instance_type* base;
};


template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::BPlusTreeBaseIterator(const self_type& iter)
{
	this->item = iter.item;
	this->base = iter.base;
	
	if(!item.expired()){
		base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	if(!item.expired() && item.lock()->node){
		// reserve node
		base->processIteratorNodeReserved(item.lock()->node);
	}
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type& BPlusTreeBaseIterator<Key, T, D>::operator=(const self_type& iter)
{
	if(!item.expired() && item.lock()->node){
		// release node
		base->processIteratorNodeReleased(item.lock()->node);
	}
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = iter.item;
	this->base = iter.base;
	
	if(!item.expired()){
		base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	if(!item.expired() && item.lock()->node){
		// reserve node
		base->processIteratorNodeReserved(item.lock()->node);
	}
	
	return *this;
}

template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::BPlusTreeBaseIterator(self_type&& iter)
{
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type& BPlusTreeBaseIterator<Key, T, D>::operator=(self_type&& iter)
{
	if(!item.expired() && item.lock()->node){
		// release node
		base->processIteratorNodeReleased(item.lock()->node);
	}
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
	return *this;
}

template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::BPlusTreeBaseIterator()
{
    this->base = nullptr;
    //this->item = nullptr;
}

template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::BPlusTreeBaseIterator(child_item_type_wptr item, instance_type* base)
{
	// Item already should be reserved
    this->item = item;
    this->base = base;
    if(!item.expired() && item.lock()->node){
		// reserve node
		base->processIteratorNodeReserved(item.lock()->node);
	}
}

template <class Key, class T, class D>
Key BPlusTreeBaseIterator<Key, T, D>::get_key()
{
	if(item.expired() || !item.lock()->item)
		throw std::out_of_range("Could Not get key from end()");
    return this->item.lock()->item->first;
}

template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::~BPlusTreeBaseIterator()
{
	if(!item.expired() && item.lock()->node){
		base->processIteratorNodeReleased(item.lock()->node);
	}
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
}

template <class Key, class T, class D>
T BPlusTreeBaseIterator<Key, T, D>::get_value()
{
	if(item.expired() || !item.lock()->item)
		throw std::out_of_range("Could Not get value from end()");
	return this->item.lock()->item->second;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator++()
{	
	// Process Move Start
	base->processIteratorMoveStart(item.lock(), 1);
	
	node_ptr node = nullptr;
	if(!item.expired() && item.lock()->node)
		node = item.lock()->node;
		
	// Process release item
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		// Process search start
		base->processSearchNodeStart(tmp, instance_type::PROCESS_TYPE::READ);
		
		if(!tmp->size()){
			item = child_item_type_ptr(nullptr);
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item.lock()->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		base->processIteratorMoveEnd(item.lock(), 1);
		
		return *this;
	}
	if(node->childs_size() == item.lock()->pos+1){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->next_leaf();
		
		item = child_item_type_ptr(nullptr);
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node, instance_type::PROCESS_TYPE::READ);
			
			item = node->first_child();
			
			// Reserve Item
			base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move End
		base->processIteratorMoveEnd(item.lock(), 1);
		
		return *this;
	}
	
	item = node->get(item.lock()->pos+1);
	
	// Reserve Item
	base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	base->processIteratorMoveEnd(item.lock(), 1);
	
	return *this;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator++(int)
{
	// Process Move Start
	base->processIteratorMoveStart(item.lock(), 1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(!item.expired() && item.lock()->node)
		node = item.lock()->node;
		
	// Release current item
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		// Process search start
		base->processSearchNodeStart(tmp, instance_type::PROCESS_TYPE::READ);
		
		if(!tmp->size()){
			item = child_item_type_ptr(nullptr);
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item.lock()->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		base->processIteratorMoveEnd(item.lock(), 1);
		
		return n;
	}
	if(node->childs_size() == item.lock()->pos+1){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->next_leaf();
		
		item = child_item_type_ptr(nullptr);
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node, instance_type::PROCESS_TYPE::READ);
			
			item = node->first_child();
			
			// Reserve Item
			base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move End
		base->processIteratorMoveEnd(item.lock(), 1);
		
		return n;
	}
	
	item = node->get(item.lock()->pos+1);
	
	// Reserve Item
	base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	base->processIteratorMoveEnd(item.lock(), 1);
	
	return n;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator--()
{
	// Process Move Start
	base->processIteratorMoveStart(item.lock(), -1);
	
	node_ptr node = nullptr;
	if(!item.expired() && item.lock()->node)
		node = item.lock()->node;
		
	// Release current item
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
	
	if(!node){
		node_ptr tmp = base->max_node();
		
		// Process search start
		base->processSearchNodeStart(tmp, instance_type::PROCESS_TYPE::READ);
		
		if(!tmp->size()){
			item = child_item_type_ptr(nullptr);
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item.lock()->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		base->processIteratorMoveEnd(item.lock(), -1);
		
		return *this;
	}
	if(item.lock()->pos == 0){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->prev_leaf();
		
		item = child_item_type_ptr(nullptr);
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node, instance_type::PROCESS_TYPE::READ);
			
			item = node->last_child();
			
			// Relerve Item
			base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);

			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move Start
		base->processIteratorMoveEnd(item.lock(), -1);
		
		return *this;
	}
	
	item = node->get(item.lock()->pos-1);
	
	// Reserve Item
	base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	base->processIteratorMoveEnd(item.lock(), -1);
	
	return *this;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator--(int)
{
	// Process Move Start
	base->processIteratorMoveStart(item.lock(), -1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(!item.expired() && item.lock()->node)
		node = item.lock()->node;
		
	// Release current item
	if(!item.expired()){
		base->processItemRelease(item.lock(), instance_type::PROCESS_TYPE::READ);
	}
		
	if(!node){
		node_ptr tmp = base->max_node();
		
		// Process search start
		base->processSearchNodeStart(tmp, instance_type::PROCESS_TYPE::READ);
		
		if(!tmp->size()){
			item = child_item_type_ptr(nullptr);
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(item.lock()->node);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move Start
		base->processIteratorMoveEnd(item.lock(), -1);
		
		return n;
	}
	if(item.lock()->pos == 0){
		
		// Process release iterator node
		base->processIteratorNodeReleased(node);
		
		node = node->prev_leaf();
		
		item = child_item_type_ptr(nullptr);
		
		if(node){
			// Process search start
			base->processSearchNodeStart(node, instance_type::PROCESS_TYPE::READ);
			
			item = node->last_child();
			
			// Reserve Item
			base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
			
			// Process reserve iterator node
			base->processIteratorNodeReserved(node);
			
			// Process search end
			base->processSearchNodeEnd(node, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move Start
		base->processIteratorMoveEnd(item.lock(), -1);
		
		return n;
	}
	
	item = node->get(item.lock()->pos-1);
	
	// Reserve Item
	base->processItemReserve(item.lock(), instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	base->processIteratorMoveEnd(item.lock(), -1);
	
	return n;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::reference BPlusTreeBaseIterator<Key, T, D>::operator*()
{
	return *(item.lock()->item);
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::pointer BPlusTreeBaseIterator<Key, T, D>::operator->()
{
	return item.lock()->item;
}

template <class Key, class T, class D>
bool BPlusTreeBaseIterator<Key, T, D>::operator==(const self_type &r)
{
	if(item.expired() && r.item.expired()){
		return base == r.base;
	}
	if(item.expired() || r.item.expired()){
		return false;
	}
	return (item.lock()->node.get() == r.item.lock()->node.get() && item.lock()->pos == r.item.lock()->pos);
}

template <class Key, class T, class D>
bool BPlusTreeBaseIterator<Key, T, D>::operator!=(const self_type &r)
{
	if(item.expired() && r.item.expired()){
		return base != r.base;
	}
	if(item.expired() || r.item.expired()){
		return true;
	}
	return item.lock()->node.get() != r.item.lock()->node.get() || item.lock()->pos != r.item.lock()->pos;
}


#endif // BPLUSTREEBASEITERATOR_H
