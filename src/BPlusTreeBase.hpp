#ifndef BPLUSTREEBASE_H
#define BPLUSTREEBASE_H

#include <iostream>
#include <atomic>
#include <memory>
#include <list>
#include "BPlusTreeBaseNode.hpp"
#include "BPlusTreeBaseInternalNode.hpp"
#include "BPlusTreeBaseLeafNode.hpp"
#include "BPlusTreeBaseIterator.hpp"

template <class Key, class T>
class BPlusTreeBase
{
	
    public:
		friend class BPlusTreeBaseIterator<Key, T>;

		typedef BPlusTreeBaseNode<Key, T> Node;
		typedef BPlusTreeBaseIterator<Key, T> iterator;
		typedef BPlusTreeBaseInternalNode<Key, T> InternalNode;
		typedef BPlusTreeBaseLeafNode<Key, T> LeafNode;
		typedef std::pair<const Key, T> EntryItem;
		typedef std::shared_ptr<EntryItem> EntryItem_ptr;
		typedef std::pair<Key, T> value_type;
		typedef typename Node::childs_type_iterator childs_type_iterator;	
		typedef std::shared_ptr<Node> node_ptr;
		typedef typename Node::child_item_type_ptr childs_item_ptr;
		typedef BPlusTreeBase<Key,T> self_type;
		typedef std::list<node_ptr> node_list;
		
        BPlusTreeBase(const self_type& obj);
        BPlusTreeBase(int factor);
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
    
    // DEBUG METHODS
	#ifdef DEBUG
		std::atomic<int> mutex_count;
		int reserved_count = 0;
		int iterator_move_count = 0;
		void print_tree();
		void print_tree(node_ptr node, std::string tabs);
		std::vector<Key> bfs_result();
		void bfs_result(node_ptr node, std::vector<Key>& res);
		int get_mutex_count();
	#endif

    protected:
        const Key get_entry_key(EntryItem_ptr item);
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
        bool insert_req(node_ptr node, node_ptr parent, EntryItem_ptr& item, node_ptr& ins, bool overwrite, node_list& list);
        bool erase_req(node_ptr node, node_ptr parent, const Key &key, node_list& list);
        void clear_req(node_ptr node);
        bool is_leaf(node_ptr node);
        void set_root(node_ptr node);
        void release_node(node_ptr node);
        virtual void processSearchNodeStart(node_ptr& node);
        virtual void processSearchNodeEnd(node_ptr& node);
        virtual void processInsertNode(node_ptr& node);
        virtual void processDeleteNode(node_ptr& node);
        virtual void processIteratorNodeReserved(node_ptr& node);
        virtual void processIteratorNodeReleased(node_ptr& node);
        virtual void processIteratorMoveStart(childs_item_ptr& item, int step);
        virtual void processIteratorMoveEnd(childs_item_ptr& item, int step);
        void release_entry_item(childs_item_ptr item);
        EntryItem_ptr create_entry_item(Key key, T val);
        
        node_ptr root;
        node_ptr last;
        node_ptr stem;
        std::atomic<long long int> v_count;
        const int factor;
};

template<class Key, class T>
BPlusTreeBase<Key,T>::BPlusTreeBase(const self_type& obj) : factor(obj.factor)
{
	root = obj.root;
	last = obj.last;
	stem = obj.stem;
	v_count = obj.v_count.load();
	#ifdef DEBUG
	mutex_count = obj.mutex_count.load();
	#endif
}

template<class Key, class T>
BPlusTreeBase<Key,T>::BPlusTreeBase(int f) : factor(f)
{
	stem = create_internal_node();
    node_ptr node = create_leaf_node();
    set_root(node);
    last = nullptr;
    v_count = 0;
    #ifdef DEBUG
    mutex_count = 0;
    #endif
}

template<class Key, class T>
BPlusTreeBase<Key,T>::~BPlusTreeBase()
{
    if(!get_root())
        return;
    release_node(root);
    root = nullptr;
}

template<class Key, class T>
const Key BPlusTreeBase<Key, T>::get_entry_key(EntryItem_ptr item)
{
    return item->first;
}

template<class Key, class T>
long long int BPlusTreeBase<Key,T>::size()
{
    return v_count.load();
}

template<class Key, class T>
T BPlusTreeBase<Key, T>::operator[](Key key)
{
	return find(key)->second;
}

