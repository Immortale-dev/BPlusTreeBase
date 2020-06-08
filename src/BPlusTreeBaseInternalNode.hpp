#ifndef BPLUSTREEBASEINTERNALNODE_H
#define BPLUSTREEBASEINTERNALNODE_H

#include <vector>
#include <memory>
#include <functional>
#include "BPlusTreeBaseNode.hpp"

__B_PLUS_TREE_NODE_TEMPLATE__
class BPlusTreeBaseInternalNode : public __B_PLUS_TREE_BASENODE_CLASS__
{
    typedef __B_PLUS_TREE_BASENODE_CLASS__ Node;
    typedef __B_PLUS_TREE_BASEINTERNALNODE_CLASS__ InternalNode;
    typedef std::shared_ptr<Node> node_ptr;
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
        int get_index(const Key& key, bool to_lower = false);
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


__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASEINTERNALNODE_CLASS__::BPlusTreeBaseInternalNode()
{
	child_keys = new keys_type();
	child_nodes = new nodes_type();
}

__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASEINTERNALNODE_CLASS__::BPlusTreeBaseInternalNode(keys_type* keys, nodes_type* nodes)
{
	set_keys(keys);
	set_nodes(nodes);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::keys_type* __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::get_keys()
{
	return child_keys;
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::nodes_type* __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::get_nodes()
{
	return child_nodes;
}

__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASEINTERNALNODE_CLASS__::~BPlusTreeBaseInternalNode()
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

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::release_node(node_ptr node)
{
	if(node->is_leaf()){
		node->set_prev_leaf(nullptr);
		node->set_next_leaf(nullptr);
		auto childs = node->get_childs();
		if(!childs){
            return;
		}
		int childs_size = node->childs_size();
		for(int i=0;i<childs_size;i++){
			(*childs)[i]->node = nullptr;
		}
	}
    // TODO: replace with allocator methods
    // No need to delete smart ptr
    // delete node;
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::set_keys(keys_type* keys)
{
	child_keys = keys;
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::set_nodes(nodes_type* nodes)
{
	child_nodes = nodes;
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::add_keys(int ind, Key key)
{
    child_keys->insert(child_keys->begin()+ind, key);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::add_keys(int ind, keys_type_iterator s, keys_type_iterator e)
{
    child_keys->insert(child_keys->begin()+ind, s, e);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::remove_keys(int ind)
{
    child_keys->erase(child_keys->begin()+ind);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::remove_keys(keys_type_iterator s, keys_type_iterator e)
{
    child_keys->erase(s, e);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::add_nodes(int ind, node_ptr node)
{
    child_nodes->insert(child_nodes->begin()+ind, node);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e){
    child_nodes->insert(child_nodes->begin()+ind, s, e);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::remove_nodes(int ind)
{
    child_nodes->erase(child_nodes->begin()+ind);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::remove_nodes(nodes_type_iterator s, nodes_type_iterator e)
{
    child_nodes->erase(s, e);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::shift_right(node_ptr parent)
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

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::shift_left(node_ptr parent)
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

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::join_left(node_ptr parent)
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

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::join_right(node_ptr parent)
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

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::split(node_ptr node, node_ptr parent)
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

__B_PLUS_TREE_NODE_TEMPLATE__
inline bool __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::is_leaf()
{
    return false;
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::get_index(const Key& key, bool to_lower)
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

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::get_index(Node* node)
{
    int sz = nodes_size();
    for(int i=0;i<sz;i++){
        if((*child_nodes)[i].get() == node)
            return i;
    }
    return sz;
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::size()
{
	return nodes_size();
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::keys_size()
{
	return child_keys->size();
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::nodes_size()
{
	return child_nodes->size();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::node_ptr __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::find(const Key& key)
{
    int index = get_index(key);
    return get_node(index);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::node_ptr __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::find_next(const Key& key)
{
    int index = get_index(key);
    index++;
    if(index > nodes_size()-1)
        return nullptr;
    return get_node(index);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::node_ptr __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::find_prev(const Key& key)
{
    int index = get_index(key);
    index--;
    if(index < 0)
        return nullptr;
    return get_node(index);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::node_ptr __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::first_child_node()
{
    return get_node(0);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::node_ptr __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::last_child_node()
{
    return get_node(nodes_size()-1);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::node_ptr __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::get_node(int index)
{
    return (*child_nodes)[index];
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::keys_type_iterator __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::keys_iterator()
{
    return child_keys->begin();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::keys_type_iterator __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::keys_iterator_end()
{
    return child_keys->end();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::nodes_type_iterator __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::nodes_iterator()
{
    return child_nodes->begin();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::nodes_type_iterator __B_PLUS_TREE_BASEINTERNALNODE_CLASS__::nodes_iterator_end()
{
    return child_nodes->end();
}


#endif // BPLUSTREEBASEINTERNALNODE_H
