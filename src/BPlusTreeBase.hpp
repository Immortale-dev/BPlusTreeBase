#ifndef BPLUSTREEBASE_H
#define BPLUSTREEBASE_H

#include <atomic>
#include <memory>
#include <cassert>
#include <list>
#include <functional>
#include <iostream>
#include "BPlusTreeBaseUtils.hpp"
#include "BPlusTreeBaseNode.hpp"
#include "BPlusTreeBaseInternalNode.hpp"
#include "BPlusTreeBaseLeafNode.hpp"
#include "BPlusTreeBaseIterator.hpp"
#include "BPlusTreeBaseBranch.hpp"

__B_PLUS_TREE_BASE_TEMPLATE__
class BPlusTreeBase : public BPlusTreeBase__Interface<Key, T>
{
	public:
		friend ITERATOR;
		
		typedef BPlusTreeBase__Interface<Key, T> Interface;
		
		typedef typename Interface::LEAF_REF LEAF_REF;
		typedef typename Interface::PROCESS_TYPE PROCESS_TYPE;

		typedef BPlusTreeBaseNode<Key, T> Node;
		typedef ITERATOR iterator;
		typedef INTERNAL InternalNode;
		typedef LEAF LeafNode;
		typedef std::pair<const Key&, T> EntryItem;
		typedef std::shared_ptr<EntryItem> EntryItem_ptr;
		typedef std::pair<Key, T> value_type;
		typedef std::pair<iterator, bool> insert_return_type;
		typedef typename Node::childs_type_iterator childs_type_iterator;	
		typedef typename Node::child_type child_type;	
		typedef std::shared_ptr<Node> node_ptr;
		typedef std::weak_ptr<Node> node_weak;
		typedef typename Node::child_item_type child_item_type;
		typedef typename Node::child_item_type_ptr childs_item_ptr;
		typedef __B_PLUS_TREE_BASE_CLASS__ self_type;
		typedef std::list<node_ptr> node_list;
		typedef std::function<int(const Key& left, const Key& right)> compare_t;
		
		BPlusTreeBase(const self_type& obj);
		BPlusTreeBase(int factor);
		
		virtual ~BPlusTreeBase();
		void erase(Key key);
		void erase(iterator it);
		void clear();
		void insert(value_type item, bool overwrite = false);
		iterator find(Key key);
		iterator begin();
		iterator end();
		iterator lower_bound(Key key);
		iterator upper_bound(Key key);
		long long int size();
		T& operator[](Key key);
		
	
	// DEBUG METHODS
	#ifdef DEBUG
		std::atomic<int> mutex_count;
		std::atomic<int> item_reserve_count;
		int reserved_count = 0;
		int iterator_move_count = 0;
		void print_tree();
		void print_tree(node_ptr node, std::string tabs);
		std::vector<Key> bfs_result();
		void bfs_result(node_ptr node, std::vector<Key>& res);
		int get_mutex_count();
	#endif

	protected:
		const Key get_entry_key(childs_item_ptr item);
		node_ptr min_node();
		node_ptr min_node(node_ptr node);
		node_ptr max_node();
		node_ptr max_node(node_ptr node);
		node_ptr create_internal_node();
		node_ptr create_leaf_node();
		node_ptr get_root();
		node_ptr get_stem();
		bool is_root(node_ptr node);
		bool is_stem(node_ptr node);
		bool insert_req(node_ptr node, node_ptr parent, childs_type_iterator item, node_ptr& ins, bool overwrite, node_list& list);
		bool erase_req(node_ptr node, node_ptr parent, const Key &key, node_list& list);
		void clear_req(node_ptr node);
		bool is_leaf(node_ptr node);
		void set_root(node_ptr node);
		void release_node(node_ptr node);
		void update_positions(node_ptr node);
		void update_item_node(node_ptr node, childs_item_ptr item);
		void update_leaf_ref(node_ptr node, node_ptr ref_node, LEAF_REF ref);
		void init();
		virtual void processSearchNodeStart(node_ptr node, PROCESS_TYPE type);
		virtual void processSearchNodeEnd(node_ptr node, PROCESS_TYPE type);
		virtual void processInsertNode(node_ptr node);
		virtual void processDeleteNode(node_ptr node);
		virtual void processIteratorMoveStart(childs_type_iterator item, int step);
		virtual void processIteratorMoveEnd(childs_type_iterator item, int step);
		virtual void processItemReserve(childs_item_ptr item, PROCESS_TYPE type);
		virtual void processItemRelease(childs_item_ptr item, PROCESS_TYPE type);
		virtual void processItemMove(node_ptr node, childs_item_ptr item);
		virtual void processLeafReserve(node_ptr node, PROCESS_TYPE type);
		virtual void processLeafRelease(node_ptr node, PROCESS_TYPE type);
		virtual void processOffsetLeafReserve(node_ptr node, int offset);
		virtual void processOffsetLeafRelease(node_ptr node, int offset);
		virtual void processLeafInsertItem(node_ptr node, childs_item_ptr item);
		virtual void processLeafDeleteItem(node_ptr node, childs_item_ptr item);
		virtual void processLeafSplit(node_ptr node, node_ptr new_node, node_ptr link_node);
		virtual void processLeafJoin(node_ptr node, node_ptr join_node, node_ptr link_node);
		virtual void processLeafShift(node_ptr node, node_ptr shift_node);
		virtual void processLeafFree(node_ptr node);
		virtual void processLeafLock(node_ptr node);
		virtual void processLeafRef(node_ptr node, node_ptr ref_node, LEAF_REF ref);
		