template<class Key, class T>
typename BPlusTreeBase<Key, T>::iterator BPlusTreeBase<Key,T>::find(Key k)
{
	const Key key = k;
    node_ptr node = get_stem();
    processSearchNodeStart(node);
    while(true){
        if(!node){
			processSearchNodeEnd(node);
			return end();
		}
        if(node->is_leaf()){
			iterator it = end();
            if(node->exists(key))
                it = iterator(node->get(node->get_index(key)), this);
			processSearchNodeEnd(node);
            return it;
        }
        node_ptr tnode = node->find(key);
        processSearchNodeStart(tnode);
        processSearchNodeEnd(node);
        node = tnode;
    }
    return end();
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::insert(value_type item, bool overwrite)
{
    EntryItem_ptr itm = create_entry_item(item.first, item.second);
    //const Key& key = get_entry_key(itm);
    node_ptr node = get_stem();
    node_ptr ins = nullptr;
    
    // Create path list
    node_list list;
    list.push_back(node);
    
    insert_req(node, nullptr, itm, ins, overwrite, list);
    
    //processSearchNodeStart(ins);
    //childs_type_iterator child_iterator = ins->childs_iterator()+ins->get_index(key);
    //processSearchNodeEnd(ins);
    
    //return find(key);
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::erase(Key item)
{
    node_ptr node = get_stem();
    node_list list;
    list.push_back(node);
    erase_req(node, nullptr, item, list);
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::erase(iterator it)
{
    if(it == end())
        return;
    erase(it.get_key());
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::clear()
{
	node_ptr node = get_stem();
	clear_req(node);
	v_count = 0;
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::clear_req(node_ptr node)
{
	processSearchNodeStart(node);
	if(node->is_leaf()){
		// Delete node if leaf
		processDeleteNode(node);
		node->get_childs()->resize(0);
		node->set_prev_leaf(nullptr);
		node->set_next_leaf(nullptr);
	}
	else{
		// Iterate over child nodes
		for(auto& n : (*node->get_nodes())){
			clear_req(n);
		}
		// Delete internal node
		if(!is_stem(node)){
			processDeleteNode(node);
		}
		node->get_keys()->resize(0);
		node->get_nodes()->resize(0);
	}
	if(is_stem(node)){
		node_ptr n = create_leaf_node();
		processSearchNodeStart(n);
		set_root(n);
		processInsertNode(n);
		processSearchNodeEnd(n);
	}
	processSearchNodeEnd(node);
	if(!is_stem(node)){
		release_node(node);
	}
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::iterator BPlusTreeBase<Key,T>::begin()
{
    if(!size())
        return end();
    node_ptr node = min_node();
    
    processSearchNodeStart(node);
    iterator it(node->first_child(),this);
    processSearchNodeEnd(node);
    
    return it;
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::iterator BPlusTreeBase<Key,T>::end()
{
    return iterator(nullptr, this);
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key,T>::min_node()
{
    return min_node(get_stem());
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key,T>::min_node(node_ptr node)
{
    // process node before search
    processSearchNodeStart(node);

    if(is_leaf(node)){
		// process node after search
		processSearchNodeEnd(node);
        return node;
	}
	
	// Get left most node
    node_ptr nnode = node->first_child_node();
    
    // process node after search
    processSearchNodeEnd(node);
    
    // recoursively search for min leaf node
    return min_node(nnode);
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::is_root(node_ptr node)
{
    return root.get() == node.get();
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::is_stem(node_ptr node)
{
    return stem.get() == node.get();
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::is_leaf(node_ptr node)
{
    return node->is_leaf();
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key,T>::max_node()
{
    return max_node(get_stem());
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key,T>::max_node(node_ptr node)
{

    // process node before search
    processSearchNodeStart(node);

    if(is_leaf(node)){
		// process node after search
		processSearchNodeEnd(node);
        return node;
	}
	
	// Get right most node
	node_ptr nnode = node->last_child_node();

    // process node after search
    processSearchNodeEnd(node);
    
    // recoursively search for max leaf node
    return max_node(nnode);
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::set_root(node_ptr node)
{
    root = node;
    if(stem->nodes_size()){
		stem->remove_nodes(0);
	}
	stem->add_nodes(0,node);
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key, T>::create_internal_node()
{
    // TODO: replace with allocator functions
    return node_ptr(new InternalNode());
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key, T>::create_leaf_node()
{
    // TODO: replace with allocator functions
    return node_ptr(new LeafNode());
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::release_node(node_ptr node)
{
	if(node->is_leaf()){
		node->update_positions(nullptr);
	}
    // TODO: replace with allocator functions
    // No need to delete smart ptr
    // delete node;
}


template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key, T>::get_root()
{
    return root;
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::node_ptr BPlusTreeBase<Key, T>::get_stem()
{
    return stem;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processSearchNodeStart(node_ptr& node)
{
	node->lock();
	#ifdef DEBUG
		mutex_count++;
	#endif
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processSearchNodeEnd(node_ptr& node)
{
	#ifdef DEBUG
		mutex_count--;
	#endif
	node->unlock();
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processInsertNode(node_ptr& node)
{
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processDeleteNode(node_ptr& node)
{
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processIteratorNodeReserved(node_ptr& node)
{
	#ifdef DEBUG
		reserved_count++;
	#endif
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processIteratorNodeReleased(node_ptr& node)
{
	#ifdef DEBUG
		reserved_count--;
	#endif
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processIteratorMoveStart(childs_item_ptr& item, int step)
{
	#ifdef DEBUG
		iterator_move_count++;
	#endif
	return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processIteratorMoveEnd(childs_item_ptr& item, int step)
{
	#ifdef DEBUG
		iterator_move_count--;
	#endif
	return;
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::EntryItem_ptr BPlusTreeBase<Key, T>::create_entry_item(Key key, T val)
{
	// TODO: replace with allocator functions
	return EntryItem_ptr(new EntryItem(key, val));
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::release_entry_item(childs_item_ptr item)
{
	// TODO replace with allocator functions
	// No need to delete shared ptr
	// delete item;
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::erase_req(node_ptr node, node_ptr parent, const Key& key, node_list& list)
{
    // Process Node before search
    processSearchNodeStart(node);

    bool nodeChanged = false;
    
    // Remove and end with all nodes that will never be modified
	if(node->size() > factor){
		while(list.front().get() != node.get()){
			node_ptr n = list.front();
			processSearchNodeEnd(n);
			list.pop_front();
		}
	}

    if(is_leaf(node)){
		if(!node->exists(key)){
			processSearchNodeEnd(node);
			return false;
		}
        release_entry_item(node->erase(node->get_index(key)));
        node->update_positions(node);
        v_count--;
        nodeChanged = true;
        list.pop_back();
	}
    else{
		// Find next node
		node_ptr n = node->find(key);
		
		// Add internal node to the path list
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
			processSearchNodeEnd(node);
			return false;
		}
    }

    if(node->size() >= factor || is_root(node)){

        if(!is_leaf(node) && node->size() == 1){

            node_ptr child = node->first_child_node();
            
            // Process as this node will became as root
            processSearchNodeStart(child);

            // Set new root
            set_root(child);
            
            // Save Child as root
            processInsertNode(child);
            
            // Leave new root node
            processSearchNodeEnd(child);

            // Delete old root node
            node->remove_nodes(0);
            processDeleteNode(node);
            
            // Process node after search
            processSearchNodeEnd(node);
            
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
        processSearchNodeEnd(node);
        // Parent node was not changed
        return false;
    }

    node_ptr nodeLeft;
    node_ptr nodeRight;

    nodeLeft = parent->find_prev(key);
    if(nodeLeft){
		// Start working with left node
		processSearchNodeStart(nodeLeft);
	}
    if(nodeLeft && nodeLeft->size() > factor){

        // Shift nodes
        node->shift_left(parent);

        // Process node
        processInsertNode(node);
        processSearchNodeEnd(node);

        // Process updated node
        processInsertNode(nodeLeft);
        processSearchNodeEnd(nodeLeft);

        // Parent node changed
        return true;
    }
    
    nodeRight = parent->find_next(key);
	if(nodeRight){
		// Start working with right node
		processSearchNodeStart(nodeRight);
	}
    if(nodeRight && nodeRight->size() > factor){
		
		if(nodeLeft){
			// process finish with leftNode
			processSearchNodeEnd(nodeLeft);
		}

        // Shift nodes
        node->shift_right(parent);

        // Process node
        processInsertNode(node);
        processSearchNodeEnd(node);

        // Process updated node
        processInsertNode(nodeRight);
        processSearchNodeEnd(nodeRight);

        // Parent node changed
        return true;
    }

    node_ptr joinNode = nodeRight ? nodeRight : nodeLeft;

    if(nodeRight){
        node->join_right(parent);
        if(is_leaf(node)){
			node_ptr nextLeaf = joinNode->next_leaf();
			if(nextLeaf){
				// Update next to right node prev leaf
				processSearchNodeStart(nextLeaf);
				nextLeaf->set_prev_leaf(node);
				processInsertNode(nextLeaf);
				processSearchNodeEnd(nextLeaf);
			}
			node->set_next_leaf(nextLeaf);
		}
    }
    else{
        node->join_left(parent);
        if(is_leaf(node)){
			node_ptr prevLeaf = joinNode->prev_leaf();
			if(prevLeaf){
				// Update prev to left node next leaf
				processSearchNodeStart(prevLeaf);
				prevLeaf->set_next_leaf(node);
				processInsertNode(prevLeaf);
				processSearchNodeEnd(prevLeaf);
			}
			node->set_prev_leaf(prevLeaf);
		}
    }
    
    if(is_leaf(node)){
		joinNode->set_next_leaf(nullptr);
		joinNode->set_prev_leaf(nullptr);
	}

    // Save node
    processInsertNode(node);
    
    // Delete useless node
    processDeleteNode(joinNode);
    
    // End processing with side nodes
	if(nodeLeft){
		processSearchNodeEnd(nodeLeft);
	}
	if(nodeRight){
		processSearchNodeEnd(nodeRight);
	}
	
    processSearchNodeEnd(node);

    // Clear node memory
    release_node(joinNode);

    // Parent node changed
    return true;
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::insert_req(node_ptr node, node_ptr parent, EntryItem_ptr& item, node_ptr& ins, bool overwrite, node_list& list)
{	
    // Process Node before search
    processSearchNodeStart(node);

    bool nodeChanged = false;
    
    // Remove and end with all nodes from list that will never be modified
	if(node->size() < factor*2-1){
		while(list.front().get() != node.get()){
			node_ptr n = list.front();
			processSearchNodeEnd(n);
			list.pop_front();
		}
	}

    const Key& key = get_entry_key(item);
    if(is_leaf(node)){
		ins = node;
        if(!node->exists(key)){
            v_count++;
            node->insert(item);
            nodeChanged = true;
        }
        else if(overwrite){
			int index = node->get_index(key);
			node->get(index)->item->second = item->second;
			nodeChanged = true;
		}
		node->update_positions(node);
		list.pop_back();
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
			processSearchNodeEnd(node);
			return false;
		}
    }

    node_ptr nnode = nullptr;

    if(node->size() >= factor*2){

        nodeChanged = true;

        // Create new node to split to
        nnode = is_leaf(node) ? create_leaf_node() : create_internal_node();
        processSearchNodeStart(nnode);
        
        // Split node to nnode
        Key ins_key = node->split(nnode);
        
        // Update inserted node ref if necessary
        if(is_leaf(node) && !node->exists(key)){
			ins = nnode;
		}
		
		// Update dependencies of newly created node
		if(is_leaf(node)){
			node_ptr nextLeaf = node->next_leaf();
			nnode->set_next_leaf(nextLeaf);
			nnode->set_prev_leaf(node);
		}
		// Process newly created node
        processInsertNode(nnode);
        // Process Node after search
        processSearchNodeEnd(nnode);
        
        // Update leaf nodes dependencies
        if(is_leaf(node)){
			node_ptr nextLeaf = node->next_leaf();
			//nnode->set_next_leaf(nextLeaf);
			node->set_next_leaf(nnode);
			//nnode->set_prev_leaf(node);
			if(nextLeaf){
				// Set next to newly created node prev leaf
				processSearchNodeStart(nextLeaf);
				nextLeaf->set_prev_leaf(nnode);
				processInsertNode(nextLeaf);
				processSearchNodeEnd(nextLeaf);
			}
		}

        if(is_root(node)){
			// Process updated Node before parent
			processInsertNode(node);
			// Process Node after search before parent
			processSearchNodeEnd(node);
			
            // Create new root node
            parent = create_internal_node();
            processSearchNodeStart(parent);
            // Add items to parent node
            parent->add_keys(0, ins_key);
            parent->add_nodes(0, node);
            parent->add_nodes(1, nnode);
            set_root(parent);
            // Process updated Node
            processInsertNode(parent);
            // Process Node after search
            processSearchNodeEnd(parent);
            
            return true;
        }
        else{
            // get index of node
            int index = parent->get_index(key);
            // Add items to parent node
            parent->add_keys(index, ins_key);
            parent->add_nodes(index+1, nnode);
        }
    }

    if(nodeChanged){
        // Process updated Node
        processInsertNode(node);
    }

    // Process Node after search
    processSearchNodeEnd(node);

    return !!nnode;

}

#ifdef DEBUG
template<class Key, class T>
void BPlusTreeBase<Key, T>::print_tree()
{
	print_tree(get_root(), "");
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::print_tree(node_ptr node, std::string tabs)
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

template<class Key, class T>
std::vector<Key> BPlusTreeBase<Key, T>::bfs_result()
{
	std::vector<Key> res;
	bfs_result(get_root(), res);
	return res;
}

template<class Key, class T>
int BPlusTreeBase<Key, T>::get_mutex_count()
{
	return mutex_count.load();
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::bfs_result(node_ptr node, std::vector<Key>& res)
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
