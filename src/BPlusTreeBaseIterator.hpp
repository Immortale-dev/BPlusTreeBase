#ifndef BPLUSTREEBASEITERATOR_H
#define BPLUSTREEBASEITERATOR_H


#include <utility>
#include <memory>
#include <stdexcept>
#include "BPlusTreeBaseUtils.hpp"
#include "BPlusTreeBaseNode.hpp"


__B_PLUS_TREE_ITERATOR_TEMPLATE__
class BPlusTreeBaseIterator
{
	public:
		typedef std::pair<const Key, T> entry_item;
		typedef BPlusTreeBase__Interface<Key, T> instance_type;
		typedef BPlusTreeBaseNode<Key, T> Node;
		typedef typename Node::child_item_type child_item_type;
		typedef typename Node::child_item_type_ptr child_item_type_ptr;
		typedef std::weak_ptr<child_item_type> child_item_type_wptr;
		typedef std::shared_ptr<Node> node_ptr;
		typedef __B_PLUS_TREE_BASEITERATOR_CLASS__ self_type;
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
		virtual instance_type* get_base();
		child_item_type_wptr item;
		instance_type* base;
};


__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator(const self_type& iter)
{
	this->item = iter.item;
	this->base = iter.base;
	
	child_item_type_ptr it = item.lock();
	
	if(it){
		get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type& __B_PLUS_TREE_BASEITERATOR_CLASS__::operator=(const self_type& iter)
{
	child_item_type_ptr it = item.lock();
	if(it){
		get_base()->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = iter.item;
	this->base = iter.base;
	
	it = item.lock();
	
	if(it){
		get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator(self_type&& iter)
{
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type& __B_PLUS_TREE_BASEITERATOR_CLASS__::operator=(self_type&& iter)
{
	child_item_type_ptr it = item.lock();
	if(it){
		get_base()->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator()
{
	this->base = nullptr;
	//this->item = nullptr;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator(child_item_type_wptr item, instance_type* base)
{
	// Item already should be reserved
	this->item = item;
	this->base = base;
	
	child_item_type_ptr it = item.lock();
	
	if(it){
		get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
Key __B_PLUS_TREE_BASEITERATOR_CLASS__::get_key()
{
	child_item_type_ptr it = item.lock();
	if(!it || !it->item)
		throw std::out_of_range("Could Not get key from end()");
	return it->item->first;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::~BPlusTreeBaseIterator()
{
	child_item_type_ptr it = item.lock();
	if(it){
		get_base()->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
T __B_PLUS_TREE_BASEITERATOR_CLASS__::get_value()
{
	child_item_type_ptr it = item.lock();
	if(!it || !it->item)
		throw std::out_of_range("Could Not get value from end()");
	return it->item->second;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
bool __B_PLUS_TREE_BASEITERATOR_CLASS__::expired()
{
	return (bool)(item.lock());
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type __B_PLUS_TREE_BASEITERATOR_CLASS__::operator++()
{	
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	get_base()->processIteratorMoveStart(it, 1);
	
	node_ptr node = nullptr;
	if(it){
		node = it->node.lock();
	}
	
	if(!node){
		node_ptr tmp = get_base()->min_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			it = item.lock();
			
			get_base()->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		
		// Process search end
		get_base()->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(it, 1);
		
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
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process release item
		get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			get_base()->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move End
		get_base()->processIteratorMoveEnd(it, 1);
		
		return *this;
	}
	
	item = node->get(it->pos+1);
	it = item.lock();
	
	// Reserve Item
	get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Process release item
	get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	get_base()->processIteratorMoveEnd(it, 1);
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type __B_PLUS_TREE_BASEITERATOR_CLASS__::operator++(int)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	get_base()->processIteratorMoveStart(it, 1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(it){
		node = it->node.lock();
	}
	
	if(!node){
		node_ptr tmp = get_base()->min_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			it = item.lock();
			
			get_base()->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		get_base()->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(it, 1);
		
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
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			get_base()->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move End
		get_base()->processIteratorMoveEnd(it, 1);
		
		return n;
	}
	
	item = node->get(it->pos+1);
	it = item.lock();
	
	// Reserve Item
	get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	get_base()->processIteratorMoveEnd(it, 1);
	
	return n;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type __B_PLUS_TREE_BASEITERATOR_CLASS__::operator--()
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	get_base()->processIteratorMoveStart(it, -1);
	
	node_ptr node = nullptr;
	if(it){
		node = it->node.lock();
	}
	
	if(!node){
		node_ptr tmp = get_base()->max_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			it = item.lock();
			
			get_base()->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		get_base()->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(it, -1);
		
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
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			get_base()->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move Start
		get_base()->processIteratorMoveEnd(it, -1);
		
		return *this;
	}
	
	item = node->get(it->pos-1);
	it = item.lock();
	
	// Reserve Item
	get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	get_base()->processIteratorMoveEnd(it, -1);
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type __B_PLUS_TREE_BASEITERATOR_CLASS__::operator--(int)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr oit = it;
	// Process Move Start
	get_base()->processIteratorMoveStart(it, -1);
	
	self_type n = *this;
	
	node_ptr node = nullptr;
	if(it){
		node = it->node.lock();
	}
		
	if(!node){
		node_ptr tmp = get_base()->max_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = it;
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			it = item.lock();
			
			get_base()->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		get_base()->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move Start
		get_base()->processIteratorMoveEnd(it, -1);
		
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
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			get_base()->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process Move Start
		get_base()->processIteratorMoveEnd(it, -1);
		
		return n;
	}
	
	item = node->get(it->pos-1);
	it = item.lock();
	
	// Reserve Item
	get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	get_base()->processIteratorMoveEnd(it, -1);
	
	return n;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::reference __B_PLUS_TREE_BASEITERATOR_CLASS__::operator*()
{
	child_item_type_ptr it = item.lock();
	return *(it->item);
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::pointer __B_PLUS_TREE_BASEITERATOR_CLASS__::operator->()
{
	child_item_type_ptr it = item.lock();
	return it->item;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
bool __B_PLUS_TREE_BASEITERATOR_CLASS__::operator==(const self_type &r)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr rit = r.item.lock();
	if(!it && !rit){
		return base == r.base;
	}
	if(!it || !rit){
		return false;
	}
	return (it->node.lock().get() == rit->node.lock().get() && it->pos == rit->pos);
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
bool __B_PLUS_TREE_BASEITERATOR_CLASS__::operator!=(const self_type &r)
{
	child_item_type_ptr it = item.lock();
	child_item_type_ptr rit = r.item.lock();
	if(!it && !rit){
		return base != r.base;
	}
	if(!it || !rit){
		return true;
	}
	return it->node.lock().get() != rit->node.lock().get() || it->pos != rit->pos;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::instance_type* __B_PLUS_TREE_BASEITERATOR_CLASS__::get_base()
{
	return base;
}

#endif // BPLUSTREEBASEITERATOR_H
