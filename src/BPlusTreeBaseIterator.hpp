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
		typedef std::pair<const Key&, T> entry_item;
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
		
		self_type& operator++();
		self_type operator++(int);
		self_type& operator--();
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
		BPlusTreeBaseIterator(childs_type_iterator item, instance_type* base);
		virtual ~BPlusTreeBaseIterator();
		
		Key get_key();
		T get_value();
		bool expired();
		
	protected:
		virtual instance_type* get_base();
		virtual child_item_type_ptr get_item(childs_type_iterator it);
		childs_type_iterator item;
		instance_type* base;
};


__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator(const self_type& iter)
{
	this->item = iter.item;
	this->base = iter.base;
	
	
	child_item_type_ptr it = get_item(item);
	
	if(it){
		get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type& __B_PLUS_TREE_BASEITERATOR_CLASS__::operator=(const self_type& iter)
{
	child_item_type_ptr it = get_item(item);
	if(it){
		get_base()->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = iter.item;
	this->base = iter.base;
	
	it = get_item(item);
	
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
	iter.item = nullptr;
	iter.base = nullptr;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type& __B_PLUS_TREE_BASEITERATOR_CLASS__::operator=(self_type&& iter)
{
	child_item_type_ptr it = get_item(item);
	if(it){
		get_base()->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
	
	this->item = std::move(iter.item);
	this->base = std::move(iter.base);
	iter.item = nullptr;
	iter.base = nullptr;
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator()
{
	this->base = nullptr;
	this->item = nullptr;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::BPlusTreeBaseIterator(childs_type_iterator item, instance_type* base)
{
	// Item already should be reserved
	this->item = item;
	this->base = base;
	
	child_item_type_ptr it = get_item(item);
	
	if(it){
		get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	}
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
__B_PLUS_TREE_BASEITERATOR_CLASS__::~BPlusTreeBaseIterator()
{
	child_item_type_ptr it = get_item(item);
	if(it){
		get_base()->processItemRelease(it, instance_type::PROCESS_TYPE::READ);
	}
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
Key __B_PLUS_TREE_BASEITERATOR_CLASS__::get_key()
{
	child_item_type_ptr it = get_item(item);
	if(!it || !it->item)
		throw std::out_of_range("Could Not get key from end()");
	return it->item->first;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
T __B_PLUS_TREE_BASEITERATOR_CLASS__::get_value()
{
	child_item_type_ptr it = get_item(item);
	if(!it || !it->item)
		throw std::out_of_range("Could Not get value from end()");
	return it->item->second;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
bool __B_PLUS_TREE_BASEITERATOR_CLASS__::expired()
{
	return !((bool)(get_item(item)));
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type& __B_PLUS_TREE_BASEITERATOR_CLASS__::operator++()
{	
	child_item_type_ptr it = get_item(item);
	child_item_type_ptr oit = it;
	// Process Move Start
	get_base()->processIteratorMoveStart(item, 1);
	
	node_ptr node = nullptr;
	if(it){
		node = it->node.lock();
	}
	
	if(!node){
		node_ptr tmp = get_base()->min_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = nullptr;
		}
		else{
			// Item here is already reserved through the min_node method
			item = tmp->first_child();
			it = get_item(item);
			
			get_base()->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);

			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		
		// Process search end
		get_base()->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(item, 1);
		
		return *this;
	}
	if(node->get_childs()->find_next(item) == node->childs_iterator_end()){
		
		node_ptr onode = node;
		
		// Reserve next leaf
		get_base()->processOffsetLeafReserve(node, 1);
		
		node = node->next_leaf();
		
		it = nullptr;
		item = nullptr;
		
		if(node){		
			item = node->first_child();
			
			it = get_item(item);
			
			// Reserve Item
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process release item
		get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			get_base()->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release previous node
		get_base()->processOffsetLeafRelease(onode, 0);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(item, 1);
		
		return *this;
	}
	
	item = node->get_childs()->find_next(item);
	it = get_item(item);
	
	// Reserve Item
	get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Process release item
	get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move End
	get_base()->processIteratorMoveEnd(item, 1);
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type __B_PLUS_TREE_BASEITERATOR_CLASS__::operator++(int)
{
	self_type n = *this;
	
	++*this;
	
	return n;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type& __B_PLUS_TREE_BASEITERATOR_CLASS__::operator--()
{
	child_item_type_ptr it = get_item(item);
	child_item_type_ptr oit = it;
	// Process Move Start
	get_base()->processIteratorMoveStart(item, -1);
	
	node_ptr node = nullptr;
	if(it){
		node = it->node.lock();
	}
	
	if(!node){
		node_ptr tmp = get_base()->max_node();
		
		if(!tmp->size()){
			it = nullptr;
			item = nullptr;
		}
		else{
			// Item reserved through the max_node method
			item = tmp->last_child();
			it = get_item(item);
			
			get_base()->processLeafReserve(tmp, instance_type::PROCESS_TYPE::READ);
			
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Process search end
		get_base()->processSearchNodeEnd(tmp, instance_type::PROCESS_TYPE::READ);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(item, -1);
		
		return *this;
	}
	if(node->get_childs()->find_prev(item) == node->childs_iterator_end()){
		
		node_ptr onode = node;
		
		// Reserve next leaf
		get_base()->processOffsetLeafReserve(node, -1);
		
		node = node->prev_leaf();
		
		it = nullptr;
		item = nullptr;
		
		if(node){		
			item = node->last_child();
			it = get_item(item);
			
			// Relerve Item
			get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release current item
		get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
		if(!node){
			get_base()->processLeafRelease(onode, instance_type::PROCESS_TYPE::READ);
		}
		
		// Release previous node
		get_base()->processOffsetLeafRelease(onode, 0);
		
		// Process Move End
		get_base()->processIteratorMoveEnd(item, -1);
		
		return *this;
	}
	
	item = node->get_childs()->find_prev(item);
	it = get_item(item);
	
	// Reserve Item
	get_base()->processItemReserve(it, instance_type::PROCESS_TYPE::READ);
	
	// Release current item
	get_base()->processItemRelease(oit, instance_type::PROCESS_TYPE::READ);
	
	// Process Move Start
	get_base()->processIteratorMoveEnd(item, -1);
	
	return *this;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::self_type __B_PLUS_TREE_BASEITERATOR_CLASS__::operator--(int)
{
	self_type n = *this;
	
	--*this;
	
	return n;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::reference __B_PLUS_TREE_BASEITERATOR_CLASS__::operator*()
{
	child_item_type_ptr it = get_item(item);
	return *(it->item);
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::pointer __B_PLUS_TREE_BASEITERATOR_CLASS__::operator->()
{
	child_item_type_ptr it = get_item(item);
	return it->item;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
bool __B_PLUS_TREE_BASEITERATOR_CLASS__::operator==(const self_type &r)
{
	if(!item && !r.item){
		return base == r.base;
	}
	if(!item || !r.item){
		return false;
	}
	return item == r.item;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
bool __B_PLUS_TREE_BASEITERATOR_CLASS__::operator!=(const self_type &r)
{
	if(!item && !r.item){
		return base != r.base;
	}
	if(!item || !r.item){
		return true;
	}
	return item != r.item;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::instance_type* __B_PLUS_TREE_BASEITERATOR_CLASS__::get_base()
{
	return base;
}

__B_PLUS_TREE_ITERATOR_TEMPLATE__
typename __B_PLUS_TREE_BASEITERATOR_CLASS__::child_item_type_ptr __B_PLUS_TREE_BASEITERATOR_CLASS__::get_item(childs_type_iterator it)
{
	if(!it){
		return nullptr;
	}
	return it->data;
}

#endif // BPLUSTREEBASEITERATOR_H
