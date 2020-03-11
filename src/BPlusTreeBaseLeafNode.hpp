#ifndef BPLUSTREEBASELEAFNODE_H
#define BPLUSTREEBASELEAFNODE_H

#include "BPlusTreeBaseNode.hpp"
#include <utility>
#include <map>
#include <memory>


template<class Key, class T, class D>
class BPlusTreeBaseLeafNode : public BPlusTreeBaseNode<Key, T, D>
{	
    typedef BPlusTreeBaseNode<Key, T, D> Node;
    typedef std::shared_ptr<Node> node_ptr;
    typedef BPlusTreeBaseLeafNode<Key, T, D> LeafNode;
    typedef std::pair<const Key,T> record_type;
    typedef std::shared_ptr<record_type> record_type_ptr;
    typedef typename Node::child_item_type child_item_type;
    typedef std::shared_ptr<child_item_type> child_item_type_ptr;
    typedef std::vector<child_item_type_ptr> child_type;
    typedef typename child_type::iterator childs_type_iterator;

    public:
        BPlusTreeBaseLeafNode();
        BPlusTreeBaseLeafNode(child_type* ch);
        ~BPlusTreeBaseLeafNode();
        void release_node(child_item_type_ptr node);
        void set_childs(child_type* ch);
        child_type* get_childs();
        void insert(child_item_type_ptr item);
        void insert(record_type_ptr item);
        void insert(int index, childs_type_iterator s, childs_type_iterator e);
        child_item_type_ptr erase(int index);
        void erase(childs_type_iterator s, childs_type_iterator e);
        inline bool is_leaf();
        bool exists(const Key& key);
        int get_index(const Key& key);
        int get_index(child_item_type_ptr node);
        int size();
        int childs_size();
        child_item_type_ptr get(int index);
        child_item_type_ptr first_child();
        child_item_type_ptr last_child();
        void split(node_ptr node, node_ptr parent);
        void shift_left(node_ptr parent);
        void shift_right(node_ptr parent);
        void join_left(node_ptr parent);
        void join_right(node_ptr parent);
        void set_next_leaf(node_ptr node);
        void set_prev_leaf(node_ptr node);
        const Key get_key(child_item_type_ptr item);
        node_ptr next_leaf();
        node_ptr prev_leaf();
        childs_type_iterator childs_iterator();
        childs_type_iterator childs_iterator_end();
        void update_positions(node_ptr node);

    protected:
        child_type* childs;
        node_ptr next_leaf_node = nullptr;
        node_ptr prev_leaf_node = nullptr;
};


template<class Key, class T, class D>
BPlusTreeBaseLeafNode<Key, T, D>::BPlusTreeBaseLeafNode()
{
	childs = new child_type();
}

