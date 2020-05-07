#ifndef BPLUSTREEBASE_H
#define BPLUSTREEBASE_H

#include <atomic>
#include <memory>
#include <cassert>
#include <list>
#include <functional>
#include <iostream>
#include "BPlusTreeBaseNode.hpp"
#include "BPlusTreeBaseInternalNode.hpp"
#include "BPlusTreeBaseLeafNode.hpp"
#include "BPlusTreeBaseIterator.hpp"

template <class Key, class T, class D = void*>
class BPlusTreeBase
{
	public:
		friend class BPlusTreeBaseIterator<Key, T, D>;

		typedef BPlusTreeBaseNode<Key, T, D> Node;
		typedef BPlusTreeBaseIterator<Key, T, D> iterator;
		typedef BPlusTreeBaseInternalNode<Key, T, D> InternalNode;
		typedef BPlusTreeBaseLeafNode<Key, T, D> LeafNode;
		typedef std::pair<const Key, T> EntryItem;
		typedef std::shared_ptr<EntryItem> EntryItem_ptr;
		typedef std::pair<Key, T> value_type;
		typedef typename Node::childs_type_iterator childs_type_iterator;	
		typedef std::shared_ptr<Node> node_ptr;
		typedef typename Node::child_item_type child_item_type;
		typedef typename Node::child_item_type_ptr childs_item_ptr;
		typedef BPlusTreeBase<Key,T> self_type;
		typedef std::list<node_ptr> node_list;
		typedef std::function<int(const Key& left, const Key& right)> compare_t;
		
		BPlusTreeBase(const self_type& obj);
		BPlusTreeBase(int factor);
		//BPlusTreeBase(int factor, compare_t fn);
		virtual ~BPlusTreeBase();
		iterator find(Key key);
		void erase(Key key);
		void erase(iterator it);
		void clear();
		void insert(value_type item, bool overwrite = false);
		iterator begin();
		iterator end();
		long long int size();
		T operator[](Key key);
		
		enum class PROCESS_TYPE{ READ, WRITE };
		enum class LEAF_REF{ PREV, NEXT };
	
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
		bool insert_req(node_ptr node, node_ptr parent, childs_item_ptr item, node_ptr& ins, bool overwrite, node_list& list);
		bool erase_req(node_ptr node, node_ptr parent, const Key &key, node_list& list);
		void clear_req(node_ptr node);
		bool is_leaf(node_ptr node);
		void set_root(node_ptr node);
		void release_node(node_ptr node);
		void update_positions(node_ptr node, bool release = false);
		void update_leaf_ref(node_ptr node, node_ptr ref_node, LEAF_REF ref);
		void init();
		virtual void processSearchNodeStart(node_ptr node, PROCESS_TYPE type);
		virtual void processSearchNodeEnd(node_ptr node, PROCESS_TYPE type);
		virtual void processInsertNode(node_ptr node);
		virtual void processDeleteNode(node_ptr node);
		virtual void processIteratorNodeReserved(node_ptr node);
		virtual void processIteratorNodeReleased(node_ptr node);
		virtual void processIteratorMoveStart(childs_item_ptr item, int step);
		virtual void processIteratorMoveEnd(childs_item_ptr item, int step);
		virtual void processItemReserve(childs_item_ptr item, PROCESS_TYPE type);
		virtual void processItemRelease(childs_item_ptr item, PROCESS_TYPE type);
		virtual void processItemMove(node_ptr node, bool release);
		virtual void processLeafReserve(node_ptr node, PROCESS_TYPE type);
		virtual void processLeafRelease(node_ptr node, PROCESS_TYPE type);
		virtual void processLeafInsertItem(node_ptr node, childs_item_ptr item);
		virtual void processLeafDeleteItem(node_ptr node, childs_item_ptr item);
		virtual void processLeafSplit(node_ptr node, node_ptr new_node, node_ptr link_node);
		virtual void processLeafJoin(node_ptr node, node_ptr join_node, node_ptr link_node);
		virtual void processLeafShift(node_ptr node, node_ptr shift_node);
		virtual void processLeafFree(node_ptr node);
		virtual void processLeafRef(node_ptr node, node_ptr ref_node, LEAF_REF ref);
		
		void release_entry_item(childs_item_ptr item);
		EntryItem_ptr create_entry_item(Key key, T val);
		
		node_ptr root;
		childs_item_ptr last;
		node_ptr stem;
		std::atomic<long long int> v_count;
		const int factor;
		compare_t compare_fn = [](const Key& left, const Key& right)->int{if (left == right) return 0; return left < right ? -1 : 1;};
};

template<class Key, class T, class D>
BPlusTreeBase<Key, T, D>::BPlusTreeBase(const self_type& obj) : factor(obj.factor)
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