		void release_entry_item(childs_type_iterator item);
		childs_type_iterator create_entry_item(Key key, T val);
		
		node_ptr root;
		childs_type_iterator last;
		node_ptr stem;
		std::atomic<long long int> v_count;
		const int factor;
		compare_t compare_fn = [](const Key& left, const Key& right)->int{if (left == right) return 0; return left < right ? -1 : 1;};
};

__B_PLUS_TREE_BASE_TEMPLATE__
__B_PLUS_TREE_BASE_CLASS__::BPlusTreeBase(const self_type& obj) : factor(obj.factor)
{
	root = obj.root;
	last = obj.last;
	stem = obj.stem;
	v_count = obj.v_count.load();
	#ifdef DEBUG
	mutex_count = obj.mutex_count.load();
	item_reserve_count = obj.item_reserve_count.load();
	#endif
}

__B_PLUS_TREE_BASE_TEMPLATE__
__B_PLUS_TREE_BASE_CLASS__::BPlusTreeBase(int f) : factor(f)
{
	init();
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::init()
{
	stem = create_internal_node();
	node_ptr node = create_leaf_node();
	set_root(node);
	last = nullptr;
	v_count = 0;
	#ifdef DEBUG
	mutex_count = 0;
	item_reserve_count = 0;
	#endif
}

__B_PLUS_TREE_BASE_TEMPLATE__
__B_PLUS_TREE_BASE_CLASS__::~BPlusTreeBase()
{
	if(!get_root())
		return;
	release_node(root);
	root = nullptr;
	stem = nullptr;
}

__B_PLUS_TREE_BASE_TEMPLATE__
const Key __B_PLUS_TREE_BASE_CLASS__::get_entry_key(childs_item_ptr item)
{
	return item->item->first;
}

__B_PLUS_TREE_BASE_TEMPLATE__
long long int __B_PLUS_TREE_BASE_CLASS__::size()
{
	return v_count.load();
}

__B_PLUS_TREE_BASE_TEMPLATE__
T& __B_PLUS_TREE_BASE_CLASS__::operator[](Key key)
{
	iterator it = lower_bound(key);
	if(it == end() || it.get_key() != key){
		insert(value_type(key,{}));
		it = find(key);
	}
	return it->second;
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::iterator __B_PLUS_TREE_BASE_CLASS__::find(Key k)
{
	const Key key = k;
	node_ptr node = get_stem();
	processSearchNodeStart(node, PROCESS_TYPE::READ);
	while(true){
		if(!node){
			processSearchNodeEnd(node, PROCESS_TYPE::READ);
			return end();
		}
		if(node->is_leaf()){
			iterator it = end();
			if(node->exists(key)){
				it = iterator(node->get_iterator(key), static_cast<Interface*>(this));
			}
			processSearchNodeEnd(node, PROCESS_TYPE::READ);
			return it;
		}
		node_ptr tnode = node->find(key);
		processSearchNodeStart(tnode, PROCESS_TYPE::READ);
		processSearchNodeEnd(node, PROCESS_TYPE::READ);
		node = tnode;
	}
	return end();
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::insert(value_type item, bool overwrite)
{
	childs_type_iterator itm = create_entry_item(item.first, item.second);

	node_ptr node = get_stem();
	node_ptr ins = nullptr;
	
	// Create path list
	node_list list;
	list.push_back(node);
	
	insert_req(node, nullptr, itm, ins, overwrite, list);
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::erase(Key item)
{
	node_ptr node = get_stem();
	node_list list;
	list.push_back(node);
	erase_req(node, nullptr, item, list);
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::erase(iterator it)
{
	if(it == end())
		return;
	erase(it.get_key());
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::clear()
{
	node_ptr node = get_stem();
	clear_req(node);
	v_count = 0;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::clear_req(node_ptr node)
{
	processSearchNodeStart(node, PROCESS_TYPE::WRITE);
	if(node->is_leaf()){
		
		// Lock leaf
		processLeafLock(node);
		
		// Delete node if leaf
		processDeleteNode(node);
		node->get_childs()->clear();
		
		update_leaf_ref(node, nullptr, LEAF_REF::PREV);
		update_leaf_ref(node, nullptr, LEAF_REF::NEXT);
	}
	else{
		// Iterate over child nodes
		for(auto& n : (*node->get_nodes())){
			clear_req(n);
		}
		// Delete internal node
		node->get_keys()->resize(0);
		node->get_nodes()->resize(0);
		if(!is_stem(node)){
			processDeleteNode(node);
		}
	}
	if(is_stem(node)){
		node_ptr n = create_leaf_node();
		set_root(n);
	}
	if(node->is_leaf()){
		processLeafFree(node);
	}
	processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
	if(!is_stem(node)){
		release_node(node);
	}
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::iterator __B_PLUS_TREE_BASE_CLASS__::begin()
{
	if(!size()){
		return end();
	}
	
	node_ptr node = min_node();
	
	iterator it(node->first_child(),static_cast<Interface*>(this));
	processSearchNodeEnd(node, PROCESS_TYPE::READ);
	
	return it;
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::iterator __B_PLUS_TREE_BASE_CLASS__::lower_bound(Key k)
{
	if(!size()){
		return end();
	}
	
	const Key key = k;
	node_ptr node = get_stem();
	processSearchNodeStart(node, PROCESS_TYPE::READ);
	while(true){
		if(!node){
			processSearchNodeEnd(node, PROCESS_TYPE::READ);
			return end();
		}
		if(node->is_leaf()){
			iterator it = iterator(node->get_childs()->lower_bound(key), static_cast<Interface*>(this));
			processSearchNodeEnd(node, PROCESS_TYPE::READ);
			return it;
		}
		node_ptr tnode = node->find(key);
		processSearchNodeStart(tnode, PROCESS_TYPE::READ);
		processSearchNodeEnd(node, PROCESS_TYPE::READ);
		node = tnode;
	}
	return end();
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::iterator __B_PLUS_TREE_BASE_CLASS__::upper_bound(Key k)
{
	if(!size()){
		return end();
	}
	
	iterator it = lower_bound(k);
	if(it.get_key() <= k){
		++it;
	}
	return it;
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::iterator __B_PLUS_TREE_BASE_CLASS__::end()
{
	return iterator(last, static_cast<Interface*>(this));
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::min_node()
{
	node_ptr node = get_stem();
	
	// process node before search
	processSearchNodeStart(node, PROCESS_TYPE::READ);
	
	return min_node(node);
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::min_node(node_ptr node)
{
	if(is_leaf(node)){
		return node;
	}
	
	// Get left most node
	node_ptr nnode = node->first_child_node();
	
	// process node before search
	processSearchNodeStart(nnode, PROCESS_TYPE::READ);
	
	// process node after search
	processSearchNodeEnd(node, PROCESS_TYPE::READ);
	
	// recoursively search for min leaf node
	return min_node(nnode);
}

__B_PLUS_TREE_BASE_TEMPLATE__
bool __B_PLUS_TREE_BASE_CLASS__::is_root(node_ptr node)
{
	return root.get() == node.get();
}

__B_PLUS_TREE_BASE_TEMPLATE__
bool __B_PLUS_TREE_BASE_CLASS__::is_stem(node_ptr node)
{
	return stem.get() == node.get();
}

__B_PLUS_TREE_BASE_TEMPLATE__
bool __B_PLUS_TREE_BASE_CLASS__::is_leaf(node_ptr node)
{
	return node->is_leaf();
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::max_node()
{
	node_ptr node = get_stem();
	
	// process node before search
	processSearchNodeStart(node, PROCESS_TYPE::READ);
	
	return max_node(node);
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::max_node(node_ptr node)
{
	if(is_leaf(node)){
		return node;
	}
	
	// Get right most node
	node_ptr nnode = node->last_child_node();
	
	// Process next node before releasing previous one
	processSearchNodeStart(nnode, PROCESS_TYPE::READ);

	// process node after search
	processSearchNodeEnd(node, PROCESS_TYPE::READ);
	
	// recoursively search for max leaf node
	return max_node(nnode);
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::set_root(node_ptr node)
{
	root = node;
	if(stem->nodes_size()){
		stem->remove_nodes(0);
	}
	stem->add_nodes(0,node);
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::create_internal_node()
{
	// TODO: replace with allocator functions
	return node_ptr(new InternalNode());
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::create_leaf_node()
{
	// TODO: replace with allocator functions
	return node_ptr(new LeafNode());
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::release_node(node_ptr node)
{
	if(node->is_leaf()){
		update_leaf_ref(node, nullptr, LEAF_REF::PREV);
		update_leaf_ref(node, nullptr, LEAF_REF::NEXT);
	}
	// TODO: replace with allocator functions
	// No need to delete smart ptr
	// delete node;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::update_positions(node_ptr node)
{
	childs_type_iterator it = node->childs_iterator();
	while(it != node->childs_iterator_end()){
		update_item_node(node, it->data);
		it = node->get_childs()->find_next(it);
	}
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::update_item_node(node_ptr node, childs_item_ptr item)
{
	processItemMove(node, item);
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::update_leaf_ref(node_ptr node, node_ptr ref_node, LEAF_REF ref)
{
	processLeafRef(node, ref_node, ref);
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::get_root()
{
	return root;
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::node_ptr __B_PLUS_TREE_BASE_CLASS__::get_stem()
{
	return stem;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processSearchNodeStart(node_ptr node, PROCESS_TYPE type)
{
	#ifdef DEBUG
		mutex_count++;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processSearchNodeEnd(node_ptr node, PROCESS_TYPE type)
{
	#ifdef DEBUG
		mutex_count--;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processItemReserve(childs_item_ptr item, PROCESS_TYPE type)
{
	#ifdef DEBUG
		item_reserve_count++;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processItemRelease(childs_item_ptr item, PROCESS_TYPE type)
{
	#ifdef DEBUG
		item_reserve_count--;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafReserve(node_ptr node, PROCESS_TYPE type)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafRelease(node_ptr node, PROCESS_TYPE type)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processOffsetLeafReserve(node_ptr node, int offset)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processOffsetLeafRelease(node_ptr node, int offset)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafInsertItem(node_ptr node, childs_item_ptr item)
{
	#ifdef DEBUG
	if(item){
		item_reserve_count++;
	}
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafDeleteItem(node_ptr node, childs_item_ptr item)
{
	#ifdef DEBUG
		item_reserve_count++;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafSplit(node_ptr node, node_ptr new_node, node_ptr link_node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafJoin(node_ptr node, node_ptr join_node, node_ptr link_node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafShift(node_ptr node, node_ptr shift_node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafFree(node_ptr node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafLock(node_ptr node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processLeafRef(node_ptr node, node_ptr ref_node, LEAF_REF ref)
{
	if(ref == LEAF_REF::NEXT){
		node->set_next_leaf(ref_node);
	} else {
		node->set_prev_leaf(ref_node);
	}
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processItemMove(node_ptr node, childs_item_ptr item)
{
	item->node = node_weak(node);
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processInsertNode(node_ptr node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processDeleteNode(node_ptr node)
{
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processIteratorMoveStart(childs_type_iterator item, int step)
{
	#ifdef DEBUG
		iterator_move_count++;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::processIteratorMoveEnd(childs_type_iterator item, int step)
{
	#ifdef DEBUG
		iterator_move_count--;
	#endif
	return;
}

__B_PLUS_TREE_BASE_TEMPLATE__
typename __B_PLUS_TREE_BASE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASE_CLASS__::create_entry_item(Key key, T val)
{
	childs_type_iterator it = new typename child_type::Node();
	it->key = key;
	EntryItem_ptr itp = EntryItem_ptr(new EntryItem(it->key, val));
	it->data = childs_item_ptr(new child_item_type());
	it->data->item = itp;
	return it;
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::release_entry_item(childs_type_iterator item)
{
	delete item;
}

__B_PLUS_TREE_BASE_TEMPLATE__
bool __B_PLUS_TREE_BASE_CLASS__::erase_req(node_ptr node, node_ptr parent, const Key& key, node_list& list)
{
	// Process Node before search
	processSearchNodeStart(node, PROCESS_TYPE::WRITE);

	bool nodeChanged = false;
	
	// Remove and end with all nodes that will never be modified
	if(node->size() > factor){
		node_ptr comp = node;
		while(list.front().get() != comp.get()){
			node_ptr n = list.front();
			processSearchNodeEnd(n, PROCESS_TYPE::WRITE);
			list.pop_front();
		}
	}

	if(is_leaf(node)){
		// Remove current node from path list
		list.pop_back();
		
		if(!node->exists(key)){
			processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
			return false;
		}
		
		childs_type_iterator itm = node->get_iterator(key);
		
		// Get item itself
		childs_item_ptr ritm = itm->data;
		
		// Leaf change operations
		processLeafDeleteItem(node, ritm);
		
		// Release item
		release_entry_item(node->erase(itm));
		
		// process release item
		processItemRelease(ritm, PROCESS_TYPE::WRITE);
		
		// process free leaf
		processLeafFree(node);
		
		v_count--;
		nodeChanged = true;
	}
	else{
		// Find next node
		node_ptr n = node->find(key);
		
		// Add node to the path list
		list.push_back(n);
		
		nodeChanged = erase_req(n, node, key, list);
		
		// If no nodes left return as there is nothing to do
		if(list.empty()){
			return false;
		}
		
		// Remove current node from path list
		list.pop_back();
		
		if(is_stem(node)){
			// It is stem. No any operations required
			processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
			return false;
		}
	}

	if(node->size() >= factor || is_root(node)){
		
		if(!is_leaf(node) && node->size() == 1){

			node_ptr child = node->first_child_node();
			
			// Process as this node will became as root
			processSearchNodeStart(child, PROCESS_TYPE::WRITE);

			// Set new root
			set_root(child);
			
			// Save Child as root
			processInsertNode(child);
			
			// Leave new root node
			processSearchNodeEnd(child, PROCESS_TYPE::WRITE);

			// Delete old root node
			node->remove_nodes(0);
			processDeleteNode(node);
			
			// Process node after search
			processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
			
			// Clear node memory
			release_node(node);

			// Parent Node was not changed
			return false;
		}

		if(nodeChanged){
			// Process updated node
			processInsertNode(node);
		}
		// Process node after search
		processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
		
		// Parent node was not changed
		return false;
	}

	node_ptr nodeLeft;
	node_ptr nodeRight;

	nodeLeft = parent->find_prev(key);
	if(nodeLeft){
		// Start working with left node
		processSearchNodeStart(nodeLeft, PROCESS_TYPE::WRITE);
	}
	if(nodeLeft && nodeLeft->size() > factor){
		
		if(is_leaf(node)){
			// Avoid dead locks
			processLeafShift(node, nodeLeft);
		}

		// Shift nodes
		node->shift_left(parent);
		
		if(is_leaf(node)){
			// Update item node
			update_item_node(node, node->first_child()->data);
			
			// Free nodes
			processLeafFree(node);
			processLeafFree(nodeLeft);
		}

		// Process node
		processInsertNode(node);
		processSearchNodeEnd(node, PROCESS_TYPE::WRITE);

		// Process updated node
		processInsertNode(nodeLeft);
		processSearchNodeEnd(nodeLeft, PROCESS_TYPE::WRITE);
		
		// Parent node changed
		return true;
	}
	
	nodeRight = parent->find_next(key);
	if(nodeRight){
		// Start working with right node
		processSearchNodeStart(nodeRight, PROCESS_TYPE::WRITE);
	}
	if(nodeRight && nodeRight->size() > factor){
		
		if(nodeLeft){
			// process finish with leftNode
			processSearchNodeEnd(nodeLeft, PROCESS_TYPE::WRITE);
		}
		
		if(is_leaf(node)){
			// Avoid dead locks
			processLeafShift(node, nodeRight);
		}

		// Shift nodes
		node->shift_right(parent);
		
		if(is_leaf(node)){
			// Update item node
			update_item_node(node, node->last_child()->data);
			
			// Free nodes
			processLeafFree(node);
			processLeafFree(nodeRight);
		}

		// Process node
		processInsertNode(node);
		processSearchNodeEnd(node, PROCESS_TYPE::WRITE);

		// Process updated node
		processInsertNode(nodeRight);
		processSearchNodeEnd(nodeRight, PROCESS_TYPE::WRITE);
		
		// Parent node changed
		return true;
	}
	
	node_ptr joinNode = nodeRight ? nodeRight : nodeLeft;
	
	bool join_r;
	bool swapped = false;
	
	if(nodeRight){
		join_r = true;
		if(parent->last_child_node().get() == nodeRight.get()){
			swap(node, nodeRight);
			joinNode = nodeRight;
			swapped = true;
			join_r = false;
		}
	}
	else{
		join_r = false;
		if(parent->first_child_node().get() == nodeLeft.get()){
			swap(node, nodeLeft);
			joinNode = nodeLeft;
			swapped = true;
			join_r = true;
		}
	}
	if(join_r){
		if(is_leaf(node)){
			// Avoid dead locks
			node_ptr nextLeaf = swapped ? nodeRight : joinNode->next_leaf();
			if(nextLeaf && !swapped){
				processSearchNodeStart(nextLeaf, PROCESS_TYPE::WRITE);
			}
			processLeafJoin(node, joinNode, nextLeaf);
		}
		
		node->join_right(parent);
		
		if(is_leaf(node)){
			// Update positions
			update_positions(node);
			
			node_ptr nextLeaf = swapped ? nodeRight : joinNode->next_leaf();
			
			if(nextLeaf){
				update_leaf_ref(nextLeaf, node, LEAF_REF::PREV);
				
				// Free prev leaf
				processLeafFree(nextLeaf);
				
				processInsertNode(nextLeaf);
				if(!swapped){
					processSearchNodeEnd(nextLeaf, PROCESS_TYPE::WRITE);
				}
			}
			update_leaf_ref(node, nextLeaf, LEAF_REF::NEXT);
			
			// Free node
			processLeafFree(node);
		}
	}
	else{
		if(is_leaf(node)){
			// Avoid dead locks
			node_ptr prevLeaf = swapped ? nodeLeft : joinNode->prev_leaf();
			if(prevLeaf && !swapped){
				processSearchNodeStart(prevLeaf, PROCESS_TYPE::WRITE);
			}
			processLeafJoin(node, joinNode, prevLeaf);
		}
		
		node->join_left(parent);
		
		if(is_leaf(node)){
			// Update positions
			update_positions(node);
			
			node_ptr prevLeaf = swapped ? nodeLeft : joinNode->prev_leaf();
			
			if(prevLeaf){
				update_leaf_ref(prevLeaf, node, LEAF_REF::NEXT);
				
				// Free prev leaf
				processLeafFree(prevLeaf);
				
				processInsertNode(prevLeaf);
				if(!swapped){
					processSearchNodeEnd(prevLeaf, PROCESS_TYPE::WRITE);
				}
			}
			update_leaf_ref(node, prevLeaf, LEAF_REF::PREV);
			
			// Free node
			processLeafFree(node);
		}
	}
	
	if(is_leaf(node)){
		update_leaf_ref(joinNode, nullptr, LEAF_REF::PREV);
		update_leaf_ref(joinNode, nullptr, LEAF_REF::NEXT);
		
		// Free join node
		processLeafFree(joinNode);
	}

	// Save node
	processInsertNode(node);
	
	// Delete useless node
	processDeleteNode(joinNode);
	
	// End processing with side nodes
	if(nodeLeft){
		processSearchNodeEnd(nodeLeft, PROCESS_TYPE::WRITE);
	}
	if(nodeRight){
		processSearchNodeEnd(nodeRight, PROCESS_TYPE::WRITE);
	}
	
	processSearchNodeEnd(node, PROCESS_TYPE::WRITE);

	// Clear node memory
	release_node(joinNode);

	// Parent node changed
	return true;
}

__B_PLUS_TREE_BASE_TEMPLATE__
bool __B_PLUS_TREE_BASE_CLASS__::insert_req(node_ptr node, node_ptr parent, childs_type_iterator item, node_ptr& ins, bool overwrite, node_list& list)
{	
	// Process Node before search
	processSearchNodeStart(node, PROCESS_TYPE::WRITE);

	bool nodeChanged = false;
	
	// Remove and end with all nodes from list that will never be modified
	if(node->size() < factor*2-1){
		node_ptr comp = node;
		while(list.front().get() != comp.get()){
			node_ptr n = list.front();
			processSearchNodeEnd(n, PROCESS_TYPE::WRITE);
			list.pop_front();
		}
	}
	
	const Key& key = item->key;
	if(is_leaf(node)){
		// Remove current node from path list
		list.pop_back();
		
		ins = node;
		
		if(!node->exists(key)){
			// Leaf change insert item
			processLeafInsertItem(node, item->data);
			
			v_count++;			
			node->insert(item);
			
			// Update item node
			update_item_node(node, item->data);
			
			// Free item
			processItemRelease(item->data, PROCESS_TYPE::WRITE);
		
			// Free leaf item change
			processLeafFree(node);
			
			nodeChanged = true;
		}
		else if(overwrite){
			// Get item
			childs_type_iterator it_i = node->get_iterator(key);
			childs_item_ptr itm = it_i->data;
			
			// Leaf change insert item
			processLeafInsertItem(node, itm);
			
			itm->item->second = item->data->item->second;
			
			// Free memory for duplicated item
			release_entry_item(item);
			
			// Free Item
			processItemRelease(itm, PROCESS_TYPE::WRITE);
		
			// Free leaf item change
			processLeafFree(node);
			
			nodeChanged = true;
		} else {
			// Free memory for duplicated item
			release_entry_item(item);
		}
	}	
	else{
		// Find next node
		node_ptr n = node->find(key);
		
		// Add node to the path list
		list.push_back(n);
		
		nodeChanged = insert_req(node->find(key), node, item, ins, overwrite, list);
		
		// If no nodes left, return as there is nothing to do
		if(list.empty()){
			return false;
		}
		
		// Remove this node from path
		list.pop_back();
		
		if(is_stem(node)){
			// It is stem. No any operations required
			processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
			return false;
		}
	}
	
	node_ptr nnode = nullptr;

	if(node->size() >= factor*2){
		// Ensure that parent node is locked
		assert(!list.empty());
		
		// Parent node will be changed
		nodeChanged = true;

		// Create new node to split to
		nnode = is_leaf(node) ? create_leaf_node() : create_internal_node();
		processSearchNodeStart(nnode, PROCESS_TYPE::WRITE);
		
		// Create new parent
		if(is_root(node)){
			parent = create_internal_node();
			processSearchNodeStart(parent, PROCESS_TYPE::WRITE);
			parent->add_nodes(0, node);
		}
		
		if(is_leaf(node)){
			// Process all nodes simulteniously to avoid dead locks
			node_ptr link_node;
			link_node = parent->first_child_node().get() == node.get() ? node->next_leaf() : node->prev_leaf();
			if(link_node){
				processSearchNodeStart(link_node, PROCESS_TYPE::WRITE);
			}
			processLeafSplit(node, nnode, link_node);
		}
		
		// Split node to nnode
		node->split(nnode, parent);
		
		if(is_leaf(node)){
			// Update positions
			update_positions(nnode);
		}
		
		// Update inserted node ref if necessary
		if(is_leaf(node) && !node->exists(key)){
			ins = nnode;
		}
		
		// Update leaf nodes dependencies
		if(is_leaf(node)){
			if(parent->first_child_node().get() == node.get()){
				// New node is next to current node
				
				// Update dependencies of newly created node
				node_ptr nextLeaf = node->next_leaf();
				update_leaf_ref(nnode, nextLeaf, LEAF_REF::NEXT);
				update_leaf_ref(nnode, node, LEAF_REF::PREV);
				
				// Free new node
				processLeafFree(nnode);

				// Process newly created node
				processInsertNode(nnode);
				// Process Node after search
				processSearchNodeEnd(nnode, PROCESS_TYPE::WRITE);
				
				update_leaf_ref(node, nnode, LEAF_REF::NEXT);
				
				// Free node
				processLeafFree(node);
				
				// Update next leaf
				if(nextLeaf){
					update_leaf_ref(nextLeaf, nnode, LEAF_REF::PREV);
					
					// Free link node
					processLeafFree(nextLeaf);
					
					processInsertNode(nextLeaf);
					processSearchNodeEnd(nextLeaf, PROCESS_TYPE::WRITE);
				}
			}
			else{
				// New node is before the current node
				
				// Update dependencies of newly created node
				node_ptr prevLeaf = node->prev_leaf();
				update_leaf_ref(nnode, prevLeaf, LEAF_REF::PREV);
				update_leaf_ref(nnode, node, LEAF_REF::NEXT);
				
				// Free new node
				processLeafFree(nnode);

				// Process newly created node
				processInsertNode(nnode);
				// Process Node after search
				processSearchNodeEnd(nnode, PROCESS_TYPE::WRITE);
				
				update_leaf_ref(node, nnode, LEAF_REF::PREV);
				
				// Free new node
				processLeafFree(node);
				
				// Update prev node
				if(prevLeaf){
					update_leaf_ref(prevLeaf, nnode, LEAF_REF::NEXT);
					
					// Free link node
					processLeafFree(prevLeaf);
					
					processInsertNode(prevLeaf);
					processSearchNodeEnd(prevLeaf, PROCESS_TYPE::WRITE);
				}
			}
		}
		else{
			// Process newly created node
			processInsertNode(nnode);
			// Process Node after search
			processSearchNodeEnd(nnode, PROCESS_TYPE::WRITE);
		}
		
		// Process updated Node before parent
		processInsertNode(node);
		// Process Node after search before parent
		processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
		
		if(is_root(node)){
			set_root(parent);
			// Process updated Node
			processInsertNode(parent);
			// Process Node after search
			processSearchNodeEnd(parent, PROCESS_TYPE::WRITE);
		}
		return true;
	}

	if(nodeChanged){
		// Process updated Node
		processInsertNode(node);
	}

	// Process Node after search
	processSearchNodeEnd(node, PROCESS_TYPE::WRITE);

	return !!nnode;

}

#ifdef DEBUG
__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::print_tree()
{
	print_tree(get_root(), "");
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::print_tree(node_ptr node, std::string tabs)
{
	using namespace std;
	if(is_leaf(node)){
		cout << tabs << "[ ";
		auto it = node->childs_iterator();
		for(int i=0;i<node->size();i++){
			cout << (*it)->first << " ";
			++it;
		}
		cout << "]" << endl;
		return;
	}
	
	cout << tabs << "( ";
	auto itk = node->keys_iterator();
	for(int i=0;i<node->size()-1;i++){
		cout << *itk << " ";
		++itk;
	}
	cout << endl;
	
	auto itn = node->nodes_iterator();
	for(int i=0;i<node->size();i++){
		print_tree(*itn, tabs+'\t');
		++itn;
	}
	
	cout << tabs << ")" << endl;
}

__B_PLUS_TREE_BASE_TEMPLATE__
std::vector<Key> __B_PLUS_TREE_BASE_CLASS__::bfs_result()
{
	std::vector<Key> res;
	bfs_result(get_root(), res);
	return res;
}

__B_PLUS_TREE_BASE_TEMPLATE__
int __B_PLUS_TREE_BASE_CLASS__::get_mutex_count()
{
	return mutex_count.load();
}

__B_PLUS_TREE_BASE_TEMPLATE__
void __B_PLUS_TREE_BASE_CLASS__::bfs_result(node_ptr node, std::vector<Key>& res)
{
	using namespace std;
	
	if(is_leaf(node)){
		res.push_back(node->size());
		auto it = node->childs_iterator();
		while(it != node->childs_iterator_end()){
			res.push_back(it->data->item->first);
			it = node->get_childs()->find_next(it);
		}
		return;
	}

	res.push_back(node->size()-1);
	auto itk = node->keys_iterator();
	for(int i=0;i<node->size()-1;i++){
		res.push_back(*itk);
		++itk;
	}
	
	auto itn = node->nodes_iterator();
	for(int i=0;i<node->size();i++){
		bfs_result(*itn, res);
		++itn;
	}
}
#endif // ifdef DEBUG

#endif // BPLUSTREEBASE_H
