#ifndef BPLUSTREEBASELEAFNODE_H
#define BPLUSTREEBASELEAFNODE_H

#include "BPlusTreeBaseNode.hpp"
#include <utility>
#include <map>
#include <memory>

//int bbb = 0;

template<class Key, class T>
class BPlusTreeBaseLeafNode : public BPlusTreeBaseNode<Key, T>
{
    typedef BPlusTreeBaseNode<Key, T> Node;
    typedef std::shared_ptr<Node> node_ptr;
    typedef BPlusTreeBaseLeafNode<Key, T> LeafNode;
    typedef std::pair<const Key,T> child_item_type;
    typedef std::vector<child_item_type*> child_type;
    typedef typename child_type::iterator childs_type_iterator;

    public:
        BPlusTreeBaseLeafNode();
        BPlusTreeBaseLeafNode(child_type* ch);
        ~BPlusTreeBaseLeafNode();
        void release_node(child_item_type* node);
        void set_childs(child_type* ch);
        child_type* get_childs();
        void insert(child_item_type* item);
        void insert(int index, childs_type_iterator s, childs_type_iterator e);
        child_item_type* erase(int index);
        void erase(childs_type_iterator s, childs_type_iterator e);
        inline bool is_leaf();
        bool exists(const Key& key);
        int get_index(const Key& key);
        int get_index(child_item_type* node);
        int size();
        child_item_type* get(int index);
        child_item_type* first_child();
        child_item_type* last_child();
        Key split(node_ptr node);
        void shift_left(node_ptr parent);
        void shift_right(node_ptr parent);
        void join_left(node_ptr parent);
        void join_right(node_ptr parent);
        void set_next_leaf(node_ptr node);
        void set_prev_leaf(node_ptr node);
        const Key get_key(child_item_type* item);
        node_ptr next_leaf();
        node_ptr prev_leaf();
        childs_type_iterator childs_iterator();

    protected:
        child_type* childs;
        node_ptr next_leaf_node = nullptr;
        node_ptr prev_leaf_node = nullptr;
};


template<class Key, class T>
BPlusTreeBaseLeafNode<Key, T>::BPlusTreeBaseLeafNode()
{
	childs = new child_type();
	//bbb++;
}

