#ifndef BPLUSTREEBASEINTERNALNODE_H
#define BPLUSTREEBASEINTERNALNODE_H

#include <vector>
#include <memory>
#include <functional>
#include "BPlusTreeBaseNode.hpp"

template<class Key, class T, class D>
class BPlusTreeBaseInternalNode : public BPlusTreeBaseNode<Key, T, D>
{
    typedef BPlusTreeBaseNode<Key, T, D> Node;
    typedef std::shared_ptr<Node> node_ptr;
    typedef BPlusTreeBaseInternalNode<Key, T, D> InternalNode;
    typedef std::vector<Key> keys_type;
    typedef std::vector<node_ptr> nodes_type;
    typedef typename nodes_type::iterator nodes_type_iterator;
    typedef typename keys_type::iterator keys_type_iterator;

    public:
        BPlusTreeBaseInternalNode();
        BPlusTreeBaseInternalNode(keys_type* keys, nodes_type* nodes);
        ~BPlusTreeBaseInternalNode();
        void release_node(node_ptr node);
        void set_keys(keys_type* keys);
        void set_nodes(nodes_type* nodes);
        keys_type* get_keys();
        nodes_type* get_nodes();
        void add_keys(int ind, Key key);
        void add_keys(int ind, keys_type_iterator s, keys_type_iterator e);
        void add_nodes(int ind, node_ptr node);
        void add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e);
        void remove_keys(int ind);
        void remove_keys(keys_type_iterator s, keys_type_iterator e);
        void remove_nodes(int ind);
        void remove_nodes(nodes_type_iterator s, nodes_type_iterator e);
        void shift_left(node_ptr parent);
        void shift_right(node_ptr parent);
        void join_left(node_ptr parent);
        void join_right(node_ptr parent);
        void split(node_ptr node, node_ptr parent);
        node_ptr get_node(int index);
        int size();
        int keys_size();
        int nodes_size();
        int get_index(const Key& key);
        int get_index(Node* node);
        node_ptr first_child_node();
        node_ptr last_child_node();
        node_ptr find(const Key& key);
        node_ptr find_next(const Key& key);
        node_ptr find_prev(const Key& key);
        keys_type_iterator keys_iterator();
        keys_type_iterator keys_iterator_end();
        nodes_type_iterator nodes_iterator();
        nodes_type_iterator nodes_iterator_end();
        bool is_leaf();

    protected:
        keys_type* child_keys = nullptr;
        nodes_type* child_nodes = nullptr;
};


template<class Key, class T, class D>
BPlusTreeBaseInternalNode<Key, T, D>::BPlusTreeBaseInternalNode()
{
	child_keys = new keys_type();
	child_nodes = new nodes_type();
}