template<class Key, class T, class D>
BPlusTreeBaseLeafNode<Key, T, D>::BPlusTreeBaseLeafNode(child_type* ch)
{
	set_childs(ch);
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::child_type* BPlusTreeBaseLeafNode<Key, T, D>::get_childs()
{
	return childs;
}

template<class Key, class T, class D>
BPlusTreeBaseLeafNode<Key, T, D>::~BPlusTreeBaseLeafNode()
{
	if(childs){
		for(int i=0;i<childs_size();i++){
			release_node((*childs)[i]);
		}
		delete childs;
	}
	set_prev_leaf(nullptr);
	set_next_leaf(nullptr);
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::release_node(child_item_type_ptr node)
{
    // TODO: replace with allocator methods
    // No need to delete shared ptr
    // delete node;
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::set_childs(child_type* ch)
{
	childs = ch;
}

template<class Key, class T, class D>
inline bool BPlusTreeBaseLeafNode<Key, T, D>::is_leaf()
{
    return true;
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::child_item_type_ptr BPlusTreeBaseLeafNode<Key, T, D>::erase(int ind)
{
    if(ind < 0){
        return nullptr;
	}
	child_item_type_ptr deleted = (*childs)[ind];
    childs->erase(childs->begin()+ind);
    return deleted;
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::erase(childs_type_iterator s, childs_type_iterator e)
{
    childs->erase(s, e);
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::insert(int index, childs_type_iterator s, childs_type_iterator e)
{
    childs->insert(childs->begin()+index, s, e);
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::insert(child_item_type_ptr item)
{
    int res = get_index(get_key(item));
    childs->insert(childs->begin()+res, item);
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::insert(record_type_ptr item)
{
	child_item_type* ch = new child_item_type();
	ch->item = item;
    insert(child_item_type_ptr(ch));
}

template<class Key, class T, class D>
bool BPlusTreeBaseLeafNode<Key, T, D>::exists(const Key& key)
{
    int index = get_index(key);
    return index < childs_size() && get_key((*childs)[index]) == key;
}

template<class Key, class T, class D>
int BPlusTreeBaseLeafNode<Key, T, D>::get_index(child_item_type_ptr node)
{
    return get_index(get_key(node));
}

template<class Key, class T, class D>
int BPlusTreeBaseLeafNode<Key, T, D>::size()
{
	return childs_size();
}

template<class Key, class T, class D>
int BPlusTreeBaseLeafNode<Key, T, D>::childs_size()
{
	return childs->size();
}

template<class Key, class T, class D>
int BPlusTreeBaseLeafNode<Key, T, D>::get_index(const Key& key)
{
    int mn=0,mx=childs_size()-1,md;
    int res = childs_size();
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

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::child_item_type_ptr BPlusTreeBaseLeafNode<Key, T, D>::get(int index)
{
    return (*childs)[index];
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::child_item_type_ptr BPlusTreeBaseLeafNode<Key, T, D>::first_child()
{
    return get(0);
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::child_item_type_ptr BPlusTreeBaseLeafNode<Key, T, D>::last_child()
{
    return get(childs_size()-1);
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::split(node_ptr node, node_ptr parent)
{
	Key ret;
	childs_type_iterator c_start, c_end;
	int index = parent->get_index(this);
	bool nextIns = false;
	if(parent->first_child_node().get() == this){
		c_start = childs_iterator() + childs_size()/2;
		c_end = childs_iterator_end();
		// Add items to parent node
		parent->add_nodes(index+1, node);
		nextIns = true;
	}
	else{
		c_start = childs_iterator();
		c_end = childs_iterator() + childs_size()/2;
		// Add items to parent node
		parent->add_nodes(index, node);
	}
	
	node->insert(0, c_start, c_end);
    erase(c_start, c_end);
	
	if(nextIns){
		ret = get_key(node->first_child());
	}
	else{
		ret = get_key(first_child());
	}
    
	parent->add_keys(index, ret);
    node->update_positions(node);
    // Custom update pos
    for(int i=0;i<childs_size();i++){
		(*childs)[i]->pos = i;
	}
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::shift_left(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Left node index
    int index = current_index-1;
    // Get shift node
    node_ptr prev = parent->get_node(index);
    // Shift item
    insert(prev->last_child());
    prev->erase(prev->childs_size()-1);
    // Update key between shifted nodes
    *(parent->keys_iterator()+index) = get_key(first_child());
    // Update current node positions
    update_positions(parent->get_node(current_index));
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::shift_right(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Left node index
    int index = current_index+1;
    // Get shift node
    node_ptr next = parent->get_node(index);
    // Shift item
    insert(next->first_child());
    next->erase(0);
    // Update key between shifted nodes
    *(parent->keys_iterator()+(index-1)) = get_key(next->first_child());
    // Update positions
    next->update_positions(next);
    update_positions(parent->get_node(current_index));
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::join_left(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Get join node index
    int index = current_index-1;
    // Get join node
    node_ptr prev = parent->get_node(index);
    // Move all items from join node
    insert(0, prev->childs_iterator(), prev->childs_iterator_end());
    prev->erase(prev->childs_iterator(), prev->childs_iterator_end());
    // Remove from parent
    parent->remove_keys(index);
    parent->remove_nodes(index);
    // Update positions
    update_positions(parent->get_node(current_index));
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::join_right(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Get join node index
    int index = current_index+1;
    // Get join node
    node_ptr next = parent->get_node(index);
    // Move all items from join node
    insert(childs_size(), next->childs_iterator(), next->childs_iterator_end());
    next->erase(next->childs_iterator(), next->childs_iterator_end());
    // Remove from parent
    parent->remove_keys(index-1);
    parent->remove_nodes(index);
    // Update positions
    update_positions(parent->get_node(current_index));
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::set_next_leaf(node_ptr node)
{
    next_leaf_node = node;
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::set_prev_leaf(node_ptr node)
{
    prev_leaf_node = node;
}

template<class Key, class T, class D>
const Key BPlusTreeBaseLeafNode<Key, T, D>::get_key(child_item_type_ptr item)
{
    return item->item->first;
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::node_ptr BPlusTreeBaseLeafNode<Key, T, D>::next_leaf()
{
    return next_leaf_node;
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::node_ptr BPlusTreeBaseLeafNode<Key, T, D>::prev_leaf()
{
    return prev_leaf_node;
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::childs_type_iterator BPlusTreeBaseLeafNode<Key, T, D>::childs_iterator()
{
    return childs->begin();
}

template<class Key, class T, class D>
typename BPlusTreeBaseLeafNode<Key, T, D>::childs_type_iterator BPlusTreeBaseLeafNode<Key, T, D>::childs_iterator_end()
{
    return childs->end();
}

template<class Key, class T, class D>
void BPlusTreeBaseLeafNode<Key, T, D>::update_positions(node_ptr node)
{
	if(!childs){
		return;
	}
	for(int i=0;i<childs_size();i++){
		(*childs)[i]->pos = i;
		(*childs)[i]->node = node;
	}
}


#endif // BPLUSTREEBASELEAFNODE_H
