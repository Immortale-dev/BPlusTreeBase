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
		bool expired();
		
	protected:
		child_item_type_wptr item;
		instance_type* base;
};


template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::BPlusTreeBaseIterator(const self_type& iter)
{
	this->item = iter.item;
	this->base = iter.base;
	
	child_item_type_ptr it = item.lock();
	
	if(it){
		base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type& BPlusTreeBaseIterator<Key, T, D>::operator=(const self_type& iter)
{
	child_item_type_ptr it = item.lock();
	if(it){
		base->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = iter.item;
	this->base = iter.base;
	
	it = item.lock();
	
	if(it){
		base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
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
	child_item_type_ptr it = item.lock();
	if(it){
		base->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
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
	
	child_item_type_ptr it = item.lock();
	
	if(it){
		base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
}

template <class Key, class T, class D>
Key BPlusTreeBaseIterator<Key, T, D>::get_key()
{
	child_item_type_ptr it = item.lock();
	if(!it || !it->item)
		throw std::out_of_range("Could Not get key from end()");
	return it->item->first;
}

template <class Key, class T, class D>
BPlusTreeBaseIterator<Key, T, D>::~BPlusTreeBaseIterator()
{
	child_item_type_ptr it = item.lock();
	if(it){
		base->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
}

template <class Key, class T, class D>
T BPlusTreeBaseIterator<Key, T, D>::get_value()
{
	child_item_type_ptr it = item.lock();
	if(!it || !it->item)
		throw std::out_of_range("Could Not get value from end()");
	return it->item->second;
}

template <class Key, class T, class D>
bool BPlusTreeBaseIterator<Key, T, D>::expired()
{
	return (bool)(item.lock());
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator++()
{	
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	base->processIteratorMoveStart(it, 1);
	
	node_ptr node = nullptr;
	if(it && it->node)
		node = it->node;
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			it = item.lock();
			
			base->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		base->processIteratorMoveEnd(it, 1);
		
		return *this;
	}
	if(node->childs_size() == it->pos+1){
		
		node_ptr onode = node;
		
		node = node->next_leaf();
		
		it = nullptr;
		item = it;
		
		if(node){		
			item = node->first_child();
			
			it = item.lock();
			
			// Reserve Item
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process release item
		base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			base->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move End
		base->processIteratorMoveEnd(it, 1);
		
		return *this;
	}
	
	item = node->get(it->pos+1);
	it = item.lock();
	
	// Reserve Item
	base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Process release item
	base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	base->processIteratorMoveEnd(it, 1);
	
	return *this;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator++(int)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	base->processIteratorMoveStart(it, 1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(it && it->node)
		node = it->node;
	
	if(!node){
		node_ptr tmp = base->min_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			it = item.lock();
			
			base->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		base->processIteratorMoveEnd(it, 1);
		
		return n;
	}
	if(node->childs_size() == it->pos+1){
		
		node_ptr onode = node;
		
		node = node->next_leaf();
		
		it = nullptr;
		item = it;
		
		if(node){
			item = node->first_child();
			it = item.lock();
			
			// Reserve Item
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			base->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move End
		base->processIteratorMoveEnd(it, 1);
		
		return n;
	}
	
	item = node->get(it->pos+1);
	it = item.lock();
	
	// Reserve Item
	base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	base->processIteratorMoveEnd(it, 1);
	
	return n;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator--()
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	base->processIteratorMoveStart(it, -1);
	
	node_ptr node = nullptr;
	if(it && it->node)
		node = it->node;
	
	if(!node){
		node_ptr tmp = base->max_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			it = item.lock();
			
			base->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		base->processIteratorMoveEnd(it, -1);
		
		return *this;
	}
	if(it->pos == 0){
		
		node_ptr onode = node;
		node = node->prev_leaf();
		
		it = nullptr;
		item = it;
		
		if(node){		
			item = node->last_child();
			it = item.lock();
			
			// Relerve Item
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			base->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move Start
		base->processIteratorMoveEnd(it, -1);
		
		return *this;
	}
	
	item = node->get(it->pos-1);
	it = item.lock();
	
	// Reserve Item
	base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	base->processIteratorMoveEnd(it, -1);
	
	return *this;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::self_type BPlusTreeBaseIterator<Key, T, D>::operator--(int)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	base->processIteratorMoveStart(it, -1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(it && it->node)
		node = it->node;
		
	if(!node){
		node_ptr tmp = base->max_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			it = item.lock();
			
			base->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		base->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move Start
		base->processIteratorMoveEnd(it, -1);
		
		return n;
	}
	if(it->pos == 0){
		
		node_ptr onode = node;
		
		node = node->prev_leaf();
		
		it = nullptr;
		item = it;
		
		if(node){
			item = node->last_child();
			it = item.lock();
			
			// Reserve Item
			base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			base->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move Start
		base->processIteratorMoveEnd(it, -1);
		
		return n;
	}
	
	item = node->get(it->pos-1);
	it = item.lock();
	
	// Reserve Item
	base->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	base->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	base->processIteratorMoveEnd(it, -1);
	
	return n;
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::reference BPlusTreeBaseIterator<Key, T, D>::operator*()
{
	child_item_type_ptr it = item.lock();
	return *(it->item);
}

template <class Key, class T, class D>
typename BPlusTreeBaseIterator<Key, T, D>::pointer BPlusTreeBaseIterator<Key, T, D>::operator->()
{
	child_item_type_ptr it = item.lock();
	return it->item;
}

template <class Key, class T, class D>
bool BPlusTreeBaseIterator<Key, T, D>::operator==(const self_type &r)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr rit = r.item.lock();
	if(!it && !rit){
		return base == r.base;
	}
	if(!it || !rit){
		return false;
	}
	return (it->node.get() == rit->node.get() && it->pos == rit->pos);
}

template <class Key, class T, class D>
bool BPlusTreeBaseIterator<Key, T, D>::operator!=(const self_type &r)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr rit = r.item.lock();
	if(!it && !rit){
		return base != r.base;
	}
	if(!it || !rit){
		return true;
	}
	return it->node.get() != rit->node.get() || it->pos != rit->pos;
}


#endif // BPLUSTREEBASEITERATOR_H
