#ifndef BPLUSTREEBASE_H
#define BPLUSTREEBASE_H

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
		typedef std::pair<Key, T> value_type;
		typedef typename Node::childs_type_iterator childs_type_iterator;		
		
        BPlusTreeBase(int factor);
        virtual ~BPlusTreeBase();
        iterator find(Key key);
        void erase(Key key);
        void erase(iterator it);
        iterator insert(value_type item);
        iterator begin();
        iterator end();
        long size();
        T operator[](Key key);
    
		int mutex_count = 0;
    // DEBUG METHODS
	#ifdef DEBUG
		void print_tree();
		void print_tree(Node* node, std::string tabs);
		std::vector<Key> bfs_result();
		void bfs_result(Node* node, std::vector<Key>& res);
	#endif

    protected:
        const Key get_entry_key(EntryItem* item);
        Node* min_node();
        Node* min_node(Node* node);
        Node* max_node();
        Node* max_node(Node* node);
        Node* root;
        Node* last;
        Node* create_internal_node();
        Node* create_leaf_node();
        Node* get_root();
        bool is_root(Node* node);
        bool insert_req(Node* node, Node* parent, EntryItem*& item, Node*& ins);
        bool erase_req(Node* node, Node* parent, const Key &key);
        bool is_leaf(Node* node);
        void set_root(Node* node);
        void release_node(Node* node);
        virtual void processSearchNodeStart(Node* node);
        virtual void processSearchNodeEnd(Node* node);
        virtual void processInsertNode(Node* node);
        virtual void processDeleteNode(Node* node);
        void release_entry_item(EntryItem* item);
        EntryItem* create_entry_item(Key key, T val);
        long v_count;
        const int factor;
};


template<class Key, class T>
BPlusTreeBase<Key,T>::BPlusTreeBase(int f) : factor(f)
{
    Node* node = create_leaf_node();
    set_root(node);
    last = nullptr;
    v_count = 0;
}

template<class Key, class T>
BPlusTreeBase<Key,T>::~BPlusTreeBase()
{
    if(!get_root())
        return;
    release_node(root);
}

template<class Key, class T>
const Key BPlusTreeBase<Key, T>::get_entry_key(EntryItem* item)
{
    return item->first;
}