template<class Key, class T>
BPlusTreeBaseLeafNode<Key, T>::BPlusTreeBaseLeafNode(child_type* ch)
{
	set_childs(ch);
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::child_type* BPlusTreeBaseLeafNode<Key, T>::get_childs()
{
	return childs;
}

template<class Key, class T>
BPlusTreeBaseLeafNode<Key, T>::~BPlusTreeBaseLeafNode()
{
	//bbb--;
    for(int i=0;i<size();i++){
        release_node((*childs)[i]);
	}
	if(childs){
		delete childs;
	}
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::release_node(child_item_type* node)
{
    // TODO: replace with allocator methods
    delete node;
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::set_childs(child_type* ch)
{
	childs = ch;
}

template<class Key, class T>
inline bool BPlusTreeBaseLeafNode<Key, T>::is_leaf()
{
    return true;
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::child_item_type* BPlusTreeBaseLeafNode<Key, T>::erase(int ind)
{
    if(ind < 0){
        return nullptr;
	}
	child_item_type* deleted = (*childs)[ind];
    childs->erase(childs->begin()+ind);
    return deleted;
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::erase(childs_type_iterator s, childs_type_iterator e)
{
    childs->erase(s, e);
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::insert(int index, childs_type_iterator s, childs_type_iterator e)
{
    childs->insert(childs->begin()+index, s, e);
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::insert(child_item_type* item)
{
    int res = get_index(get_key(item));
    childs->insert(childs->begin()+res, item);
}

template<class Key, class T>
bool BPlusTreeBaseLeafNode<Key, T>::exists(const Key& key)
{
    int index = get_index(key);
    return index < size() && get_key((*childs)[index]) == key;
}

template<class Key, class T>
int BPlusTreeBaseLeafNode<Key, T>::get_index(child_item_type* node)
{
    return get_index(get_key(node));
}

template<class Key, class T>
int BPlusTreeBaseLeafNode<Key, T>::size()
{
	if(!childs)
		return 0;
    return childs->size();
}

template<class Key, class T>
int BPlusTreeBaseLeafNode<Key, T>::get_index(const Key& key)
{
    int mn=0,mx=size()-1,md;
    int res = size();
    while(mn<=mx){
        md = (mn+mx)/2;
        const Key& k = get_key((*childs)[md]);
        if(k < key){
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
typename BPlusTreeBaseLeafNode<Key, T>::child_item_type* BPlusTreeBaseLeafNode<Key, T>::get(int index)
{
    return (*childs)[index];
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::child_item_type* BPlusTreeBaseLeafNode<Key, T>::first_child()
{
    return get(0);
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::child_item_type* BPlusTreeBaseLeafNode<Key, T>::last_child()
{
    return get(size()-1);
}

template<class Key, class T>
Key BPlusTreeBaseLeafNode<Key, T>::split(node_ptr node)
{
    childs_type_iterator c_start = childs_iterator()+size()/2;
    childs_type_iterator c_end = childs_iterator()+size();
    node->insert(0, c_start, c_end);
    erase(c_start, c_end);
    return get_key(node->first_child());
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::shift_left(node_ptr parent)
{
    // Left node index
    int index = parent->get_index(this)-1;
    // Get shift node
    node_ptr prev = parent->get_node(index);
    // Shift item
    insert(prev->last_child());
    prev->erase(prev->size()-1);
    // Update key between shifted nodes
    *(parent->keys_iterator()+index) = get_key(first_child());
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::shift_right(node_ptr parent)
{
    // Left node index
    int index = parent->get_index(this)+1;
    // Get shift node
    node_ptr next = parent->get_node(index);
    // Shift item
    insert(next->first_child());
    next->erase(0);
    // Update key between shifted nodes
    *(parent->keys_iterator()+(index-1)) = get_key(next->first_child());
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::join_left(node_ptr parent)
{
    // Get join node index
    int index = parent->get_index(this)-1;
    // Get join node
    node_ptr prev = parent->get_node(index);
    // Move all items from join node
    insert(0, prev->childs_iterator(), prev->childs_iterator()+prev->size());
    prev->erase(prev->childs_iterator(), prev->childs_iterator()+prev->size());
    // Remove from parent
    parent->remove_keys(index);
    parent->remove_nodes(index);
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::join_right(node_ptr parent)
{
    // Get join node index
    int index = parent->get_index(this)+1;
    // Get join node
    node_ptr next = parent->get_node(index);
    // Move all items from join node
    insert(size(), next->childs_iterator(), next->childs_iterator()+next->size());
    next->erase(next->childs_iterator(), next->childs_iterator()+next->size());
    // Remove from parent
    parent->remove_keys(index-1);
    parent->remove_nodes(index);
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::set_next_leaf(node_ptr node)
{
    next_leaf_node = node;
}

template<class Key, class T>
void BPlusTreeBaseLeafNode<Key, T>::set_prev_leaf(node_ptr node)
{
    prev_leaf_node = node;
}

template<class Key, class T>
const Key BPlusTreeBaseLeafNode<Key, T>::get_key(child_item_type* item)
{
    return item->first;
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::node_ptr BPlusTreeBaseLeafNode<Key, T>::next_leaf()
{
    return next_leaf_node;
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::node_ptr BPlusTreeBaseLeafNode<Key, T>::prev_leaf()
{
    return prev_leaf_node;
}

template<class Key, class T>
typename BPlusTreeBaseLeafNode<Key, T>::childs_type_iterator BPlusTreeBaseLeafNode<Key, T>::childs_iterator()
{
    return childs->begin();
}


#endif // BPLUSTREEBASELEAFNODE_H