template<class Key, class T, class D>
BPlusTreeBase<Key, T, D>::BPlusTreeBase(int f) : factor(f)
{
	init();
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::init()
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

template<class Key, class T, class D>
BPlusTreeBase<Key, T, D>::~BPlusTreeBase()
{
	if(!get_root())
		return;
	release_node(root);
	root = nullptr;
	stem = nullptr;
}

template<class Key, class T, class D>
const Key BPlusTreeBase<Key, T, D>::get_entry_key(childs_item_ptr item)
{
	return item->item->first;
}

template<class Key, class T, class D>
long long int BPlusTreeBase<Key, T, D>::size()
{
	return v_count.load();
}

template<class Key, class T, class D>
T BPlusTreeBase<Key, T, D>::operator[](Key key)
{
	return find(key)->second;
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::iterator BPlusTreeBase<Key, T, D>::find(Key k)
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
				int index = node->get_index(key);
				//processItemReserve(node->get(index), PROCESS_TYPE::READ);
				it = iterator(node->get(index), this);
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

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::insert(value_type item, bool overwrite)
{
	EntryItem_ptr itm = create_entry_item(item.first, item.second);
	
	// Create record item
	childs_item_ptr ch(new child_item_type());
	ch->item = itm;
	
	//const Key& key = get_entry_key(itm);
	node_ptr node = get_stem();
	node_ptr ins = nullptr;
	
	// Create path list
	node_list list;
	list.push_back(node);
	
	insert_req(node, nullptr, ch, ins, overwrite, list);
	
	//processSearchNodeStart(ins);
	//childs_type_iterator child_iterator = ins->childs_iterator()+ins->get_index(key);
	//processSearchNodeEnd(ins);
	
	//return find(key);
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::erase(Key item)
{
	node_ptr node = get_stem();
	node_list list;
	list.push_back(node);
	erase_req(node, nullptr, item, list);
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::erase(iterator it)
{
	if(it == end())
		return;
	erase(it.get_key());
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::clear()
{
	node_ptr node = get_stem();
	clear_req(node);
	v_count = 0;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::clear_req(node_ptr node)
{
	processSearchNodeStart(node, PROCESS_TYPE::WRITE);
	if(node->is_leaf()){
		// Delete node if leaf
		processDeleteNode(node);
		//update_positions(node, true);
		node->get_childs()->resize(0);
		
		//node->set_prev_leaf(nullptr);
		//node->set_next_leaf(nullptr);
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
	processSearchNodeEnd(node, PROCESS_TYPE::WRITE);
	if(!is_stem(node)){
		release_node(node);
	}
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::iterator BPlusTreeBase<Key, T, D>::begin()
{
	if(!size()){
		return end();
	}
	
	node_ptr node = min_node();
	
	//processSearchNodeStart(node, PROCESS_TYPE::READ);
	iterator it(node->first_child(),this);
	processSearchNodeEnd(node, PROCESS_TYPE::READ);
	
	return it;
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::iterator BPlusTreeBase<Key, T, D>::end()
{
	return iterator(last, this);
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::min_node()
{
	node_ptr node = get_stem();
	
	// process node before search
	processSearchNodeStart(node, PROCESS_TYPE::READ);
	
	return min_node(node);
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::min_node(node_ptr node)
{
	if(is_leaf(node)){
		// Reserve first item
		//if(node->size()){
		//	processItemReserve(node->first_child(), PROCESS_TYPE::READ);
		//}
		// process node after search
		//processSearchNodeEnd(node, PROCESS_TYPE::READ);
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

template<class Key, class T, class D>
bool BPlusTreeBase<Key, T, D>::is_root(node_ptr node)
{
	return root.get() == node.get();
}

template<class Key, class T, class D>
bool BPlusTreeBase<Key, T, D>::is_stem(node_ptr node)
{
	return stem.get() == node.get();
}

template<class Key, class T, class D>
bool BPlusTreeBase<Key, T, D>::is_leaf(node_ptr node)
{
	return node->is_leaf();
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::max_node()
{
	node_ptr node = get_stem();
	
	// process node before search
	processSearchNodeStart(node, PROCESS_TYPE::READ);
	
	return max_node(node);
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::max_node(node_ptr node)
{
	if(is_leaf(node)){
		// Reserve last item
		//if(node->size()){
		//	processItemReserve(node->last_child(), PROCESS_TYPE::READ);
		//}
		// process node after search
		//processSearchNodeEnd(node, PROCESS_TYPE::READ);
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

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::set_root(node_ptr node)
{
	root = node;
	if(stem->nodes_size()){
		stem->remove_nodes(0);
	}
	stem->add_nodes(0,node);
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::create_internal_node()
{
	// TODO: replace with allocator functions
	return node_ptr(new InternalNode());
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::create_leaf_node()
{
	// TODO: replace with allocator functions
	return node_ptr(new LeafNode());
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::release_node(node_ptr node)
{
	if(node->is_leaf()){
		//node->set_next_leaf(nullptr);
		//node->set_prev_leaf(nullptr);
		update_leaf_ref(node, nullptr, LEAF_REF::PREV);
		update_leaf_ref(node, nullptr, LEAF_REF::NEXT);
		update_positions(node, true);
	}
	// TODO: replace with allocator functions
	// No need to delete smart ptr
	// delete node;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::update_positions(node_ptr node, bool release)
{
	processItemMove(node, release);
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::update_leaf_ref(node_ptr node, node_ptr ref_node, LEAF_REF ref)
{
	processLeafRef(node, ref_node, ref);
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::get_root()
{
	return root;
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::node_ptr BPlusTreeBase<Key, T, D>::get_stem()
{
	return stem;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processSearchNodeStart(node_ptr node, PROCESS_TYPE type)
{
	node->lock();
	#ifdef DEBUG
		mutex_count++;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processSearchNodeEnd(node_ptr node, PROCESS_TYPE type)
{
	#ifdef DEBUG
		mutex_count--;
	#endif
	node->unlock();
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processItemReserve(childs_item_ptr item, PROCESS_TYPE type)
{
	#ifdef DEBUG
		item_reserve_count++;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processItemRelease(childs_item_ptr item, PROCESS_TYPE type)
{
	#ifdef DEBUG
		item_reserve_count--;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafReserve(node_ptr node, PROCESS_TYPE type)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafRelease(node_ptr node, PROCESS_TYPE type)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafInsertItem(node_ptr node, childs_item_ptr item)
{
	#ifdef DEBUG
	if(item){
		item_reserve_count++;
	}
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafDeleteItem(node_ptr node, childs_item_ptr item)
{
	#ifdef DEBUG
		item_reserve_count++;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafSplit(node_ptr node, node_ptr new_node, node_ptr link_node)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafJoin(node_ptr node, node_ptr join_node, node_ptr link_node)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafShift(node_ptr node, node_ptr shift_node)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafFree(node_ptr node)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processLeafRef(node_ptr node, node_ptr ref_node, LEAF_REF ref)
{
	if(ref == LEAF_REF::NEXT){
		node->set_next_leaf(ref_node);
	} else {
		node->set_prev_leaf(ref_node);
	}
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processItemMove(node_ptr node, bool release)
{
	if(!node->get_childs()){
		return;
	}
	node_ptr update_node = node;
	if(release){
		update_node = nullptr;
	}
	auto childs = node->get_childs();
	for(int i=0;i<node->childs_size();i++){
		(*childs)[i]->pos = i;
		(*childs)[i]->node = update_node;
	}
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processInsertNode(node_ptr node)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processDeleteNode(node_ptr node)
{
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processIteratorNodeReserved(node_ptr node)
{
	#ifdef DEBUG
		reserved_count++;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processIteratorNodeReleased(node_ptr node)
{
	#ifdef DEBUG
		reserved_count--;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processIteratorMoveStart(childs_item_ptr item, int step)
{
	#ifdef DEBUG
		iterator_move_count++;
	#endif
	return;
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::processIteratorMoveEnd(childs_item_ptr item, int step)
{
	#ifdef DEBUG
		iterator_move_count--;
	#endif
	return;
}

template<class Key, class T, class D>
typename BPlusTreeBase<Key, T, D>::EntryItem_ptr BPlusTreeBase<Key, T, D>::create_entry_item(Key key, T val)
{
	// TODO: replace with allocator functions
	return EntryItem_ptr(new EntryItem(key, val));
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::release_entry_item(childs_item_ptr item)
{
	// TODO replace with allocator functions
	// No need to delete shared ptr
	// delete item;
}

template<class Key, class T, class D>
bool BPlusTreeBase<Key, T, D>::erase_req(node_ptr node, node_ptr parent, const Key& key, node_list& list)
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
		
		
		int index = node->get_index(key);
		childs_item_ptr itm = node->get(index);
		
		// Leaf change operations
		processLeafDeleteItem(node, itm);
		
		// Release item
		release_entry_item(node->erase(index));
		
		// process release item
		processItemRelease(itm, PROCESS_TYPE::WRITE);
		
		// Update position
		update_positions(node);
		
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
			// Update positions
			update_positions(node);
			update_positions(nodeLeft);
			
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
			// Update positions
			update_positions(node);
			update_positions(nodeRight);
			
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
				// Update next to right node prev leaf
				//nextLeaf->set_prev_leaf(node);
				update_leaf_ref(nextLeaf, node, LEAF_REF::PREV);
				
				// Free prev leaf
				processLeafFree(nextLeaf);
				
				processInsertNode(nextLeaf);
				if(!swapped){
					processSearchNodeEnd(nextLeaf, PROCESS_TYPE::WRITE);
				}
			}
			//node->set_next_leaf(nextLeaf);
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
				// Update prev to left node next leaf
				//prevLeaf->set_next_leaf(node);
				update_leaf_ref(prevLeaf, node, LEAF_REF::NEXT);
				
				// Free prev leaf
				processLeafFree(prevLeaf);
				
				processInsertNode(prevLeaf);
				if(!swapped){
					processSearchNodeEnd(prevLeaf, PROCESS_TYPE::WRITE);
				}
			}
			//node->set_prev_leaf(prevLeaf);
			update_leaf_ref(node, prevLeaf, LEAF_REF::PREV);
			
			// Free node
			processLeafFree(node);
		}
	}
	
	if(is_leaf(node)){
		//joinNode->set_next_leaf(nullptr);
		//joinNode->set_prev_leaf(nullptr);
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

template<class Key, class T, class D>
bool BPlusTreeBase<Key, T, D>::insert_req(node_ptr node, node_ptr parent, childs_item_ptr item, node_ptr& ins, bool overwrite, node_list& list)
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

	const Key& key = get_entry_key(item);
	if(is_leaf(node)){
		// Remove current node from path list
		list.pop_back();
		
		ins = node;
		
		if(!node->exists(key)){
			// Leaf change insert item
			processLeafInsertItem(node, item);
			
			v_count++;			
			node->insert(item);
			
			update_positions(node);
			
			// Free item
			processItemRelease(item, PROCESS_TYPE::WRITE);
		
			// Free leaf item change
			processLeafFree(node);
			
			nodeChanged = true;
		}
		else if(overwrite){
			int index = node->get_index(key);
			
			childs_item_ptr itm = node->get(index);
			
			// Leaf change insert item
			processLeafInsertItem(node, itm);
			
			itm->item->second = item->item->second;
			
			update_positions(node);
			
			// Free Item
			processItemRelease(itm, PROCESS_TYPE::WRITE);
		
			// Free leaf item change
			processLeafFree(node);
			
			nodeChanged = true;
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
			update_positions(node);
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
				//nnode->set_next_leaf(nextLeaf);
				//nnode->set_prev_leaf(node);
				update_leaf_ref(nnode, nextLeaf, LEAF_REF::NEXT);
				update_leaf_ref(nnode, node, LEAF_REF::PREV);
				
				// Free new node
				processLeafFree(nnode);

				// Process newly created node
				processInsertNode(nnode);
				// Process Node after search
				processSearchNodeEnd(nnode, PROCESS_TYPE::WRITE);
				
				// Set next to newly created node prev leaf
				//node->set_next_leaf(nnode);
				update_leaf_ref(node, nnode, LEAF_REF::NEXT);
				
				// Free node
				processLeafFree(node);
				
				// Update next leaf
				if(nextLeaf){
					//processSearchNodeStart(nextLeaf, PROCESS_TYPE::WRITE);
					//nextLeaf->set_prev_leaf(nnode);
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
				//nnode->set_prev_leaf(prevLeaf);
				//nnode->set_next_leaf(node);
				update_leaf_ref(nnode, prevLeaf, LEAF_REF::PREV);
				update_leaf_ref(nnode, node, LEAF_REF::NEXT);
				
				// Free new node
				processLeafFree(nnode);

				// Process newly created node
				processInsertNode(nnode);
				// Process Node after search
				processSearchNodeEnd(nnode, PROCESS_TYPE::WRITE);
				
				// Set prev from newly created node next leaf
				//node->set_prev_leaf(nnode);
				update_leaf_ref(node, nnode, LEAF_REF::PREV);
				
				// Free new node
				processLeafFree(node);
				
				// Update prev node
				if(prevLeaf){
					//processSearchNodeStart(prevLeaf, PROCESS_TYPE::WRITE);
					//prevLeaf->set_next_leaf(nnode);
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
template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::print_tree()
{
	print_tree(get_root(), "");
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::print_tree(node_ptr node, std::string tabs)
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

template<class Key, class T, class D>
std::vector<Key> BPlusTreeBase<Key, T, D>::bfs_result()
{
	std::vector<Key> res;
	bfs_result(get_root(), res);
	return res;
}

template<class Key, class T, class D>
int BPlusTreeBase<Key, T, D>::get_mutex_count()
{
	return mutex_count.load();
}

template<class Key, class T, class D>
void BPlusTreeBase<Key, T, D>::bfs_result(node_ptr node, std::vector<Key>& res)
{
	using namespace std;
	

	if(is_leaf(node)){
		res.push_back(node->size());
		auto it = node->childs_iterator();
		for(int i=0;i<node->size();i++){
			res.push_back((*it)->item->first);
			++it;
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