template<class Key, class T>
long BPlusTreeBase<Key,T>::size()
{
    return v_count;
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
    Node* node = get_root();
    while(true){
        if(!node) return end();
        processSearchNodeStart(node);
        if(node->is_leaf()){
			iterator it = end();
            if(node->exists(key))
                it = iterator(node, node->childs_iterator()+node->get_index(key), this);
			processSearchNodeEnd(node);
            return it;
        }
        Node* tnode = node->find(key);
        processSearchNodeEnd(node);
        node = tnode;
    }
    return end();
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::iterator BPlusTreeBase<Key,T>::insert(value_type item)
{
    EntryItem* itm = create_entry_item(item.first, item.second);
    const Key& key = get_entry_key(itm);
    Node* node = get_root();
    Node* ins = nullptr;
    insert_req(node, nullptr, itm, ins);
    
    processSearchNodeStart(ins);
    childs_type_iterator child_iterator = ins->childs_iterator()+ins->get_index(key);
    processSearchNodeEnd(ins);
    
    return iterator(ins, child_iterator, this);
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::erase(Key item)
{
    erase(find(item));
}

template<class Key, class T>
void BPlusTreeBase<Key,T>::erase(iterator it)
{
    if(it == end())
        return;
    Node* node = get_root();
    Key key = it.get_key();
    erase_req(node, nullptr, key);
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::iterator BPlusTreeBase<Key,T>::begin()
{
    if(!size())
        return end();
    Node* node = min_node();
    return iterator(node,node->childs_iterator(),this);
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::iterator BPlusTreeBase<Key,T>::end()
{
    return iterator(last, {}, this);
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key,T>::min_node()
{
    return min_node(get_root());
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key,T>::min_node(Node* node)
{
    // process node before search
    processSearchNodeStart(node);

    if(is_leaf(node)){
		// process node after search
		processSearchNodeEnd(node);
        return node;
	}
	
	// Get left most node
    Node* nnode = node->first_child_node();
    
    // process node after search
    processSearchNodeEnd(node);
    
    // recoursively search for min leaf node
    return min_node(nnode);
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::is_root(Node* node)
{
    return root == node;
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::is_leaf(Node* node)
{
    return node->is_leaf();
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key,T>::max_node()
{
    return max_node(get_root());
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key,T>::max_node(Node* node)
{

    // process node before search
    processSearchNodeStart(node);

    if(is_leaf(node)){
		// process node after search
		processSearchNodeEnd(node);
        return node;
	}
	
	// Get right most node
	Node* nnode = node->last_child_node();

    // process node after search
    processSearchNodeEnd(node);
    
    // recoursively search for max leaf node
    return max_node(nnode);
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::set_root(Node* node)
{
    root = node;
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key, T>::create_internal_node()
{
    // TODO: replace with allocator functions
    return new InternalNode();
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key, T>::create_leaf_node()
{
    // TODO: replace with allocator functions
    return new LeafNode();
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::release_node(Node* node)
{
    // TODO: replace with allocator functions
    delete node;
}


template<class Key, class T>
typename BPlusTreeBase<Key,T>::Node* BPlusTreeBase<Key, T>::get_root()
{
    return root;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processSearchNodeStart(Node* node)
{
	mutex_count++;
	node->lock();
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processSearchNodeEnd(Node* node)
{
	mutex_count--;
	node->unlock();
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processInsertNode(Node* node)
{
    return;
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::processDeleteNode(Node* node)
{
    return;
}

template<class Key, class T>
typename BPlusTreeBase<Key,T>::EntryItem* BPlusTreeBase<Key, T>::create_entry_item(Key key, T val)
{
	// TODO: replace with allocator functions
	return new EntryItem(key, val);
}

template<class Key, class T>
void BPlusTreeBase<Key, T>::release_entry_item(EntryItem* item)
{
	// TODO replace with allocator functions
	delete item;
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::erase_req(Node* node, Node* parent, const Key& key)
{
    // Process Node before search
    processSearchNodeStart(node);

    bool nodeChanged = false;

    if(is_leaf(node)){
		if(!node->exists(key)){
			processSearchNodeEnd(node);
			return false;
		}
        release_entry_item(node->erase(node->get_index(key)));
        v_count--;
        nodeChanged = true;
    }
    else{
        nodeChanged = erase_req(node->find(key), node, key);
    }

    if(node->size() >= factor || is_root(node)){

        if(!is_leaf(node) && node->size() == 1){

            Node* child = node->first_child_node();

            // Set new root
            set_root(child);

            // Delete old root node
            processDeleteNode(node);

            // Process node after search
            processSearchNodeEnd(node);

            // Clear node memory
            node->remove_nodes(0);
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

    Node* nodeLeft;
    Node* nodeRight;

    nodeLeft = parent->find_prev(key);
    if(nodeLeft && nodeLeft->size() > factor){
		
        // Process Node before search
        processSearchNodeStart(nodeLeft);

        // Shift nodes
        node->shift_left(parent);

        // Process updated node
        processInsertNode(nodeLeft);
        processSearchNodeEnd(nodeLeft);

        processInsertNode(node);
        processSearchNodeEnd(node);

        // Parent node changed
        return true;
    }

    nodeRight = parent->find_next(key);
    if(nodeRight && nodeRight->size() > factor){
		
        // Process Node before search
        processSearchNodeStart(nodeRight);

        // Shift nodes
        node->shift_right(parent);

        // Process updated node
        processInsertNode(nodeRight);
        processSearchNodeEnd(nodeRight);

        processInsertNode(node);
        processSearchNodeEnd(node);

        // Parent node changed
        return true;
    }

    Node* joinNode = nodeRight ? nodeRight : nodeLeft;
    processSearchNodeStart(joinNode);

    if(nodeRight){
        node->join_right(parent);
        node->set_next_leaf(joinNode->next_leaf());
    }
    else{
        node->join_left(parent);
        node->set_prev_leaf(joinNode->prev_leaf());
    }

    // Delete useless node
    processDeleteNode(joinNode);
    
    processSearchNodeEnd(joinNode);
    processSearchNodeEnd(node);

    // Clear node memory
    release_node(joinNode);

    // Parent node changed
    return true;
}

template<class Key, class T>
bool BPlusTreeBase<Key,T>::insert_req(Node* node, Node* parent, EntryItem*& item, Node*& ins)
{
    // Process Node before search
    processSearchNodeStart(node);

    bool nodeChanged = false;

    const Key& key = get_entry_key(item);
    if(is_leaf(node)){
		ins = node;
        if(!node->exists(key)){
            v_count++;
            node->insert(item);
            nodeChanged = true;
        }
    }
    else{
        nodeChanged = insert_req(node->find(key), node, item, ins);
    }

    Node* nnode = nullptr;

    if(node->size() >= factor*2){

        nodeChanged = true;

        // Create new node to split to
        nnode = is_leaf(node) ? create_leaf_node() : create_internal_node();
        processSearchNodeStart(nnode);
        
        // Split node to nnode
        Key ins_key = node->split(nnode);
        
        // Update inserted node ref if necessary
        if(!node->exists(key)){
			ins = nnode;
		}
        
        // Update leaf nodes dependencies
        if(is_leaf(node)){
			nnode->set_next_leaf(node->next_leaf());
			node->set_next_leaf(nnode);
			nnode->set_prev_leaf(node);
		}
		
		// Process newly created node
        processInsertNode(nnode);

        // Process Node after search
        processSearchNodeEnd(nnode);

        if(is_root(node)){
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
void BPlusTreeBase<Key, T>::print_tree(Node* node, std::string tabs)
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
void BPlusTreeBase<Key, T>::bfs_result(Node* node, std::vector<Key>& res)
{
	using namespace std;
	

	if(is_leaf(node)){
		res.push_back(node->size());
		auto it = node->childs_iterator();
		for(int i=0;i<node->size();i++){
			res.push_back((*it)->first);
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
