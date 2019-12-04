#ifndef BPLUSTREEBASEINTERNALNODE_H
#define BPLUSTREEBASEINTERNALNODE_H

#include <vector>
#include <memory>
#include "BPlusTreeBaseNode.hpp"

template<class Key, class T>
class BPlusTreeBaseInternalNode : public BPlusTreeBaseNode<Key, T>
{
    typedef BPlusTreeBaseNode<Key, T> Node;
    typedef std::shared_ptr<Node> node_ptr;
    typedef BPlusTreeBaseInternalNode<Key, T> InternalNode;
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
        Key split(node_ptr node);
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
        inline bool is_leaf();

    protected:
        keys_type* child_keys = nullptr;
        nodes_type* child_nodes = nullptr;
};


template<class Key, class T>
BPlusTreeBaseInternalNode<Key, T>::BPlusTreeBaseInternalNode()
{
	child_keys = new keys_type();
	child_nodes = new nodes_type();
}

template<class Key, class T>
BPlusTreeBaseInternalNode<Key, T>::BPlusTreeBaseInternalNode(keys_type* keys, nodes_type* nodes)
{
	set_keys(keys);
	set_nodes(nodes);
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::keys_type* BPlusTreeBaseInternalNode<Key, T>::get_keys()
{
	return child_keys;
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::nodes_type* BPlusTreeBaseInternalNode<Key, T>::get_nodes()
{
	return child_nodes;
}

template<class Key, class T>
BPlusTreeBaseInternalNode<Key, T>::~BPlusTreeBaseInternalNode()
{
    for(int i=0;i<nodes_size();i++){
        release_node((*child_nodes)[i]);
	}
	if(child_keys){
		delete child_keys;
	}
	if(child_nodes){
		delete child_nodes;
	}
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::release_node(node_ptr node)
{
    // TODO: replace with allocator methods
    // No need to delete smart ptr
    // delete node;
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::set_keys(keys_type* keys)
{
	child_keys = keys;
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::set_nodes(nodes_type* nodes)
{
	child_nodes = nodes;
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::add_keys(int ind, Key key)
{
    child_keys->insert(child_keys->begin()+ind, key);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::add_keys(int ind, keys_type_iterator s, keys_type_iterator e)
{
    child_keys->insert(child_keys->begin()+ind, s, e);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::remove_keys(int ind)
{
    child_keys->erase(child_keys->begin()+ind);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::remove_keys(keys_type_iterator s, keys_type_iterator e)
{
    child_keys->erase(s, e);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::add_nodes(int ind, node_ptr node)
{
    child_nodes->insert(child_nodes->begin()+ind, node);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e){
    child_nodes->insert(child_nodes->begin()+ind, s, e);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::remove_nodes(int ind)
{
    child_nodes->erase(child_nodes->begin()+ind);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::remove_nodes(nodes_type_iterator s, nodes_type_iterator e)
{
    child_nodes->erase(s, e);
}

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::shift_right(node_ptr parent)
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

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::shift_left(node_ptr parent)
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

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::join_left(node_ptr parent)
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

template<class Key, class T>
void BPlusTreeBaseInternalNode<Key, T>::join_right(node_ptr parent)
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

template<class Key, class T>
Key BPlusTreeBaseInternalNode<Key, T>::split(node_ptr node)
{
    // Get iterators to move to new node
    keys_type_iterator kstart = keys_iterator()+nodes_size()/2;
    keys_type_iterator kend = keys_iterator_end();
    nodes_type_iterator nstart = nodes_iterator()+nodes_size()/2;
    nodes_type_iterator nend = nodes_iterator_end();

    // Copy items to another node
    node->add_keys(0, kstart, kend);
    node->add_nodes(0, nstart, nend);

    // Get the return key
    Key ret = *(kstart-1);

    // Remove moved items
    remove_keys(kstart-1, kend);
    remove_nodes(nstart, nend);

    return ret;
}

template<class Key, class T>
inline bool BPlusTreeBaseInternalNode<Key, T>::is_leaf()
{
    return false;
}

template<class Key, class T>
int BPlusTreeBaseInternalNode<Key, T>::get_index(const Key& key)
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

template<class Key, class T>
int BPlusTreeBaseInternalNode<Key, T>::get_index(Node* node)
{
    int sz = nodes_size();
    for(int i=0;i<sz;i++){
        if((*child_nodes)[i].get() == node)
            return i;
    }
    return 0;
}

template<class Key, class T>
int BPlusTreeBaseInternalNode<Key, T>::size()
{
	return nodes_size();
}

template<class Key, class T>
int BPlusTreeBaseInternalNode<Key, T>::keys_size()
{
	return child_keys->size();
}

template<class Key, class T>
int BPlusTreeBaseInternalNode<Key, T>::nodes_size()
{
	return child_nodes->size();
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::node_ptr BPlusTreeBaseInternalNode<Key, T>::find(const Key& key)
{
    int index = get_index(key);
    return get_node(index);
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::node_ptr BPlusTreeBaseInternalNode<Key, T>::find_next(const Key& key)
{
    int index = get_index(key);
    index++;
    if(index > nodes_size()-1)
        return nullptr;
    return get_node(index);
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::node_ptr BPlusTreeBaseInternalNode<Key, T>::find_prev(const Key& key)
{
    int index = get_index(key);
    index--;
    if(index < 0)
        return nullptr;
    return get_node(index);
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::node_ptr BPlusTreeBaseInternalNode<Key, T>::first_child_node()
{
    return get_node(0);
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::node_ptr BPlusTreeBaseInternalNode<Key, T>::last_child_node()
{
    return get_node(nodes_size()-1);
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::node_ptr BPlusTreeBaseInternalNode<Key, T>::get_node(int index)
{
    return (*child_nodes)[index];
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::keys_type_iterator BPlusTreeBaseInternalNode<Key, T>::keys_iterator()
{
    return child_keys->begin();
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::keys_type_iterator BPlusTreeBaseInternalNode<Key, T>::keys_iterator_end()
{
    return child_keys->end();
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::nodes_type_iterator BPlusTreeBaseInternalNode<Key, T>::nodes_iterator()
{
    return child_nodes->begin();
}

template<class Key, class T>
typename BPlusTreeBaseInternalNode<Key, T>::nodes_type_iterator BPlusTreeBaseInternalNode<Key, T>::nodes_iterator_end()
{
    return child_nodes->end();
}


#endif // BPLUSTREEBASEINTERNALNODE_H