template<class Key, class T, class D>
BPlusTreeBaseInternalNode<Key, T, D>::BPlusTreeBaseInternalNode(keys_type* keys, nodes_type* nodes)
{
	set_keys(keys);
	set_nodes(nodes);
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::keys_type* BPlusTreeBaseInternalNode<Key, T, D>::get_keys()
{
	return child_keys;
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::nodes_type* BPlusTreeBaseInternalNode<Key, T, D>::get_nodes()
{
	return child_nodes;
}

template<class Key, class T, class D>
BPlusTreeBaseInternalNode<Key, T, D>::~BPlusTreeBaseInternalNode()
{
	if(child_keys){
		delete child_keys;
	}
	if(child_nodes){
		for(int i=0;i<nodes_size();i++){
			release_node((*child_nodes)[i]);
		}
		delete child_nodes;
	}
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::release_node(node_ptr node)
{
	if(node->is_leaf()){
		node->set_prev_leaf(nullptr);
		node->set_next_leaf(nullptr);
		node->update_positions(nullptr);
	}
    // TODO: replace with allocator methods
    // No need to delete smart ptr
    // delete node;
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::set_keys(keys_type* keys)
{
	child_keys = keys;
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::set_nodes(nodes_type* nodes)
{
	child_nodes = nodes;
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::add_keys(int ind, Key key)
{
    child_keys->insert(child_keys->begin()+ind, key);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::add_keys(int ind, keys_type_iterator s, keys_type_iterator e)
{
    child_keys->insert(child_keys->begin()+ind, s, e);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::remove_keys(int ind)
{
    child_keys->erase(child_keys->begin()+ind);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::remove_keys(keys_type_iterator s, keys_type_iterator e)
{
    child_keys->erase(s, e);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::add_nodes(int ind, node_ptr node)
{
    child_nodes->insert(child_nodes->begin()+ind, node);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e){
    child_nodes->insert(child_nodes->begin()+ind, s, e);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::remove_nodes(int ind)
{
    child_nodes->erase(child_nodes->begin()+ind);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::remove_nodes(nodes_type_iterator s, nodes_type_iterator e)
{
    child_nodes->erase(s, e);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::shift_right(node_ptr parent)
{
    // Get index of shift node
    int index = parent->get_index(this)+1;
    // Get Node to shift
    node_ptr next = parent->get_node(index);
    // Get key iterator between nodes to shift
    keys_type_iterator divider = parent->keys_iterator()+(index-1);
    // Get key between nodes
    Key shift_key = *divider;
    // Add divide key to this node
    this->add_keys(this->keys_size(), shift_key);
    // Add new child from next node
    this->add_nodes(this->nodes_size(), next->first_child_node());
    // Update key between nodes
    *divider = *(next->keys_iterator());
    // Remove shifted items from next node
    next->remove_keys(0);
    next->remove_nodes(0);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::shift_left(node_ptr parent)
{
    // Get index of shift node
    int index = parent->get_index(this)-1;
    // Get Node to shift
    node_ptr prev = parent->get_node(index);
    // Get key iterator between nodes to shift
    keys_type_iterator divider = parent->keys_iterator()+(index);
    // Get key between nodes
    Key shift_key = *divider;
    // Add divide key to this node
    this->add_keys(0, shift_key);
    // Add new child from prev node
    this->add_nodes(0, prev->last_child_node());
    // Update key between nodes
    *divider = *(--prev->keys_iterator_end());
    // Remove shifted items from prev node
    prev->remove_keys(prev->keys_size()-1);
    prev->remove_nodes(prev->nodes_size()-1);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::join_left(node_ptr parent)
{
    // Get index of join node
    int index = parent->get_index(this)-1;
    // Get Node to join
    node_ptr prev = parent->get_node(index);
    // Get key iterator between nodes to join
    keys_type_iterator divider = parent->keys_iterator()+(index);
    // Get key between nodes
    Key join_key = *divider;
    // Add divide key to this node
    this->add_keys(0, join_key);
    // Move all items from left node
    this->add_keys(0, prev->keys_iterator(), prev->keys_iterator_end());
    this->add_nodes(0, prev->nodes_iterator(), prev->nodes_iterator_end());
    prev->remove_nodes(prev->nodes_iterator(), prev->nodes_iterator_end());
    // Delete joined items form parent
    parent->remove_keys(index);
    parent->remove_nodes(index);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::join_right(node_ptr parent)
{
    // Get index of join node
    int index = parent->get_index(this)+1;
    // Get Node to join
    node_ptr next = parent->get_node(index);
    // Get key iterator between nodes to join
    keys_type_iterator divider = parent->keys_iterator()+(index-1);
    // Get key between nodes
    Key join_key = *divider;
    // Add divide key to this node
    this->add_keys(keys_size(), join_key);
    // Join all items from left node
    this->add_keys(keys_size(), next->keys_iterator(), next->keys_iterator_end());
    this->add_nodes(nodes_size(), next->nodes_iterator(), next->nodes_iterator_end());
    next->remove_nodes(next->nodes_iterator(), next->nodes_iterator_end());
    // Delete joined items form parent
    parent->remove_keys(index-1);
    parent->remove_nodes(index);
}

template<class Key, class T, class D>
void BPlusTreeBaseInternalNode<Key, T, D>::split(node_ptr node, node_ptr parent)
{
    // Get iterators to move to new node
    keys_type_iterator kstart = keys_iterator();
    keys_type_iterator kend = keys_iterator() + keys_size()/2;
    nodes_type_iterator nstart = nodes_iterator();
    nodes_type_iterator nend = nodes_iterator() + nodes_size()/2;

    // Copy items to another node
    node->add_keys(0, kstart, kend);
    node->add_nodes(0, nstart, nend);

    // Get the return key
    Key ret = *kend;

    // Remove moved items
    remove_keys(kstart, kend + 1);
    remove_nodes(nstart, nend);
    
    // Insert to parent
    int index = parent->get_index(this);
    parent->add_keys(index, ret);
    parent->add_nodes(index, node);
}

template<class Key, class T, class D>
inline bool BPlusTreeBaseInternalNode<Key, T, D>::is_leaf()
{
    return false;
}

template<class Key, class T, class D>
int BPlusTreeBaseInternalNode<Key, T, D>::get_index(const Key& key)
{
    int mn=0,mx=keys_size()-1,md;
    int res = keys_size();
    while(mn<=mx){
        md = (mn+mx)/2;
        if((*child_keys)[md] <= key){
            mn = md+1;
        }
        else{
            mx = md-1;
            res = md;
        }
    }
    return res;
}

template<class Key, class T, class D>
int BPlusTreeBaseInternalNode<Key, T, D>::get_index(Node* node)
{
    int sz = nodes_size();
    for(int i=0;i<sz;i++){
        if((*child_nodes)[i].get() == node)
            return i;
    }
    return sz;
}

template<class Key, class T, class D>
int BPlusTreeBaseInternalNode<Key, T, D>::size()
{
	return nodes_size();
}

template<class Key, class T, class D>
int BPlusTreeBaseInternalNode<Key, T, D>::keys_size()
{
	return child_keys->size();
}

template<class Key, class T, class D>
int BPlusTreeBaseInternalNode<Key, T, D>::nodes_size()
{
	return child_nodes->size();
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::node_ptr BPlusTreeBaseInternalNode<Key, T, D>::find(const Key& key)
{
    int index = get_index(key);
    return get_node(index);
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T ,D>::node_ptr BPlusTreeBaseInternalNode<Key, T, D>::find_next(const Key& key)
{
    int index = get_index(key);
    index++;
    if(index > nodes_size()-1)
        return nullptr;
    return get_node(index);
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::node_ptr BPlusTreeBaseInternalNode<Key, T, D>::find_prev(const Key& key)
{
    int index = get_index(key);
    index--;
    if(index < 0)
        return nullptr;
    return get_node(index);
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::node_ptr BPlusTreeBaseInternalNode<Key, T, D>::first_child_node()
{
    return get_node(0);
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::node_ptr BPlusTreeBaseInternalNode<Key, T, D>::last_child_node()
{
    return get_node(nodes_size()-1);
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::node_ptr BPlusTreeBaseInternalNode<Key, T, D>::get_node(int index)
{
    return (*child_nodes)[index];
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::keys_type_iterator BPlusTreeBaseInternalNode<Key, T, D>::keys_iterator()
{
    return child_keys->begin();
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::keys_type_iterator BPlusTreeBaseInternalNode<Key, T, D>::keys_iterator_end()
{
    return child_keys->end();
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::nodes_type_iterator BPlusTreeBaseInternalNode<Key, T, D>::nodes_iterator()
{
    return child_nodes->begin();
}

template<class Key, class T, class D>
typename BPlusTreeBaseInternalNode<Key, T, D>::nodes_type_iterator BPlusTreeBaseInternalNode<Key, T, D>::nodes_iterator_end()
{
    return child_nodes->end();
}


#endif // BPLUSTREEBASEINTERNALNODE_H
